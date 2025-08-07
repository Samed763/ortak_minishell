/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 19:37:55 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/07 19:07:19 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	is_builtin(char *command)
{
	if (!command)
		return (0);
	if (ft_strcmp(command, "echo") == 0)
		return (1);
	if (ft_strcmp(command, "cd") == 0)
		return (1);
	if (ft_strcmp(command, "pwd") == 0)
		return (1);
	if (ft_strcmp(command, "export") == 0)
		return (1);
	if (ft_strcmp(command, "unset") == 0)
		return (1);
	if (ft_strcmp(command, "env") == 0)
		return (1);
	if (ft_strcmp(command, "exit") == 0)
		return (1);
	return (0);
}

static int	search_in_path(char *command, char **splited_path,
							char **full_path)
{
	int		i;
	char	*temp_path;

	if (!splited_path)
		return (-1);
	i = 0;
	while (splited_path[i])
	{
		temp_path = ft_strjoin(splited_path[i], "/");
		*full_path = ft_strjoin(temp_path, command);
		free(temp_path);
		if (access(*full_path, X_OK) == 0)
			return (0);
		free(*full_path);
		i++;
	}
	*full_path = NULL;
	return (-1);
}

int	is_accessable(char *command, char **splited_path, char **full_path)
{
	if (ft_strchr(command, '/'))
	{
		if (!access(command, F_OK))
			return -2;
		if (access(command, X_OK) == 0)
		{
			*full_path = ft_strdup(command);
			return (0);
		}
		return (-1);
	}
	return (search_in_path(command, splited_path, full_path));
}

void	set_exit_status(t_data *data, int status)
{
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
		write(STDOUT_FILENO, "\n", 1);
	if (WIFEXITED(status))
		data->exit_value = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		data->exit_value = 128 + WTERMSIG(status);
	else
		data->exit_value = 1;
}

void	restore_fds(t_data *data)
{
	if (data->original_stdin != -1)
	{
		if (dup2(data->original_stdin, STDIN_FILENO) == -1)
			perror("dup2 restore");
		close(data->original_stdin);
		data->original_stdin = -1;
	}
	if (data->original_stdout != -1)
	{
		if (dup2(data->original_stdout, STDOUT_FILENO) == -1)
			perror("dup2 restore");
		close(data->original_stdout);
		data->original_stdout = -1;
	}
}
