/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 18:37:06 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/08 11:01:52 by sadinc           ###   ########.fr       */
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

/*
** GÜNCELLENDİ: add_heredoc_to_command
** Bu fonksiyon artık sadece heredoc içeriklerini (delimiter, satırlar vb.)
** tutacak olan t_heredoc listesine bir eleman ekler.
** Yönlendirmenin kendisi (t_redir) zaten parser tarafından listeye ekleniyor.
*/
int	add_heredoc_to_command(t_command *cmd, char *raw_word)
{
	t_heredoc	*new_heredoc;
	t_heredoc	*current;
	char		*cleaned_delimiter;
	int			should_expand;

	if (!cmd || !raw_word)
		return (0);
	// delimiter'daki tırnakları temizle ve genişletme bayrağını ayarla
	cleaned_delimiter = remove_quotes_from_word(raw_word, &should_expand);
	if (!cleaned_delimiter)
		return (-1);

	new_heredoc = create_heredoc(cleaned_delimiter, should_expand);
	free(cleaned_delimiter);
	if (!new_heredoc)
		return (-1);

	// Yeni heredoc'u komutun heredoc listesinin sonuna ekle
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
// YENİ FONKSİYON
void	add_redir_to_list(t_command *cmd, char *filename, int type)
{
    t_redir	*new_redir;
    t_redir	*current;

    new_redir = malloc(sizeof(t_redir));
    if (!new_redir)
        return ;
    new_redir->filename = remove_quotes(filename); // Dosya adındaki tırnakları temizle
    new_redir->type = type;
    new_redir->next = NULL;
    if (!cmd->redirs)
        cmd->redirs = new_redir;
    else
    {
        current = cmd->redirs;
        while (current->next)
            current = current->next;
        current->next = new_redir;
    }
}
static char	**realloc_args(char **old_args, char *cleaned_word)
{
	char	**new_args;
	int		old_argc;
	int		i;

	old_argc = 0;
	while (old_args && old_args[old_argc])
		old_argc++;
	new_args = malloc(sizeof(char *) * (old_argc + 2));
	if (!new_args)
	{
		free(cleaned_word);
		return (NULL);
	}
	i = 0;
	while (i < old_argc)
	{
		new_args[i] = old_args[i];
		i++;
	}
	new_args[i] = cleaned_word;
	new_args[i + 1] = NULL;
	if (old_args)
		free(old_args);
	return (new_args);
}

void	add_argument_to_command(t_command *cmd, char *word)
{
	char	*cleaned_word;

	if (!cmd || !word || !*word)
		return ;
	cleaned_word = remove_quotes(word);
	if (!cleaned_word)
		return ;
	cmd->args = realloc_args(cmd->args, cleaned_word);
	if (!cmd->args)
		return ;
}
