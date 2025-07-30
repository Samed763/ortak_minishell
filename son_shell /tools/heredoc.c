/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 19:06:41 by sadinc            #+#    #+#             */
/*   Updated: 2025/07/30 19:07:16 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	heredoc_child_routine(int pipe_write_fd, char *delimiter)
{
	char	*line;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, delimiter) == 0)
		{
			if (line)
				free(line);
			break ;
		}
		write(pipe_write_fd, line, ft_strlen(line));
		write(pipe_write_fd, "\n", 1);
		free(line);
	}
	close(pipe_write_fd);
	exit(0);
}

static int	wait_and_process_child(t_data *data, t_command *cmd, pid_t pid,
		int pipe_read_fd)
{
	int	status;

	waitpid(pid, &status, 0);
	read_from_pipe_and_fill_list(pipe_read_fd, cmd);
	signal(SIGINT, signal_handler);
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		write(STDOUT_FILENO, "\n", 1);
		data->exit_value = 130;
		free_heredoc_lines(cmd->heredoc_lines);
		cmd->heredoc_lines = NULL;
		return (-1);
	}
	return (0);
}

int	handle_heredoc(t_data *data, t_command *cmd)
{
	int		pipefd[2];
	pid_t	pid;

	if (!cmd->heredoc_delimiter)
		return (0);
	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return (-1);
	}
	signal(SIGINT, SIG_IGN);
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (-1);
	}
	if (pid == 0)
		heredoc_child_routine(pipefd[1], cmd->heredoc_delimiter);
	close(pipefd[1]);
	if (wait_and_process_child(data, cmd, pid, pipefd[0]) == -1)
		return (-1);
	if (cmd->should_expand_heredoc)
		expand_heredoc_lines(data, cmd);
	return (0);
}
