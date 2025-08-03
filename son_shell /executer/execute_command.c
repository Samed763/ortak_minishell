/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_command.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 19:46:46 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/02 17:45:41 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	child_process_routine(t_data *data, char **splitted_path)
{
	char	*full_path;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	if (apply_input_redirection(data, data->cmd) == -1
		|| apply_output_redirection(data->cmd) == -1)
		cleanup_and_exit(data, 1);
	if (data->cmd->args && data->cmd->args[0])
	{
		if (is_accessable(data->cmd->args[0], splitted_path, &full_path) == -1)
		{
			write(2, data->cmd->args[0], ft_strlen(data->cmd->args[0]));
			write(2, ": command not found\n", 20);
			cleanup_and_exit(data, 127);
		}
		if (execve(full_path, data->cmd->args, data->env) == -1)
		{
			perror("execve");
			free(full_path);
			cleanup_and_exit(data, 1);
		}
	}
	cleanup_and_exit(data, 0);
}

static void	execute_single_builtin(t_data *data)
{
	int	original_stdin;
	int	original_stdout;

	original_stdin = dup(STDIN_FILENO);
	original_stdout = dup(STDOUT_FILENO);
	if (original_stdin == -1 || original_stdout == -1)
	{
		perror("dup");
		data->exit_value = 1;
		if (original_stdin != -1)
			close(original_stdin);
		if (original_stdout != -1)
			close(original_stdout);
		return ;
	}
	if (apply_input_redirection(data, data->cmd) == -1
		|| apply_output_redirection(data->cmd) == -1)
		data->exit_value = 1;
	else
		data->exit_value = try_builtin(data->cmd, data, 1);
	restore_fds(original_stdin, original_stdout);
}

static void	execute_single_external(t_data *data, char **splitted_path)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		data->exit_value = 1;
		return ;
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

void	execute_command(t_data *data)
{
	char	*path_val;

	path_val = find_value_by_key(data, "PATH");
	data->splitted_path = ft_split(path_val, ':');
	if (path_val)
		free(path_val);
	if (data->cmd->next)
		pipe_execute(data, data->splitted_path);
	else if (data->cmd->args && is_builtin(data->cmd->args[0]))
		execute_single_builtin(data);
	else
		execute_single_external(data, data->splitted_path);
	if (data->splitted_path)
		free_word_array(data->splitted_path);
	data->splitted_path = NULL;
}
