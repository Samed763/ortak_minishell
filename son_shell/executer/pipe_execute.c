/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_execute.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 19:44:56 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/13 18:41:02 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
# include <signal.h>
# include <stdio.h>
# include <unistd.h>

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
 * Initialize pipe file descriptors for all commands
 */
static int	init_all_pipes(t_data *data)
{
	t_command	*cmd;

	cmd = data->cmd;
	while (cmd)
	{
		cmd->pipe_fd[0] = -1;
		cmd->pipe_fd[1] = -1;
		if (cmd->next)
		{
			if (pipe(cmd->pipe_fd) == -1)
			{
				perror("pipe");
				return (-1);
			}
		}
		cmd = cmd->next;
	}
	return (0);
}

/**
 * Fork and execute a single command in the pipe chain
 */
static pid_t	fork_and_execute(t_pipe_data *p_data)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (-1);
	}
	else if (pid == 0)
		pipe_child_routine(p_data);
	return (pid);
}

/**
 * Close appropriate pipe ends in parent after forking
 */
static void	parent_close_pipe_ends(t_command *cmd, t_command *prev_cmd)
{
	// Close the read end of the previous pipe (already read by current child)
	if (prev_cmd && prev_cmd->pipe_fd[0] != -1)
		safe_close(&prev_cmd->pipe_fd[0]);
	
	// Close the write end of the current pipe (written by current child)
	if (cmd->next && cmd->pipe_fd[1] != -1)
		safe_close(&cmd->pipe_fd[1]);
}

/**
 * Execute the pipe chain
 */
static void	execute_pipe_loop(t_pipe_data *p_data)
{
	pid_t		pid;
	t_command	*prev_cmd;

	if (init_all_pipes(p_data->data) == -1)
		return ;
	
	p_data->current = p_data->data->cmd;
	prev_cmd = NULL;
	
	while (p_data->current)
	{
		p_data->prev_cmd = prev_cmd;
		pid = fork_and_execute(p_data);
		if (pid == -1)
		{
			cleanup_pipe_fds(p_data->data);
			return ;
		}
		
		p_data->current->pid = pid;
		parent_close_pipe_ends(p_data->current, prev_cmd);
		
		prev_cmd = p_data->current;
		p_data->current = p_data->current->next;
	}
	
	// Close any remaining pipe file descriptors
	cleanup_pipe_fds(p_data->data);
}

/**
 * Main pipe execution function
 */
void	pipe_execute(t_data *data)
{
	t_pipe_data	p_data;

	p_data.data = data;
	p_data.current = NULL;
	p_data.prev_cmd = NULL;
	
	execute_pipe_loop(&p_data);
	
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	wait_for_all_children(data);
	setup_signals();
}
