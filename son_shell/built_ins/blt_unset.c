/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   blt_unset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 11:22:28 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/15 20:58:50 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "built_in.h"

static int	find_env_index(char **env, char *arg)
{
	int		j;
	int		arg_len;

	arg_len = ft_strlen(arg);
	j = 0;
	while (env[j])
	{
		if (ft_strncmp(env[j], arg, arg_len) == 0
			&& env[j][arg_len] == '=')
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

	env_index = find_env_index(data->env, arg);
	if (env_index != -1)
		remove_env_var(data->env, env_index);
	return (0);
}

int	builtin_unset(t_data *data)
{
	int	i;
	int	status;
	int	arg_status;

	if (!data || !data->cmd || !data->env)
		return (1);
	if (!data->cmd->args[1])
		return (0);
	i = 1;
	status = 0;
	arg_status = 0;
	while (data->cmd->args[i])
	{
		arg_status = process_unset_arg(data, data->cmd->args[i]);
		if (arg_status == 1)
			status = 1;
		i++;
	}
	return (status);
}
