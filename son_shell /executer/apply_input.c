/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apply_input.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 19:42:50 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/06 18:44:34 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	apply_heredoc_input(t_heredoc *heredoc)
{
	int				pipefd[2];
	t_heredoc_line	*current_line;

	if (pipe(pipefd) == -1)
		return (-1);
	current_line = heredoc->lines;
	while (current_line)
	{
		write(pipefd[1], current_line->content,
			ft_strlen(current_line->content));
		write(pipefd[1], "\n", 1);
		current_line = current_line->next;
	}
	close(pipefd[1]);
	if (dup2(pipefd[0], STDIN_FILENO) == -1)
	{
		close(pipefd[0]);
		return (-1);
	}
	close(pipefd[0]);
	return (0);
}

int	apply_input_redirection(t_data *data, t_command *cmd)
{
	int			fd;
	t_heredoc	*current_heredoc;
	t_heredoc	*last_heredoc;

	(void)data;
	if (cmd->input_files && *cmd->input_files)
	{
		fd = open(cmd->input_files, O_RDONLY);
		if (fd == -1)
		{
			perror(cmd->input_files);
			return (-1);
		}
		if (dup2(fd, STDIN_FILENO) == -1)
		{
			close(fd);
			return (-1);
		}
		close(fd);
	}
	else if (cmd->heredocs)
	{
		current_heredoc = cmd->heredocs;
		last_heredoc = current_heredoc;
		while (current_heredoc)
		{
			last_heredoc = current_heredoc;
			current_heredoc = current_heredoc->next;
		}
		return (apply_heredoc_input(last_heredoc));
	}
	return (0);
}
