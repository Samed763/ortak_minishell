/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 18:37:06 by sadinc            #+#    #+#             */
/*   Updated: 2025/07/31 16:06:14 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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

	if (!cmd || !word)
		return ;
	cleaned_word = remove_quotes_from_word(word);
	if (!cleaned_word)
		return ;
	cmd->args = realloc_args(cmd->args, cleaned_word);
	if (!cmd->args)
		return ;
}

static int	realloc_and_copy_outputs(t_command *curr, char ***n_files,
		int **n_modes)
{
	int	i;

	*n_files = malloc(sizeof(char *) * (curr->output_count + 2));
	if (!*n_files)
		return (0);
	*n_modes = malloc(sizeof(int) * (curr->output_count + 1));
	if (!*n_modes)
	{
		free(*n_files);
		return (0);
	}
	i = 0;
	while (i < curr->output_count)
	{
		(*n_files)[i] = curr->output_files[i];
		(*n_modes)[i] = curr->append_modes[i];
		i++;
	}
	return (1);
}

void	add_output_to_command(t_command *curr, char *filename, int append_mode)
{
	char	**new_files;
	int		*new_modes;

	if (!curr || !filename)
		return ;
	if (!realloc_and_copy_outputs(curr, &new_files, &new_modes))
		return ;
	new_files[curr->output_count] = ft_strdup(filename);
	if (!new_files[curr->output_count])
	{
		free(new_files);
		free(new_modes);
		return ;
	}
	new_files[curr->output_count + 1] = NULL;
	new_modes[curr->output_count] = append_mode;
	if (curr->output_files)
		free(curr->output_files);
	if (curr->append_modes)
		free(curr->append_modes);
	curr->output_files = new_files;
	curr->append_modes = new_modes;
	curr->output_count++;
}
