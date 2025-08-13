/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 19:53:31 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/13 20:30:00 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
# include <signal.h>
# include <unistd.h>
# include <sys/wait.h>
# include <stdio.h>

/**
 * Cleanup pipe file descriptors (now mostly unused but kept for compatibility)
 */
void	cleanup_pipe_fds(t_data *data)
{
	t_command	*cmd;

	if (!data || !data->cmd)
		return ;
	
	cmd = data->cmd;
	while (cmd)
	{
		if (cmd->pipe_fd[0] != -1)
		{
			close(cmd->pipe_fd[0]);
			cmd->pipe_fd[0] = -1;
		}
		if (cmd->pipe_fd[1] != -1)
		{
			close(cmd->pipe_fd[1]);
			cmd->pipe_fd[1] = -1;
		}
		cmd = cmd->next;
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
