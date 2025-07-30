/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_execute.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 19:44:56 by sadinc            #+#    #+#             */
/*   Updated: 2025/07/30 19:51:54 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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

static void	execute_pipe_loop(t_pipe_data *p_data)
{
	pid_t	pid;

	p_data->current = p_data->data->cmd;
	p_data->prev_fd = -1;
	while (p_data->current)
	{
		if (p_data->current->next && pipe(p_data->pipefd) == -1)
		{
			perror("pipe");
			return ;
		}
		pid = fork_and_execute(p_data);
		if (pid == -1)
			return ;
		p_data->prev_fd = pipe_parent_routine(p_data->current, p_data->pipefd,
				p_data->prev_fd);
		p_data->current = p_data->current->next;
	}
	if (p_data->prev_fd != -1)
		close(p_data->prev_fd);
}

void	pipe_execute(t_data *data, char **splitted_path)
{
	t_pipe_data	p_data;
	int			pipefd[2];

	p_data.data = data;
	p_data.splitted_path = splitted_path;
	p_data.pipefd = pipefd;
	execute_pipe_loop(&p_data);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	wait_for_all_children(data);
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, SIG_IGN);
}
