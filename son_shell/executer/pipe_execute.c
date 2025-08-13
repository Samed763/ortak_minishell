/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_execute.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 19:44:56 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/13 20:30:00 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
# include <signal.h>
# include <stdio.h>
# include <unistd.h>

/**
 * Check if command has heredoc input redirection
 */
static int	has_heredoc_input(t_command *cmd)
{
	t_redir	*redir;

	if (!cmd || !cmd->redirs)
		return (0);
	
	redir = cmd->redirs;
	while (redir)
	{
		if (redir->type == TOKEN_HEREDOC)
			return (1);
		redir = redir->next;
	}
	return (0);
}

/**
 * Count the number of commands in the pipe chain
 */
static int	count_commands(t_command *cmd)
{
	int	count;

	count = 0;
	while (cmd)
	{
		count++;
		cmd = cmd->next;
	}
	return (count);
}

/**
 * Execute a single command in the pipe chain
 */
static void	execute_single_command(t_command *cmd, t_data *data, int input_fd, int output_fd)
{
	char	*full_path;
	int		access_ret;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGPIPE, SIG_DFL);  // Handle SIGPIPE properly

	// Setup input redirection from pipe (will be overridden by heredoc if present)
	if (input_fd != STDIN_FILENO && input_fd >= 0)
	{
		if (dup2(input_fd, STDIN_FILENO) == -1)
		{
			perror("dup2 input");
			cleanup_and_exit(1);
		}
		close(input_fd);
	}

	// Setup output redirection to pipe (will be overridden by file redirections if present)
	if (output_fd != STDOUT_FILENO && output_fd >= 0)
	{
		if (dup2(output_fd, STDOUT_FILENO) == -1)
		{
			perror("dup2 output");
			cleanup_and_exit(1);
		}
		close(output_fd);
	}

	// Handle file redirections - these override pipe redirections
	// This is important: heredoc will override the pipe input we set above
	if (handle_redirections(cmd) == -1)
		cleanup_and_exit(1);

	// Check if command is empty
	if (!cmd->args || !cmd->args[0])
		cleanup_and_exit(0);

	// Try to execute builtin command
	if (try_builtin(cmd, data, 0))
		cleanup_and_exit(data->exit_value);

	// Execute external command
	access_ret = is_accessable(cmd->args[0], data->splitted_path, &full_path);
	check_error(access_ret, data);

	if (execve(full_path, cmd->args, data->env) == -1)
	{
		perror("execve");
		free(full_path);
		cleanup_and_exit(1);
	}
}

/**
 * Create pipes selectively - skip pipes where the next command has heredoc
 */
static int	**create_pipes_selective(t_data *data, int cmd_count)
{
	int			**pipes;
	int			i;
	t_command	*cmd;

	if (cmd_count <= 1)
		return (NULL);

	pipes = malloc(sizeof(int *) * (cmd_count - 1));
	if (!pipes)
		return (NULL);

	// Initialize all to NULL
	i = 0;
	while (i < cmd_count - 1)
	{
		pipes[i] = NULL;
		i++;
	}

	// Create pipes only where needed
	cmd = data->cmd;
	i = 0;
	while (cmd && i < cmd_count - 1)
	{
		// Don't create a pipe if the next command has heredoc
		if (cmd->next && has_heredoc_input(cmd->next))
		{
			pipes[i] = NULL;  // No pipe needed
		}
		else
		{
			pipes[i] = malloc(sizeof(int) * 2);
			if (!pipes[i] || pipe(pipes[i]) == -1)
			{
				perror("pipe");
				// Cleanup already created pipes
				while (--i >= 0)
				{
					if (pipes[i])
					{
						close(pipes[i][0]);
						close(pipes[i][1]);
						free(pipes[i]);
					}
				}
				free(pipes);
				return (NULL);
			}
		}
		cmd = cmd->next;
		i++;
	}
	return (pipes);
}

/**
 * Close all pipe file descriptors in parent process
 */
static void	close_all_pipes(int **pipes, int pipe_count)
{
	int	i;

	if (!pipes)
		return;

	i = 0;
	while (i < pipe_count)
	{
		if (pipes[i])
		{
			if (pipes[i][0] != -1)
				close(pipes[i][0]);
			if (pipes[i][1] != -1)
				close(pipes[i][1]);
			free(pipes[i]);
		}
		i++;
	}
	free(pipes);
}

/**
 * Execute the entire pipe chain
 */
void	pipe_execute(t_data *data)
{
	t_command	*cmd;
	int			**pipes;
	int			cmd_count;
	int			i;
	pid_t		pid;
	int			input_fd;
	int			output_fd;
	int			devnull_fd;

	cmd_count = count_commands(data->cmd);
	pipes = create_pipes_selective(data, cmd_count);
	
	if (cmd_count > 1 && !pipes)
	{
		data->exit_value = 1;
		return;
	}

	// Pre-open /dev/null to avoid opening it multiple times
	devnull_fd = -1;

	cmd = data->cmd;
	i = 0;
	while (cmd)
	{
		// Determine input file descriptor
		if (i == 0)
			input_fd = STDIN_FILENO;
		else if (pipes && pipes[i - 1])
			input_fd = pipes[i - 1][0];
		else
			input_fd = STDIN_FILENO;  // No pipe from previous command

		// Determine output file descriptor
		if (i == cmd_count - 1)
			output_fd = STDOUT_FILENO;
		else if (cmd->next && has_heredoc_input(cmd->next))
		{
			// Next command has heredoc, redirect this command's output to /dev/null
			if (devnull_fd == -1)
			{
				devnull_fd = open("/dev/null", O_WRONLY);
				if (devnull_fd == -1)
				{
					perror("open /dev/null");
					close_all_pipes(pipes, cmd_count - 1);
					data->exit_value = 1;
					return;
				}
			}
			output_fd = devnull_fd;
		}
		else if (pipes && pipes[i])
			output_fd = pipes[i][1];
		else
			output_fd = STDOUT_FILENO;  // No pipe to next command

		pid = fork();
		if (pid == -1)
		{
			perror("fork");
			if (devnull_fd != -1)
				close(devnull_fd);
			close_all_pipes(pipes, cmd_count - 1);
			data->exit_value = 1;
			return;
		}
		else if (pid == 0)
		{
			// Child process - close all other pipes except the ones we need
			if (pipes)
			{
				int j = 0;
				while (j < cmd_count - 1)
				{
					if (pipes[j])
					{
						// Keep input pipe open if it's ours
						if (j != i - 1 && pipes[j][0] != -1)
						{
							close(pipes[j][0]);
							pipes[j][0] = -1;
						}
						// Keep output pipe open if it's ours
						if (j != i && pipes[j][1] != -1)
						{
							close(pipes[j][1]);
							pipes[j][1] = -1;
						}
					}
					j++;
				}
			}
			
			execute_single_command(cmd, data, input_fd, output_fd);
		}
		else
		{
			// Parent process - store PID
			cmd->pid = pid;
		}

		cmd = cmd->next;
		i++;
	}

	// Parent closes /dev/null if it was opened
	if (devnull_fd != -1)
		close(devnull_fd);

	// Parent closes all pipes
	close_all_pipes(pipes, cmd_count - 1);

	// Wait for all children
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	wait_for_all_children(data);
	setup_signals();
}
