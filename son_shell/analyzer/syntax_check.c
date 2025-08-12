/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_check.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/26 19:53:19 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/12 18:14:35 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	check_token_syntax(char *line, int i)
{
	int	j;

	if (line[i] == '|' && line[i + 1] && line[i + 1] == '|')
		return (1);
	if ((line[i] == '>' || line[i] == '<') && line[i + 1] == line[i]
		&& line[i + 2] == line[i])
		return (1);
	j = i;
	if ((line[j] == '<' || line[j] == '>') && line[j + 1] == line[j])
		j++;
	j++;
	while (line[j] && (line[j] == ' ' || line[j] == '\t'))
		j++;
	if (is_token(line[i]))
	{
		if (line[j] == '\0')
			return (1);
		if (line[i] != '|' && is_token(line[j]))
			return (1);
		if (line[i] == '|' && line[j] == '|')
			return (1);
	}
	return (0);
}

static int	process_char(char *line, int i, int *in_s_quote, int *in_d_quote)
{
	if (!line || !in_s_quote || !in_d_quote)
		return (1);
	if (line[i] == '\'' && !(*in_d_quote))
		*in_s_quote = !(*in_s_quote);
	else if (line[i] == '"' && !(*in_s_quote))
		*in_d_quote = !(*in_d_quote);
	if (is_token(line[i]) && !(*in_s_quote) && !(*in_d_quote))
	{
		if (check_token_syntax(line, i))
			return (1);
	}
	return (0);
}

static int	syntax_validation(char *line)
{
	int	i;
	int	in_single_quote;
	int	in_double_quote;

	if (!line)
		return (1);
	i = 0;
	in_single_quote = 0;
	in_double_quote = 0;
	while (line[i] && (line[i] == ' ' || line[i] == '\t'))
		i++;
	if (line[i] == '|')
		return (1);
	while (line[i])
	{
		if (process_char(line, i, &in_single_quote, &in_double_quote))
			return (1);
		i++;
	}
	if (in_single_quote || in_double_quote)
		return (1);
	return (0);
}

int	syntax_check(char *line)
{
	int	i;

	i = 0;
	if (!line || !*line)
		return (0);
	while (line[i] && (line[i] == ' ' || line[i] == '\t'))
		i++;
	if (line[i] == '\0')
		return (0);
	if (syntax_validation(line))
		return (1);
	return (0);
}
