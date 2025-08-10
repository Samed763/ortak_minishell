/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 19:06:41 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/10 10:34:15 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	heredoc_child_process(int *pipefd, t_heredoc *heredoc)
{
	char	*line;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_IGN);
	close(pipefd[0]);
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, heredoc->delimiter) == 0)
		{
			if (!line)
				printf("minishell: here-document delimited by end-of-file\n");
			free(line);
			break ;
		}
		write(pipefd[1], line, ft_strlen(line));
		write(pipefd[1], "\n", 1);
		free(line);
	}
	close(pipefd[1]);
	cleanup_and_exit(0);
}

static int	process_single_heredoc(t_heredoc *heredoc)
{
	int		pipefd[2];
	pid_t	pid;
	int		status;

	if (pipe(pipefd) == -1)
		return (-1);
	pid = fork();
	if (pid == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		return (-1);
	}
	if (pid == 0)
		heredoc_child_process(pipefd, heredoc);
	else
	{
		close(pipefd[1]);
		read_pipe_fill_list(pipefd[0], heredoc);
		waitpid(pid, &status, 0);
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
			return (-1);
	}
	return (0);
}

int	handle_heredoc(t_data *data, t_command *cmd)
{
	t_heredoc	*current_heredoc;

	current_heredoc = cmd->heredocs;
	while (current_heredoc)
	{
		if (process_single_heredoc(current_heredoc) == -1)
			return (-1);
		current_heredoc = current_heredoc->next;
	}
	expand_heredoc_lines(data, cmd);
	return (0);
}
