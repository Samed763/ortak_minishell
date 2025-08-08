/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 16:15:10 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/07 14:39:28 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	is_identifier_char(int c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0'
			&& c <= '9') || c == '_');
}

static char	*handle_variable_expansion(t_data *data, char *line, int *i)
{
	char	*key;
	char	*value;
	char	*new_line;
	int		key_end;

	key = extract_variable_key(line, *i, &key_end);
	if (!key)
	{
		free(line);
		return (NULL);
	}
	value = find_value_by_key(data, key);
	new_line = put_var(line, value, *i, key_end);
	free(key);
	if (value)
		free(value);
	return (new_line);
}

char	*expand_single_line(t_data *data, char *line, int f)
{
	t_expand_state	state;
	char			*result;

	result = ft_strdup(line);
	if (!result)
		return (NULL);
	state.s_quotes = 0;
	state.d_quotes = 0;
	state.i = 0;
	while (result && result[state.i])
	{
		update_quoting_state(result[state.i], &state.s_quotes, &state.d_quotes);
		if (result[state.i] == '$' && (f == 1 || state.s_quotes == 0))
		{
			if (result[state.i + 1] && is_valid_to_expand(result[state.i + 1]))
			{
				result = handle_variable_expansion(data, result, &state.i);
				if (!result)
					return (NULL);
				state.i = -1;
			}
		}
		state.i++;
	}
	return (result);
}

void	expander(t_data *data)
{
	int		i;
	char	*expanded;

	if (!data || !data->word_array)
		return ;
	i = 0;
	while (data->word_array[i])
	{
		if (i > 0 && data->token[i - 1] == TOKEN_HEREDOC)
		{
			i++;
			continue ;
		}
		expanded = expand_single_line(data, data->word_array[i], 0);
		if (expanded)
		{
			free(data->word_array[i]);
			data->word_array[i] = expanded;
		}
		i++;
	}
}
