/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apply_input.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 19:42:50 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/02 17:44:52 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	input_heredoc(t_data *data, t_command *cmd)
{
	pid_t	pid;
	int		pipefd[2];

	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return (-1);
	}
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (-1);
	}
	else if (pid == 0)
		heredoc_child_process(data, pipefd, cmd);
	else
		return (heredoc_parent_process(pipefd));
	return (0);
}

static int	input_file(t_command *cmd)
{
	int	fd;

	fd = open(cmd->input_files, O_RDONLY);
	if (fd == -1)
	{
		perror(cmd->input_files);
		return (-1);
	}
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		perror("dup2");
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

static int	handle_heredoc_input(t_data *data, t_command *cmd)
{
	int	fd;

	if (cmd->heredoc_lines != NULL)
		return (input_heredoc(data, cmd));
	else
	{
		fd = open("/dev/null", O_RDONLY);
		if (fd == -1)
		{
			perror("/dev/null");
			return (-1);
		}
		if (dup2(fd, STDIN_FILENO) == -1)
		{
			perror("dup2");
			close(fd);
			return (-1);
		}
		close(fd);
	}
	return (0);
}

int	apply_input_redirection(t_data *data, t_command *cmd)
{
	if (cmd->heredoc_delimiter)
		return (handle_heredoc_input(data, cmd));
	else if (cmd->input_files != NULL)
		return (input_file(cmd));
	return (0);
}
