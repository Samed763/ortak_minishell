/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   blt_in.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 14:11:39 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/16 14:11:41 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "built_in.h"

static int	handle_echo_cd_pwd(t_command *current_cmd, t_data *data)
{
	(void)data;
	if (ft_strcmp(current_cmd->args[0], "echo") == 0)
	{
		data->exit_value = builtin_echo(current_cmd->args);
		return (1);
	}
	if (ft_strcmp(current_cmd->args[0], "cd") == 0)
	{
		data->exit_value = builtin_cd(data, current_cmd->args);
		return (1);
	}
	if (ft_strcmp(current_cmd->args[0], "pwd") == 0)
	{
		data->exit_value = builtin_pwd();
		return (1);
	}
	return (0);
}

static int	handle_export_unset(t_command *current_cmd, t_data *data)
{
	if (ft_strcmp(current_cmd->args[0], "export") == 0)
	{
		data->exit_value = builtin_export(data);
		return (1);
	}
	if (ft_strcmp(current_cmd->args[0], "unset") == 0)
	{
		data->exit_value = builtin_unset(data);
		return (1);
	}
	return (0);
}

static int	handle_env_exit(t_command *current_cmd, t_data *data, int is_parent)
{
	if (ft_strcmp(current_cmd->args[0], "env") == 0)
	{
		if (current_cmd->args[1] == NULL)
			data->exit_value = builtin_env(data->env);
		else
		{
			ft_putstr_fd("env:", 2);
			ft_putstr_fd(current_cmd->args[1], 2);
			ft_putendl_fd(": No such file or directory", 2);
			data->exit_value = 127;
		}
		return (1);
	}
	if (ft_strcmp(current_cmd->args[0], "exit") == 0)
	{
		data->exit_value = builtin_exit(current_cmd->args, data, is_parent);
		return (1);
	}
	return (0);
}

int	try_builtin(t_command *current_cmd, t_data *data, int is_parent)
{
	if (!current_cmd->args || !current_cmd->args[0])
		return (0);
	if (handle_echo_cd_pwd(current_cmd, data))
		return (1);
	if (handle_export_unset(current_cmd, data))
		return (1);
	if (handle_env_exit(current_cmd, data, is_parent))
		return (1);
	return (0);
}
