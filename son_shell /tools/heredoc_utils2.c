/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 16:09:54 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/06 16:33:12 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_heredoc	*create_heredoc(char *delimiter, int should_expand)
{
	t_heredoc	*new_heredoc;

	new_heredoc = malloc(sizeof(t_heredoc));
	if (!new_heredoc)
		return (NULL);
	new_heredoc->delimiter = ft_strdup(delimiter);
	if (!new_heredoc->delimiter)
	{
		free(new_heredoc);
		return (NULL);
	}
	new_heredoc->should_expand = should_expand;
	new_heredoc->lines = NULL;
	new_heredoc->next = NULL;
	return (new_heredoc);
}

int	add_heredoc_to_command(t_command *cmd, char *raw_word)
{
	t_heredoc	*new_heredoc;
	t_heredoc	*current;
	char		*cleaned_delimiter;
	int			should_expand;

	if (!cmd || !raw_word)
		return (0);
	cleaned_delimiter = remove_quotes_from_word(raw_word, &should_expand);
	if (!cleaned_delimiter)
		return (-1);
	new_heredoc = create_heredoc(cleaned_delimiter, should_expand);
	free(cleaned_delimiter);
	if (!new_heredoc)
		return (-1);
	if (cmd->heredocs == NULL)
		cmd->heredocs = new_heredoc;
	else
	{
		current = cmd->heredocs;
		while (current->next)
			current = current->next;
		current->next = new_heredoc;
	}
	return (0);
}
