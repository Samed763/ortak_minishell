/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 17:18:03 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/09 22:58:45 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	handle_redirections_2(t_data *data, t_command *current, int *i)
{
	char	*raw_word;
	int		token_type;

	token_type = data->token[*i];
	(*i)++;
	if (!data->word_array[*i]) // Token'dan sonra dosya adı yoksa sentaks hatasıdır.
	{
		printf("syntax error near unexpected token\n");
		return (-1);
	}
	raw_word = data->word_array[*i];
	if (token_type == TOKEN_HEREDOC)
	{
		// add_heredoc_to_command fonksiyonunu, t_heredoc listesine içerik için
		// bir düğüm ekleyecek şekilde güncelleyeceğiz.
		if (add_heredoc_to_command(current, raw_word) == -1)
			return (-1);
	}
	add_redir_to_list(current, raw_word, token_type);
	(*i)++;
	return (0);
}

static t_command	*handle_pipe_token(t_command *current, t_command *head)
{
	current->next = create_list();
	if (!current->next)
	{
		free_command_list(head);
		return (NULL);
	}
	return (current->next);
}

/*
** DEĞİŞTİRİLDİ: dispatch_token ve process_token
** Mantık aynı kaldı, sadece handle_redirections'ın yeni versiyonunu kullanıyor.
*/
static int	dispatch_token(t_data *data, t_command **curr, t_command *head,
		int *i)
{
	if (data->token[*i] == TOKEN_WORD)
	{
		add_argument_to_command(*curr, data->word_array[*i]);
		(*i)++;
	}
	else if (data->token[*i] == TOKEN_PIPE)
	{
		*curr = handle_pipe_token(*curr, head);
		if (!*curr)
			return (1);
		(*i)++;
	}
	else
	{
		// Yeniden yazdığımız handle_redirections fonksiyonunu çağırıyoruz.
		if (handle_redirections_2(data, *curr, i) == -1)
		{
			free_command_list(head); // Hata durumunda hafızayı temizle.
			return (1);
		}
	}
	return (0);
}

static int	process_token(t_data *data, t_command **current, t_command *head)
{
	int	i;

	i = 0;
	while (data->word_array[i])
	{
		if (dispatch_token(data, current, head, &i) != 0)
			return (1);
	}
	return (0);
}

t_command	*parser(t_data *data)
{
	t_command	*head;
	t_command	*current;

	if (!data || !data->word_array)
		return (NULL);
	head = create_list();
	if (!head)
		return (NULL);
	current = head;
	if (process_token(data, &current, head) != 0)
		return (NULL);
	return (head);
}
