/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   count_words.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 17:25:59 by sadinc            #+#    #+#             */
/*   Updated: 2025/07/30 18:37:18 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	update_quote_state(char c, int *in_s_quote, int *in_d_quote)
{
	if (c == '\'' && !(*in_d_quote))
		*in_s_quote = !(*in_s_quote);
	else if (c == '"' && !(*in_s_quote))
		*in_d_quote = !(*in_d_quote);
}

static void	handle_word_counting(char *line, int *i, int *wc, int *word)
{
	if (*word)
	{
		(*wc)++;
		*word = 0;
	}
	if (is_token(line[*i]) && line[*i + 1] && line[*i] == line[*i + 1])
	{
		(*wc)++;
		(*i)++;
	}
	else if (is_token(line[*i]))
	{
		(*wc)++;
	}
}

int	count_word(char *line)
{
	int	i;
	int	wc;
	int	word;
	int	in_s_quote;
	int	in_d_quote;

	i = 0;
	wc = 0;
	word = 0;
	in_s_quote = 0;
	in_d_quote = 0;
	if (!line)
		return (0);
	while (line[i])
	{
		update_quote_state(line[i], &in_s_quote, &in_d_quote);
		if (!in_s_quote && !in_d_quote && (line[i] == ' ' || is_token(line[i])))
			handle_word_counting(line, &i, &wc, &word);
		else if (!word)
			word = 1;
		i++;
	}
	if (word)
		wc++;
	return (wc);
}
