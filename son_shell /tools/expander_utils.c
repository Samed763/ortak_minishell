/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 19:01:29 by sadinc            #+#    #+#             */
/*   Updated: 2025/07/30 19:12:47 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	is_identifier_char(int c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
		|| (c >= '0' && c <= '9') || c == '_');
}

void	update_quoting_state(char c, int *s_quotes, int *d_quotes)
{
	if (c == '\'' && *d_quotes == 0)
		*s_quotes = !(*s_quotes);
	else if (c == '"' && *s_quotes == 0)
		*d_quotes = !(*d_quotes);
}

char	*extract_variable_key(char *line, int start_pos, int *end_pos)
{
	int	k;

	k = start_pos + 1;
	if (line[k] == '?')
	{
		*end_pos = k + 1;
		return (ft_strdup("?"));
	}
	while (line[k] && is_identifier_char(line[k]))
		k++;
	*end_pos = k;
	return (ft_substr(line, start_pos + 1, k - (start_pos + 1)));
}

static char	*build_new_string(char *start, char *middle, char *end)
{
	char	*temp;
	char	*result;
	char	*safe_middle;

	if (!middle)
		safe_middle = ft_strdup("");
	else
		safe_middle = ft_strdup(middle);
	if (!safe_middle)
	{
		free(start);
		free(end);
		return (NULL);
	}
	temp = ft_strjoin(safe_middle, end);
	free(safe_middle);
	free(end);
	if (!temp)
	{
		free(start);
		return (NULL);
	}
	result = ft_strjoin(start, temp);
	free(start);
	free(temp);
	return (result);
}

char	*put_var(char *line, char *var_value, int key_start, int key_end)
{
	char	*start_str;
	char	*end_str;
	char	*new_line;

	start_str = ft_substr(line, 0, key_start);
	if (!start_str)
	{
		free(line);
		return (NULL);
	}
	end_str = ft_substr(line, key_end, ft_strlen(line));
	if (!end_str)
	{
		free(start_str);
		free(line);
		return (NULL);
	}
	new_line = build_new_string(start_str, var_value, end_str);
	if (!new_line)
		free(line);
	else
		free(line);
	return (new_line);
}
