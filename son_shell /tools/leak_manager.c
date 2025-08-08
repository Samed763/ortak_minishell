/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   leak_manager.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 16:09:43 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/08 09:44:03 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	free_redir_list(t_redir *head)
{
	t_redir	*current;
	t_redir	*next;

	current = head;
	while (current)
	{
		next = current->next;
		free(current->filename);
		free(current);
		current = next;
	}
}

void	free_heredoc_lines(t_heredoc_line *head)
{
	t_heredoc_line	*current;
	t_heredoc_line	*next;

	current = head;
	while (current)
	{
		next = current->next;
		free(current->content);
		free(current);
		current = next;
	}
}

void	free_heredoc_list(t_heredoc *head)
{
	t_heredoc	*current;
	t_heredoc	*next;

	current = head;
	while (current)
	{
		next = current->next;
		free(current->delimiter);
		free_heredoc_lines(current->lines);
		free(current);
		current = next;
	}
}

/*
** GÜNCELLENDİ: free_command_list
** Artık yeni `free_redir_list` fonksiyonunu çağırarak
** yönlendirme listesini de temizliyor. Eski gereksiz free'ler kaldırıldı.
*/
void	free_command_list(t_command *head)
{
	t_command	*current;
	t_command	*next;

	current = head;
	while (current)
	{
		next = current->next;
		free_word_array(current->args);
		free_redir_list(current->redirs); // YENİ: Yönlendirme listesini temizle.
		free_heredoc_list(current->heredocs);
		free(current);
		current = next;
	}
}

void	free_data_resources(t_data *data)
{
	if (!data)
		return ;
	if (data->word_array)
		free_word_array(data->word_array);
	data->word_array = NULL;
	if (data->token)
		free(data->token);
	data->token = NULL;
	if (data->cmd)
		free_command_list(data->cmd);
	data->cmd = NULL;
}