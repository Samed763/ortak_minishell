/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 19:06:41 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/12 00:37:29 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <unistd.h>
# include <sys/wait.h>

extern int	g_signal_received;

void    heredoc_interrupt_handler(int signum)
{
	(void)signum;
	g_signal_received = 1;
	write(STDOUT_FILENO,"\n",1);
	// readline'ın engellendiği STDIN'i kapatarak onu kesintiye uğrat.
	close(STDIN_FILENO);
}

void    heredoc_child_process(int *pipefd, t_heredoc *heredoc)
{
	char    *line;

	signal(SIGINT, heredoc_interrupt_handler);
	signal(SIGQUIT, SIG_IGN);
	close(pipefd[0]);

	while (1)
	{
		line = readline("> ");
		// Ctrl+C (g_interrupted), Ctrl+D (!line) veya delimiter girilirse döngüden çık.
		if (g_signal_received || !line || ft_strcmp(line, heredoc->delimiter) == 0)
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

static int  process_single_heredoc(t_heredoc *heredoc)
{
	int     pipefd[2];
	pid_t   pid;
	int     status;

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
	{
		close(pipefd[1]);
		read_pipe_fill_list(pipefd[0], heredoc);
		waitpid(pid, &status, 0);
		setup_signals();
		// DÜZELTME: Alt işlemin `exit()` ile mi sonlandığını ve
		// çıkış kodunun 130 olup olmadığını kontrol et.
		if (WIFEXITED(status) && WEXITSTATUS(status) == 130)
		{
			// Çıkış kodu 130 ise, bu Ctrl+C anlamına gelir.
			// Ana shell'in çıkış kodunu güncellemek için 130 döndür.
			return (130);
		}
	}
	return (0);
}

int handle_heredoc(t_data *data, t_command *cmd)
{
	t_heredoc   *current_heredoc;
	int         heredoc_status;

	heredoc_status = 0;
	current_heredoc = cmd->heredocs;
	while (current_heredoc)
	{
		heredoc_status = process_single_heredoc(current_heredoc);
		if (heredoc_status != 0)
		{
			// heredoc 130 ile kesildiyse, ana shell'in çıkış kodunu güncelle
			// ve diğer heredoc'ları işleme almadan çık.
			data->exit_value = heredoc_status;
			return (heredoc_status);
		}
		current_heredoc = current_heredoc->next;
	}
	expand_heredoc_lines(data, cmd);
	return (heredoc_status);
}
