/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 19:07:05 by sadinc            #+#    #+#             */
/*   Updated: 2025/07/31 16:43:58 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	expand_heredoc_lines(t_data *data, t_command *cmd)
{
	t_heredoc_line	*current;
	char			*expanded;

	current = cmd->heredoc_lines;
	while (current)
	{
		expanded = expand_single_line(data, current->content);
		free(current->content);
		current->content = expanded;
		current = current->next;
	}
}

static void	add_line_to_list(t_command *cmd, char *line)
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
	if (cmd->heredoc_lines == NULL)
		cmd->heredoc_lines = new_node;
	else
	{
		current = cmd->heredoc_lines;
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

void	read_from_pipe_and_fill_list(int pipe_read_fd, t_command *cmd)
{
	char	*full_content;
	char	**lines;
	int		i;

	full_content = read_pipe_content(pipe_read_fd);
	close(pipe_read_fd);
	if (!full_content)
		return ;
	lines = ft_split(full_content, '\n');
	free(full_content);
	if (!lines)
		return ;
	i = 0;
	while (lines[i])
	{
		add_line_to_list(cmd, lines[i]);
		free(lines[i]);
		i++;
	}
	free(lines);
}
