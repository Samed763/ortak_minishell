/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   blt_exp_utils2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 11:22:40 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/15 16:54:41 by yant56           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "built_in.h"

void	bubble_sort_env(char **copy, int count)
{
	int		i;
	int		j;
	char	*tmp;

	i = 0;
	while (i < count - 1)
	{
		j = 0;
		while (j < count - i - 1)
		{
			if (ft_strcmp(copy[j], copy[j + 1]) > 0)
			{
				tmp = copy[j];
				copy[j] = copy[j + 1];
				copy[j + 1] = tmp;
			}
			j++;
		}
		i++;
	}
}

int	is_valid_var(const char *str)
{
	int	i;

	if (!str || !*str)
		return (0);
	if (!(ft_isalpha(*str) || *str == '_'))
		return (0);
	i = 1;
	while (str[i] && str[i] != '=')
	{
		if (!(ft_isalnum(str[i]) || str[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}

int	find_env_var(char **env, char *new_entry, int name_len)
{
	int	j;

	j = 0;
	while (env[j])
	{
		if (ft_strncmp(env[j], new_entry, name_len) == 0
			&& env[j][name_len] == '=')
			return (j);
		j++;
	}
	return (-1);
}

char	**create_new_env(char **env, char *new_entry)
{
	int		env_size;
	int		j;
	char	**new_env;

	env_size = 0;
	while (env[env_size])
		env_size++;
	new_env = malloc((env_size + 2) * sizeof(char *));
	if (!new_env)
		return (NULL);
	j = 0;
	while (j < env_size)
	{
		new_env[j] = env[j];
		j++;
	}
	new_env[env_size] = new_entry;
	new_env[env_size + 1] = NULL;
	return (new_env);
}

int	find_exact_var(char **env, char *arg)
{
	int	j;

	j = 0;
	while (env[j])
	{
		if (ft_strncmp(env[j], arg, ft_strlen(arg)) == 0)
			return (j);
		j++;
	}
	return (-1);
}
