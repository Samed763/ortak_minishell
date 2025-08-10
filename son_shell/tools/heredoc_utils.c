/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 19:07:05 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/10 15:11:52 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
# include <unistd.h>

void	expand_heredoc_lines(t_data *data, t_command *cmd)
{
	t_heredoc		*current_heredoc;
	t_heredoc_line	*current_line;
	char			*expanded;

	current_heredoc = cmd->heredocs;
	while (current_heredoc)
	{
		if (current_heredoc->should_expand)
		{
			current_line = current_heredoc->lines;
			while (current_line)
			{
				expanded = expand_single_line(data, current_line->content, 1);
				free(current_line->content);
				current_line->content = expanded;
				current_line = current_line->next;
			}
		}
		current_heredoc = current_heredoc->next;
	}
}

static void	add_line_to_list(t_heredoc *heredoc, char *line)
{
	t_heredoc_line	*new_node;
	t_heredoc_line	*current;

	new_node = malloc(sizeof(t_heredoc_line));
	if (!new_node)
		return ;
	new_node->content = ft_strdup(line);
	if (!new_node->content)
	{
		free(new_node);
		return ;
	}
	new_node->next = NULL;
	if (heredoc->lines == NULL)
		heredoc->lines = new_node;
	else
	{
		current = heredoc->lines;
		while (current->next)
			current = current->next;
		current->next = new_node;
	}
}

static char	*read_pipe_content(int pipe_read_fd)
{
	char	buffer[4097];
	ssize_t	bytes_read;
	char	*full_content;
	char	*temp;

	full_content = ft_strdup("");
	if (!full_content)
		return (NULL);
	bytes_read = 1;
	while (bytes_read > 0)
	{
		bytes_read = read(pipe_read_fd, buffer, 4096);
		if (bytes_read <= 0)
			break ;
		buffer[bytes_read] = '\0';
		temp = ft_strjoin(full_content, buffer);
		free(full_content);
		if (!temp)
			return (NULL);
		full_content = temp;
	}
	return (full_content);
}

void	read_pipe_fill_list(int pipe_read_fd, t_heredoc *heredoc)
{
	char	*full_content;
	char	*line_start;
	int		i;

	full_content = read_pipe_content(pipe_read_fd);
	close(pipe_read_fd);
	if (!full_content)
		return ;
	line_start = full_content;
	i = 0;
	while (full_content[i])
	{
		if (full_content[i] == '\n')
		{
			full_content[i] = '\0';
			add_line_to_list(heredoc, line_start);
			line_start = &full_content[i + 1];
		}
		i++;
	}
	if (*line_start)
		add_line_to_list(heredoc, line_start);
	free(full_content);
}

void	add_line_to_heredoc(t_heredoc *heredoc, char *content)
{
	t_heredoc_line	*new_line;
	t_heredoc_line	*current;

	if (!heredoc || !content)
		return ;
	new_line = malloc(sizeof(t_heredoc_line));
	if (!new_line)
		return ;
	new_line->content = ft_strdup(content);
	if (!new_line->content)
	{
		free(new_line);
		return ;
	}
	new_line->next = NULL;
	if (heredoc->lines == NULL)
		heredoc->lines = new_line;
	else
	{
		current = heredoc->lines;
		while (current->next)
			current = current->next;
		current->next = new_line;
	}
}
