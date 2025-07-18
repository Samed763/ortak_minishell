#include "minishell.h"

static int	find_env_index(char **env, char *arg)
{
	int	j;
	int	arg_len;

	arg_len = ft_strlen(arg);
	j = 0;
	while (env[j])
	{
		if (ft_strncmp(env[j], arg, arg_len) == 0 && 
			env[j][arg_len] == '=')
			return (j);
		j++;
	}
	return (-1);
}

static void	remove_env_var(char **env, int env_index)
{
	free(env[env_index]);
	while (env[env_index + 1])
	{
		env[env_index] = env[env_index + 1];
		env_index++;
	}
	env[env_index] = NULL;
}

static int	process_unset_arg(t_data *data, char *arg)
{
	int	env_index;

	if (!is_valid_var(arg))
	{
		fprintf(stderr, "unset: '%s': not a valid identifier\n", arg);
		return (EXIT_FAILURE);
	}
	env_index = find_env_index(data->env, arg);
	if (env_index != -1)
		remove_env_var(data->env, env_index);
	return (EXIT_SUCCESS);
}

int	builtin_unset(t_data *data)
{
	int	i;
	int	status;
	int	arg_status;

	if (!data || !data->cmd || !data->cmd->args[1] || !data->env)
		return (EXIT_FAILURE);
	i = 1;
	status = EXIT_SUCCESS;
	while (data->cmd->args[i])
	{
		arg_status = process_unset_arg(data, data->cmd->args[i]);
		if (arg_status == EXIT_FAILURE)
			status = EXIT_FAILURE;
		i++;
	}
	return (status);
}
