#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "../minishell.h"


static int	handle_echo_cd_pwd(t_data *data)
{
	if (strcmp(data->cmd->args[0], "echo") == 0)
	{
		builtin_echo(data->cmd->args);
		return (1);
	}
	if (strcmp(data->cmd->args[0], "cd") == 0)
	{
		builtin_cd(data->cmd->args);
		return (1);
	}
	if (strcmp(data->cmd->args[0], "pwd") == 0)
	{
		builtin_pwd();
		return (1);
	}
	return (0);
}

static int	handle_export_unset(t_data *data)
{
	if (strcmp(data->cmd->args[0], "export") == 0)
	{
		builtin_export(data);
		return (1);
	}
	if (strcmp(data->cmd->args[0], "unset") == 0)
	{
		builtin_unset(data);
		return (1);
	}
	return (0);
}
static int	handle_env_exit(t_data *data)
{
	if (strcmp(data->cmd->args[0], "env") == 0)
	{
		builtin_env(data->env);
		return (1);
	}
	if (strcmp(data->cmd->args[0], "exit") == 0)
	{
		// **** DEĞİŞİKLİK BURADA ****
		// İkinci argüman olarak 'data->exit_value' eklenmeli.
		builtin_exit(data->cmd->args, data->exit_value);
		return (1);
	}
	return (0);
}

int	try_builtin(t_data *data, int is_parent)
{
	(void)is_parent; // Şimdilik kullanılmadığı için uyarıyı engelle

	if (!data->cmd->args || !data->cmd->args[0])
		return (0);
	if (handle_echo_cd_pwd(data))
		return (1);
	if (handle_export_unset(data))
		return (1);
	if (handle_env_exit(data)) // Burası artık doğru çalışacak
		return (1);
	return (0);
}