/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   leak_manager.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 16:09:43 by sadinc            #+#    #+#             */
/*   Updated: 2025/07/31 16:09:44 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	free_heredoc_lines(t_heredoc_line *head)
{
	t_heredoc_line	*tmp;

	while (head)
	{
		tmp = head->next;
		if (head->content)
			free(head->content);
		free(head);
		head = tmp;
	}
}

void	free_command_list(t_command *head)
{
	t_command	*tmp;

	while (head)
	{
		tmp = head->next;
		if (head->args)
			free_word_array(head->args);
		if (head->input_files)
			free(head->input_files);
		if (head->output_files)
			free_word_array(head->output_files);
		if (head->append_modes)
			free(head->append_modes);
		if (head->heredoc_delimiter)
			free(head->heredoc_delimiter);
		if (head->heredoc_lines)
			free_heredoc_lines(head->heredoc_lines);
		free(head);
		head = tmp;
	}
}

void	free_data_resources(t_data *data)
{
	if (!data)
		return ;
	if (data->word_array)
		free_word_array(data->word_array);
	data->word_array = NULL;
	if (data->token)
		free(data->token);
	data->token = NULL;
	if (data->cmd)
		free_command_list(data->cmd);
	data->cmd = NULL;
}
