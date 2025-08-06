/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apply_input.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 19:42:50 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/06 18:53:09 by sadinc           ###   ########.fr       */
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

static int	handle_file_input(t_command *cmd)
{
	int	fd;

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
	return (0);
}

static int	handle_heredoc_input(t_command *cmd)
{
	t_heredoc	*current_heredoc;
	t_heredoc	*last_heredoc;

	current_heredoc = cmd->heredocs;
	last_heredoc = current_heredoc;
	while (current_heredoc)
	{
		last_heredoc = current_heredoc;
		current_heredoc = current_heredoc->next;
	}
	return (apply_heredoc_input(last_heredoc));
}

int	apply_input_redirection(t_command *cmd)
{
	if (cmd->input_files && *cmd->input_files)
		return (handle_file_input(cmd));
	else if (cmd->heredocs)
		return (handle_heredoc_input(cmd));
	return (0);
}
