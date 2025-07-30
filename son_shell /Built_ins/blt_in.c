#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "../minishell.h"

static int	handle_echo_cd_pwd(t_command *current_cmd, t_data *data)
{
	(void)data;
	if (strcmp(current_cmd->args[0], "echo") == 0)
	{
		builtin_echo(current_cmd->args);
		return (1);
	}
	if (strcmp(current_cmd->args[0], "cd") == 0)
	{
		builtin_cd(current_cmd->args);
		return (1);
	}
	if (strcmp(current_cmd->args[0], "pwd") == 0)
	{
		builtin_pwd();
		return (1);
	}
	return (0);
}

static int	handle_export_unset(t_command *current_cmd, t_data *data)
{
	if (strcmp(current_cmd->args[0], "export") == 0)
	{
		builtin_export(data);
		return (1);
	}
	if (strcmp(current_cmd->args[0], "unset") == 0)
	{
		builtin_unset(data);
		return (1);
	}
	return (0);
}

static int	handle_env_exit(t_command *current_cmd, t_data *data)
{
	if (strcmp(current_cmd->args[0], "env") == 0)
	{
		builtin_env(data->env);
		return (1);
	}
	if (strcmp(current_cmd->args[0], "exit") == 0)
	{
		builtin_exit(data);
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
