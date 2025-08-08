/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_command.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 19:46:46 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/08 10:09:07 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

#include "../minishell.h"

// Hata mesajı yazıp çıkan yardımcı fonksiyon.
void write_error_and_exit(t_data *data, int exit_val, char *arg, char *error)
{
	write(2, "minishell: ", 11);
	write(2, arg, ft_strlen(arg));
	write(2, ": ", 2);
	write(2, error, ft_strlen(error));
	write(2, "\n", 1);
	cleanup_and_exit(data, exit_val);
}

// YENİ: Birleşik yönlendirme fonksiyonu.
// Yönlendirmeleri sırayla işler ve ilk hatada durur.
int handle_redirections_out(t_command *cmd)
{
	t_redir *redir;
	int fd;
	t_heredoc *heredoc_iter;
	int heredoc_count;

	redir = cmd->redirs;
	heredoc_iter = cmd->heredocs;
	heredoc_count = 0;
	while (redir)
	{
		if (redir->type == TOKEN_REDIRECT_OUT || redir->type == TOKEN_APPEND)
		{
			fd = open(redir->filename,
					  (O_WRONLY | O_CREAT | (O_TRUNC * (redir->type == TOKEN_REDIRECT_OUT)) | (O_APPEND * (redir->type == TOKEN_APPEND))), 0644);
			if (fd == -1)
			{
				perror(redir->filename);
				return (-1);
			}
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (redir->type == TOKEN_REDIRECT_IN)
		{
			fd = open(redir->filename, O_RDONLY);
			if (fd == -1)
			{
				perror(redir->filename);
				return (-1);
			}
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		else if (redir->type == TOKEN_HEREDOC)
		{
			heredoc_count++;
			// Sadece son heredoc'u uygula
			if (heredoc_iter && heredoc_count > 0)
			{
				apply_input_redirection(cmd);
				heredoc_iter = heredoc_iter->next;
			}
		}
		redir = redir->next;
	}
	return (0);
}

// Çocuk prosesin çalıştıracağı ana fonksiyon.
static void child_process_routine(t_data *data, char **splitted_path)
{
	char *full_path;
	int access_ret;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	if (handle_redirections_out(data->cmd) == -1)
		cleanup_and_exit(data, 1);
	if (data->cmd->args && data->cmd->args[0])
	{
		access_ret = is_accessable(data->cmd->args[0], splitted_path,
								   &full_path);
		if (access_ret == -1)
			write_error_and_exit(data, 127, data->cmd->args[0],
								 "command not found");
		else if (access_ret == -2)
			write_error_and_exit(data, 126, data->cmd->args[0],
								 "Permission denied");
		if (execve(full_path, data->cmd->args, data->env) == -1)
		{
			perror("execve");
			free(full_path);
			cleanup_and_exit(data, 1);
		}
	}
	cleanup_and_exit(data, 0);
}

// Dahili bir komutu çalıştıran fonksiyon.
static void execute_single_builtin(t_data *data)
{
	data->original_stdin = dup(STDIN_FILENO);
	data->original_stdout = dup(STDOUT_FILENO);
	if (handle_redirections_out(data->cmd) == -1)
		data->exit_value = 1;
	else
		try_builtin(data->cmd, data, 1);
	restore_fds(data);
}

// Harici bir komutu çalıştıran fonksiyon.
static void execute_single_external(t_data *data, char **splitted_path)
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		data->exit_value = 1;
		return;
	}
	if (pid == 0)
		child_process_routine(data, splitted_path);
	else
	{
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		wait(&status);
		set_exit_status(data, status);
		signal(SIGINT, signal_handler);
		signal(SIGQUIT, SIG_IGN);
	}
}

// Tüm heredoc'ları okuyan fonksiyon.
static int handle_all_heredocs(t_data *data)
{
	t_command *current_cmd;

	current_cmd = data->cmd;
	while (current_cmd)
	{
		if (current_cmd->heredocs)
		{
			if (handle_heredoc(data, current_cmd) == -1)
			{
				data->exit_value = 1;
				return (-1);
			}
		}
		current_cmd = current_cmd->next;
	}
	return (0);
}

// Komut yürütmenin ana giriş noktası.
void execute_command(t_data *data)
{
	char *path_val;

	if (handle_all_heredocs(data) == -1)
		return;
	path_val = find_value_by_key(data, "PATH");
	data->splitted_path = ft_split(path_val, ':');
	if (path_val)
		free(path_val);
	// if (data->cmd->next)
	// 	//pipe_execute(data);
	if (data->cmd->args && is_builtin(data->cmd->args[0]))
		execute_single_builtin(data);
	else
		execute_single_external(data, data->splitted_path);
	if (data->splitted_path)
		free_word_array(data->splitted_path);
	data->splitted_path = NULL;
}