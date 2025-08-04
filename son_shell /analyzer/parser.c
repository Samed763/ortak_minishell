/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 17:18:03 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/04 08:45:05 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	handle_redirections(t_data *data, t_command *current, int *i)
{
	int	token_type;

	token_type = data->token[*i];
	(*i)++;
	if (!data->word_array[*i])
		return (-1);
	if (token_type == TOKEN_REDIRECT_IN)
		add_input_to_command(current, data->word_array[*i]);
	else if (token_type == TOKEN_REDIRECT_OUT)
		add_output_to_command(current, data->word_array[*i], 0);
	else if (token_type == TOKEN_APPEND)
		add_output_to_command(current, data->word_array[*i], 1);
	else if (token_type == TOKEN_HEREDOC)
	{
		multiple_heredoc(data, current->heredoc_delimiter);
		remove_quotes_parser_helper(data->word_array[*i],
			&current->heredoc_delimiter, current);
		if (handle_heredoc(data, current) == -1)
			return (-1);
	}
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
		if (handle_redirections(data, *curr, i) == -1)
		{
			free_command_list(head);
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
