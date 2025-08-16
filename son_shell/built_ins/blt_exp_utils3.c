/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   blt_exp_utils3.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 14:11:26 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/16 14:11:26 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "built_in.h"
#include <stdio.h>

void	print_env_vars(char **copy, int count)
{
	int		i;
	char	*eq;

	i = 0;
	while (i < count)
	{
		eq = ft_strchr(copy[i], '=');
		if (eq)
		{
			if (*(eq + 1) != '\0')
				printf("declare -x %.*s=\"%s\"\n", (int)(eq - copy[i]),
					copy[i], eq + 1);
			else
				printf("declare -x %.*s\n", (int)(eq - copy[i]), copy[i]);
		}
		else
			printf("declare -x %s\n", copy[i]);
		free(copy[i]);
		i++;
	}
}

void	cleanup_and_return(char *var_name, char *var_value)
{
	free(var_name);
	free(var_value);
}

int	print_sorted_env(char **env)
{
	int		count;
	int		i;
	char	**copy;

	if (!env)
		return (1);
	count = 0;
	while (env[count])
		count++;
	copy = malloc((count + 1) * sizeof(char *));
	if (!copy)
		return (1);
	i = 0;
	while (i < count)
	{
		copy[i] = ft_strdup(env[i]);
		i++;
	}
	copy[count] = NULL;
	bubble_sort_env(copy, count);
	print_env_vars(copy, count);
	free(copy);
	return (0);
}

int	add_new_var(t_data *data, char *new_entry)
{
	char	**new_env;

	new_env = create_new_env(data->env, new_entry);
	if (!new_env)
	{
		free(new_entry);
		perror("export");
		return (0);
	}
	free(data->env);
	data->env = new_env;
	return (1);
}
