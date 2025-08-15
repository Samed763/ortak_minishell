/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 19:06:41 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/15 20:46:12 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <readline/readline.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

extern int	g_signal_received;

void	heredoc_interrupt_handler(int signum)
{
	(void)signum;
	g_signal_received = 1;
	write(STDOUT_FILENO, "\n", 1);
	close(STDIN_FILENO);
}

static int	heredoc_parent_process(pid_t pid, int pipefd[2], t_heredoc *heredoc)
{
	int	status;

	close(pipefd[1]);
	read_pipe_fill_list(pipefd[0], heredoc);
	close(pipefd[0]);
	waitpid(pid, &status, 0);
	setup_signals();
	if (WIFEXITED(status) && WEXITSTATUS(status) == 130)
		return (130);
	return (0);
}

void	heredoc_child_process(int *pipefd, t_heredoc *heredoc)
{
	char	*line;

	signal(SIGINT, heredoc_interrupt_handler);
	signal(SIGQUIT, SIG_IGN);
	close(pipefd[0]);
	while (1)
	{
		line = readline("> ");
		if (g_signal_received || !line || ft_strcmp(line,
				heredoc->delimiter) == 0)
		{
			if (!line && !g_signal_received)
				printf("minishell: here-document delimited by end-of-file\n");
			free(line);
			break ;
		}
		write(pipefd[1], line, ft_strlen(line));
		write(pipefd[1], "\n", 1);
		free(line);
	}
	close(pipefd[1]);
	if (g_signal_received)
		cleanup_and_exit(130);
	cleanup_and_exit(0);
}

static int	process_single_heredoc(t_heredoc *heredoc)
{
	int		pipefd[2];
	pid_t	pid;

	if (pipe(pipefd) == -1)
		return (-1);
	signal(SIGINT, SIG_IGN);
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
		return (heredoc_parent_process(pid, pipefd, heredoc));
	return (0);
}

int	handle_heredoc(t_data *data, t_command *cmd)
{
	t_heredoc	*current_heredoc;
	int			heredoc_status;

	heredoc_status = 0;
	current_heredoc = cmd->heredocs;
	while (current_heredoc)
	{
		heredoc_status = process_single_heredoc(current_heredoc);
		if (heredoc_status != 0)
		{
			data->exit_value = heredoc_status;
			return (heredoc_status);
		}
		current_heredoc = current_heredoc->next;
	}
	expand_heredoc_lines(data, cmd);
	return (heredoc_status);
}
