/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apply_redir.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 22:10:08 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/15 20:32:01 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <stdio.h>
#include <unistd.h>

static int	apply_output_redirection(t_redir *redir)
{
	int	fd;
	int	open_flags;

	if (redir->type == TOKEN_APPEND)
		open_flags = O_WRONLY | O_CREAT | O_APPEND;
	else
		open_flags = O_WRONLY | O_CREAT | O_TRUNC;
	fd = open(redir->filename, open_flags, 0644);
	if (fd == -1)
	{
		perror(redir->filename);
		return (-1);
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		perror("dup2");
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

static int	apply_input_file_redirection(t_redir *redir)
{
	int	fd;

	fd = open(redir->filename, O_RDONLY);
	if (fd == -1)
	{
		perror(redir->filename);
		return (-1);
	}
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		perror("dup2");
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

static int	process_single_redirection(t_redir *redir, t_heredoc **heredoc_iter)
{
	if (redir->type == TOKEN_REDIRECT_OUT || redir->type == TOKEN_APPEND)
	{
		if (apply_output_redirection(redir) == -1)
			return (-1);
	}
	else if (redir->type == TOKEN_REDIRECT_IN)
	{
		if (apply_input_file_redirection(redir) == -1)
			return (-1);
	}
	else if (redir->type == TOKEN_HEREDOC)
	{
		if (apply_specific_heredoc(*heredoc_iter) == -1)
			return (-1);
		if (*heredoc_iter)
			*heredoc_iter = (*heredoc_iter)->next;
	}
	return (0);
}

int	handle_redirections(t_command *cmd)
{
	t_redir		*redir_iter;
	t_heredoc	*heredoc_iter;

	redir_iter = cmd->redirs;
	heredoc_iter = cmd->heredocs;
	while (redir_iter)
	{
		if (process_single_redirection(redir_iter, &heredoc_iter) == -1)
			return (-1);
		redir_iter = redir_iter->next;
	}
	return (0);
}
