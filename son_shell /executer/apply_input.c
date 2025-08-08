/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apply_input.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 19:42:50 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/08 09:58:12 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	apply_heredoc_input(t_heredoc *heredoc)
{
	int				pipefd[2];
	t_heredoc_line	*current_line;

	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return (-1);
	}
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
		perror("dup2");
		close(pipefd[0]);
		return (-1);
	}
	close(pipefd[0]);
	return (0);
}

// Bu fonksiyon, bir komut için tüm girdi yönlendirmelerini uygular.
// Standart shell davranışına uygun olarak, en son girdi yönlendirmesini bulur
// ve sadece onu uygular.
int	apply_input_redirection(t_command *cmd)
{
	t_redir		*redir;
	t_redir		*last_in_redir;
	t_heredoc	*heredoc_iter;
	int			heredoc_idx;
	int			target_heredoc_idx;
	int			fd;

	if (!cmd || !cmd->redirs)
		return (0);
	last_in_redir = NULL;
	target_heredoc_idx = 0;
	redir = cmd->redirs;
	// 1. Son girdi yönlendirmesini bul ve o ana kadarki heredoc sayısını say.
	//    handle_all_heredocs zaten tüm heredoc'ları okur. Bizim görevimiz
	//    doğru olanı STDIN'e bağlamak.
	while (redir)
	{
		if (redir->type == TOKEN_HEREDOC)
			target_heredoc_idx++;
		if (redir->type == TOKEN_REDIRECT_IN || redir->type == TOKEN_HEREDOC)
			last_in_redir = redir;
		redir = redir->next;
	}
	if (!last_in_redir)
		return (0);
	// 2. Son yönlendirmeyi uygula.
	if (last_in_redir->type == TOKEN_REDIRECT_IN)
	{
		fd = open(last_in_redir->filename, O_RDONLY);
		if (fd == -1)
		{
			perror(last_in_redir->filename);
			return (-1);
		}
		if (dup2(fd, STDIN_FILENO) == -1)
		{
			close(fd);
			perror("dup2");
			return (-1);
		}
		close(fd);
	}
	else if (last_in_redir->type == TOKEN_HEREDOC)
	{
		// Doğru heredoc'u bul ve içeriğini pipe aracılığıyla STDIN'e yönlendir.
		heredoc_iter = cmd->heredocs;
		heredoc_idx = 1;
		while (heredoc_iter && heredoc_idx < target_heredoc_idx)
		{
			heredoc_iter = heredoc_iter->next;
			heredoc_idx++;
		}
		if (heredoc_iter)
			return (apply_heredoc_input(heredoc_iter));
	}
	return (0);
}