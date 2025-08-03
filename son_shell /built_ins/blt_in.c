/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   blt_in.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 11:22:32 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/02 11:22:32 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	handle_echo_cd_pwd(t_command *current_cmd, t_data *data)
{
	(void)data;
	if (strcmp(current_cmd->args[0], "echo") == 0)
	{
		data->exit_value =builtin_echo(current_cmd->args);
		return (1);
	}
	if (strcmp(current_cmd->args[0], "cd") == 0)
	{
		data->exit_value =builtin_cd(current_cmd->args);
		return (1);
	}
	if (strcmp(current_cmd->args[0], "pwd") == 0)
	{
		data->exit_value =builtin_pwd();
		return (1);
	}
	return (0);
}

static int	handle_export_unset(t_command *current_cmd, t_data *data)
{
	if (strcmp(current_cmd->args[0], "export") == 0)
	{
		data->exit_value =builtin_export(data);
		return (1);
	}
	if (strcmp(current_cmd->args[0], "unset") == 0)
	{
		data->exit_value =builtin_unset(data);
		return (1);
	}
	return (0);
}

static int	handle_env_exit(t_command *current_cmd, t_data *data)
{
	if (strcmp(current_cmd->args[0], "env") == 0)
	{
		data->exit_value =builtin_env(data->env);
		return (1);
	}
	if (strcmp(current_cmd->args[0], "exit") == 0)
	{
		data->exit_value =builtin_exit(data);
		return (1);
	}
	return (0);
}

int	try_builtin(t_command *current_cmd, t_data *data, int is_parent)
{
	(void)is_parent;
	if (!data->cmd->args || !data->cmd->args[0])
		return (0);
	if (handle_echo_cd_pwd(current_cmd, data))
		return (1);
	if (handle_export_unset(current_cmd, data))
		return (1);
	if (handle_env_exit(current_cmd, data))
		return (1);
	return (0);
}
