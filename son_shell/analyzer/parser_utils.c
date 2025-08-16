/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 14:12:12 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/16 14:12:14 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_command	*create_list(void)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->pid = -1;
	cmd->redirs = NULL;
	cmd->heredocs = NULL;
	cmd->next = NULL;
	return (cmd);
}

static void	process_quoted_section(char *str, char *new_str, int *i, int *j)
{
	char	quote_char;

	quote_char = str[*i];
	(*i)++;
	while (str[*i] && str[*i] != quote_char)
	{
		new_str[*j] = str[*i];
		(*j)++;
		(*i)++;
	}
	if (str[*i] == quote_char)
		(*i)++;
}

char	*remove_quotes(char *str)
{
	char	*new_str;
	int		i;
	int		j;

	if (!str)
		return (NULL);
	new_str = (char *)malloc(ft_strlen(str) + 1);
	if (!new_str)
		return (NULL);
	i = 0;
	j = 0;
	while (str[i])
	{
		if (str[i] == '\'' || str[i] == '"')
			process_quoted_section(str, new_str, &i, &j);
		else
			new_str[j++] = str[i++];
	}
	new_str[j] = '\0';
	return (new_str);
}

char	*remove_quotes_from_word(char *str, int *should_expand)
{
	char	*new_str;
	int		i;
	int		j;

	if (!str)
		return (NULL);
	*should_expand = 1;
	if (ft_strchr(str, '\'') != NULL || ft_strchr(str, '"') != NULL)
		*should_expand = 0;
	new_str = (char *)malloc(ft_strlen(str) + 1);
	if (!new_str)
		return (NULL);
	i = 0;
	j = 0;
	while (str[i])
	{
		if (str[i] == '\'' || str[i] == '"')
			process_quoted_section(str, new_str, &i, &j);
		else
			new_str[j++] = str[i++];
	}
	new_str[j] = '\0';
	return (new_str);
}
