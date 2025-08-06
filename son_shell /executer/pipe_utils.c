/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 19:53:31 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/06 18:53:50 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	pipe_child_routine(t_pipe_data *p_data)
{
	char	*full_path;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	handle_pipe_redirections(p_data->data, p_data->current, p_data->pipefd,
		p_data->prev_fd);
	if (!p_data->current->args || !p_data->current->args[0])
		cleanup_and_exit(p_data->data, 0);
	if (try_builtin(p_data->current, p_data->data, 0))
		cleanup_and_exit(p_data->data, p_data->data->exit_value);
	if (is_accessable(p_data->current->args[0], p_data->data->splitted_path,
			&full_path) == -1)
	{
		write(2, p_data->current->args[0], ft_strlen(p_data->current->args[0]));
		write(2, ": command not found\n", 20);
		cleanup_and_exit(p_data->data, 127);
	}
	if (execve(full_path, p_data->current->args, p_data->data->env) == -1)
	{
		perror("execve");
		free(full_path);
		cleanup_and_exit(p_data->data, 1);
	}
}

int	pipe_parent_routine(t_command *current, int *pipefd, int prev_fd)
{
	if (prev_fd != -1)
		close(prev_fd);
	if (current->next)
	{
		close(pipefd[1]);
		return (pipefd[0]);
	}
	return (-1);
}

void	handle_pipe_redirections(t_data *data, t_command *current, int *pipefd,
		int prev_fd)
{
	if (prev_fd != -1)
	{
		safe_dup2(prev_fd, STDIN_FILENO, data);
		close(prev_fd);
	}
	else
	{
		if (apply_input_redirection(current) == -1)
			cleanup_and_exit(data, 1);
	}
	if (current->next)
	{
		close(pipefd[0]);
		safe_dup2(pipefd[1], STDOUT_FILENO, data);
		close(pipefd[1]);
	}
	else
	{
		if (apply_output_redirection(current) == -1)
			cleanup_and_exit(data, 1);
	}
}

void	wait_for_all_children(t_data *data)
{
	t_command	*current;
	int			status;
	int			last_status;

	if (!data || !data->cmd)
		return ;
	current = data->cmd;
	last_status = 0;
	while (current)
	{
		wait(&status);
		last_status = status;
		current = current->next;
	}
	set_exit_status(data, last_status);
}
