/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 18:36:57 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/07 15:37:46 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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
	*should_expand = (ft_strchr(str, '\'') == NULL
			&& ft_strchr(str, '\"') == NULL);
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

void	remove_quotes_parser_helper(const char *str, char **del, t_command *cur)
{
	size_t	len;

	if (!str || !del || !cur)
		return ;
	len = ft_strlen(str);
	if (len >= 2 && ((str[0] == '"' && str[len - 1] == '"') || (str[0] == '\''
				&& str[len - 1] == '\'')))
		*del = ft_substr(str, 1, len - 2);
	else
		*del = ft_strdup(str);
	if (!*del)
		*del = ft_strdup("");
}

t_command	*create_list(void)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->input_files = NULL;
	cmd->output_files = NULL;
	cmd->append_modes = NULL;
	cmd->output_count = 0;
	cmd->heredocs = NULL;
	cmd->next = NULL;
	return (cmd);
}

void	add_input_to_command(t_command *current, char *filename)
{
	char	*new_input_file;

	if (!current || !filename)
		return ;
	new_input_file = remove_quotes_2(filename);
	if (!new_input_file)
		return ;
	if (current->input_files)
		free(current->input_files);
	current->input_files = new_input_file;
}
