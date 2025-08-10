/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apply_redir2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 13:01:00 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/10 15:12:40 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
# include <unistd.h>
# include <stdio.h>

static void	write_heredoc_to_pipe(int write_fd, t_heredoc *heredoc)
{
	t_heredoc_line	*current_line;

	current_line = heredoc->lines;
	while (current_line)
	{
		write(write_fd, current_line->content,
			ft_strlen(current_line->content));
		write(write_fd, "\n", 1);
		current_line = current_line->next;
	}
}

int	apply_specific_heredoc(t_heredoc *heredoc)
{
	int	pipefd[2];

	if (!heredoc)
		return (0);
	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return (-1);
	}
	write_heredoc_to_pipe(pipefd[1], heredoc);
	close(pipefd[1]);
	if (dup2(pipefd[0], STDIN_FILENO) == -1)
	{
		perror("dup2");
		close(pipefd[0]);
		return (-1);
	}
	close(pipefd[0]);
	return (0);
}

void	write_error_and_exit(int exit_val, char *arg, char *error)
{
	write(2, "minishell: ", 11);
	write(2, arg, ft_strlen(arg));
	write(2, ": ", 2);
	write(2, error, ft_strlen(error));
	write(2, "\n", 1);
	cleanup_and_exit(exit_val);
}

void	check_error(int access_ret, t_data *data)
{
	if (access_ret == -1)
		write_error_and_exit(127, data->cmd->args[0], "command not found");
	else if (access_ret == -2)
		write_error_and_exit(126, data->cmd->args[0], "Permission denied");
	else if (access_ret == -3)
		write_error_and_exit(126, data->cmd->args[0], "Is a directory");
	else if (access_ret == -4)
		write_error_and_exit(127, data->cmd->args[0],
			"No such file or directory");
}
