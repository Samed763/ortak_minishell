/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_command.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 19:46:46 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/12 17:21:04 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
# include <signal.h>
# include <stdio.h>
# include <unistd.h>
# include <sys/wait.h>

static void	child_process_routine(t_data *data, char **splitted_path)
{
	char	*full_path;
	int		access_ret;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	if (handle_redirections(data->cmd) == -1)
		cleanup_and_exit(1);
	if (data->cmd->args && data->cmd->args[0])
	{
		access_ret = is_accessable(data->cmd->args[0], splitted_path,
				&full_path);
		check_error(access_ret, data);
		if (execve(full_path, data->cmd->args, data->env) == -1)
		{
			perror("execve");
			free(full_path);
			cleanup_and_exit(1);
		}
	}
	cleanup_and_exit(0);
}

static void	execute_single_builtin(t_data *data)
{
	data->original_stdin = dup(STDIN_FILENO);
	data->original_stdout = dup(STDOUT_FILENO);
	if (handle_redirections(data->cmd) == -1)
		data->exit_value = 1;
	else
		try_builtin(data->cmd, data, 1);
	restore_fds(data);
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
		setup_signals();
	}
}

static int	handle_all_heredocs(t_data *data)
{
	t_command	*current_cmd;
	int status;

	status = 0;
	current_cmd = data->cmd;
	while (current_cmd)
	{
		if (current_cmd->heredocs)
		{
			status = handle_heredoc(data, current_cmd);
			if (status == -1)
			{
				data->exit_value = 1;
				return (-1);
			}
			if (status == 130)
			{
				data->exit_value = 130;
				return (-1);
			}
		}
		current_cmd = current_cmd->next;
	}
	return (status);
}

void	execute_command(t_data *data)
{
	char	*path_val;

	if (handle_all_heredocs(data) == -1)
		return ;
	path_val = find_value_by_key(data, "PATH");
	data->splitted_path = ft_split(path_val, ':');
	if (path_val)
		free(path_val);
	if (data->cmd->next)
		pipe_execute(data);
	else if (data->cmd->args && is_builtin(data->cmd->args[0]))
		execute_single_builtin(data);
	else
		execute_single_external(data, data->splitted_path);
	if (data->splitted_path)
		free_word_array(data->splitted_path);
	data->splitted_path = NULL;
}
