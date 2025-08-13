/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 19:53:31 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/13 18:41:02 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
# include <signal.h>
# include <unistd.h>
# include <sys/wait.h>
# include <stdio.h>

/**
 * Safely close file descriptor and set to -1
 */
static void	safe_close(int *fd)
{
	if (*fd != -1)
	{
		close(*fd);
		*fd = -1;
	}
}

/**
 * Close all pipe file descriptors for cleanup
 */
void	cleanup_pipe_fds(t_data *data)
{
	t_command	*cmd;

	if (!data || !data->cmd)
		return ;
	
	cmd = data->cmd;
	while (cmd)
	{
		safe_close(&cmd->pipe_fd[0]);
		safe_close(&cmd->pipe_fd[1]);
		cmd = cmd->next;
	}
}

/**
 * Close all pipe file descriptors except those needed by current command
 */
static void	close_unused_pipe_fds(t_pipe_data *p_data)
{
	t_command	*cmd;

	cmd = p_data->data->cmd;
	while (cmd)
	{
		// Don't close pipes that this command needs
		if (cmd == p_data->current)
		{
			cmd = cmd->next;
			continue ;
		}
		
		// Close all other pipe file descriptors
		safe_close(&cmd->pipe_fd[0]);
		safe_close(&cmd->pipe_fd[1]);
		cmd = cmd->next;
	}
}

/**
 * Setup input/output redirection for pipes in child process
 */
static int	setup_pipe_io(t_pipe_data *p_data)
{
	// Setup input from previous command's pipe
	if (p_data->prev_cmd && p_data->prev_cmd->pipe_fd[0] != -1)
	{
		if (dup2(p_data->prev_cmd->pipe_fd[0], STDIN_FILENO) == -1)
		{
			perror("dup2 stdin");
			return (-1);
		}
		safe_close(&p_data->prev_cmd->pipe_fd[0]);
	}
	
	// Setup output to current command's pipe
	if (p_data->current->next && p_data->current->pipe_fd[1] != -1)
	{
		if (dup2(p_data->current->pipe_fd[1], STDOUT_FILENO) == -1)
		{
			perror("dup2 stdout");
			return (-1);
		}
		safe_close(&p_data->current->pipe_fd[1]);
	}
	
	return (0);
}

/**
 * Child process routine for pipe execution
 */
void	pipe_child_routine(t_pipe_data *p_data)
{
	char	*full_path;
	int		access_ret;
	 
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	
	// Setup pipe input/output first
	if (setup_pipe_io(p_data) == -1)
		cleanup_and_exit(1);
	
	// Close all unused pipe file descriptors in child
	close_unused_pipe_fds(p_data);
	
	// Handle file redirections (these override pipe redirections)
	if (handle_redirections(p_data->current) == -1)
		cleanup_and_exit(1);
	
	// Check if command is empty
	if (!p_data->current->args || !p_data->current->args[0])
		cleanup_and_exit(0);
	
	// Try to execute builtin command
	if (try_builtin(p_data->current, p_data->data, 0))
		cleanup_and_exit(p_data->data->exit_value);
	
	// Execute external command
	access_ret = is_accessable(p_data->current->args[0], 
		p_data->data->splitted_path, &full_path);
	check_error(access_ret, p_data->data);
	
	if (execve(full_path, p_data->current->args, p_data->data->env) == -1)
	{
		perror("execve");
		free(full_path);
		cleanup_and_exit(1);
	}
}

/**
 * Count commands and get last PID for proper exit status handling
 */
static int	pid_and_count(t_command *cmd, pid_t *last_pid)
{
	t_command	*iter;
	int			count;

	iter = cmd;
	count = 0;
	*last_pid = -1;
	while (iter)
	{
		count++;
		if (iter->next == NULL)
			*last_pid = iter->pid;
		iter = iter->next;
	}
	return (count);
}

/**
 * Wait for all child processes to complete
 */
void	wait_for_all_children(t_data *data)
{
	int		cmd_count;
	pid_t	last_pid;
	pid_t	waited_pid;
	int		status;
	int		last_status;

	if (!data || !data->cmd)
		return ;
	
	cmd_count = pid_and_count(data->cmd, &last_pid);
	last_status = 0;
	
	while (cmd_count > 0)
	{
		waited_pid = wait(&status);
		if (waited_pid == -1)
			break ;
			
		if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
			write(STDOUT_FILENO, "\n", 1);
		
		if (waited_pid == last_pid)
			last_status = status;
		
		cmd_count--;
	}
	
	set_exit_status(data, last_status);
}
