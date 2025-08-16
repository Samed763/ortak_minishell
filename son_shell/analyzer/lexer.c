/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 14:12:08 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/16 14:12:09 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../minishell.h"

static char	*extract_token(char *line, int *start)
{
	int	len;

	len = 1;
	if ((line[*start] == '<' && line[*start + 1] == '<') || (line[*start] == '>'
			&& line[*start + 1] == '>'))
	{
		len = 2;
		*start += 2;
	}
	else
	{
		(*start)++;
	}
	return (ft_substr(line, *start - len, len));
}

static char	*extract_word(char *line, int *start)
{
	int	word_start;
	int	in_single_quote;
	int	in_double_quote;

	word_start = *start;
	in_single_quote = 0;
	in_double_quote = 0;
	while (line[*start])
	{
		if (line[*start] == '\'' && !in_double_quote)
			in_single_quote = !in_single_quote;
		else if (line[*start] == '"' && !in_single_quote)
			in_double_quote = !in_double_quote;
		else if (!in_single_quote && !in_double_quote && (line[*start] == ' '
				|| is_token(line[*start])))
			break ;
		(*start)++;
	}
	return (ft_substr(line, word_start, *start - word_start));
}

static void	skip_whitespace(char *line, int *pos)
{
	while (line[*pos] && (line[*pos] == ' ' || line[*pos] == '\t'))
		(*pos)++;
}

static void	fill_token_arrays(char *line, t_data *data)
{
	int	i;
	int	word_index;

	i = 0;
	word_index = 0;
	while (line[i])
	{
		skip_whitespace(line, &i);
		if (!line[i])
			break ;
		if (is_token(line[i]))
			data->word_array[word_index] = extract_token(line, &i);
		else
			data->word_array[word_index] = extract_word(line, &i);
		if (!data->word_array[word_index])
		{
			cleanup_and_exit(1);
			return ;
		}
		data->token[word_index] = get_token_type(data->word_array[word_index]);
		word_index++;
	}
	data->word_array[word_index] = NULL;
	data->token[word_index] = -1;
}

void	lexer(char *line, t_data *data)
{
	int	word_count;

	if (!line || !data)
		return ;
	word_count = count_word(line);
	data->word_array = malloc(sizeof(char *) * (word_count + 1));
	data->token = malloc(sizeof(int) * (word_count + 1));
	if (!data->word_array || !data->token)
	{
		cleanup_and_exit(1);
		return ;
	}
	fill_token_arrays(line, data);
}
