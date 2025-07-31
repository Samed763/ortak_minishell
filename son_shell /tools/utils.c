/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 16:19:20 by sadinc            #+#    #+#             */
/*   Updated: 2025/07/31 17:19:45 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	free_word_array(char **array)
{
	int	i;

	if (!array)
		return ;
	i = 0;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}

char	*remove_d_quotes(char *str)
{
	int		len;
	int		start;
	int		end;
	char	*result;

	len = ft_strlen(str);
	start = 0;
	end = len - 1;
	if (len >= 2 && ((str[0] == '"' && str[end] == '"')))
	{
		start = 1;
		end = len - 2;
	}
	result = ft_substr(str, start, end - start + 1);
	return (result);
}

char	**copy_env(char **envp)
{
	int		i;
	char	**new_env;

	i = 0;
	while (envp[i])
		i++;
	new_env = malloc(sizeof(char *) * (i + 1));
	if (!new_env)
		return (NULL);
	i = 0;
	while (envp[i])
	{
		new_env[i] = ft_strdup(envp[i]);
		i++;
	}
	new_env[i] = NULL;
	return (new_env);
}

static char	*handle_exit_status(t_data *data)
{
	char	*exit_str;

	exit_str = ft_itoa(data->exit_value);
	if (!exit_str)
		return (NULL);
	return (exit_str);
}

char	*find_value_by_key(t_data *data, char *key)
{
	int		i;
	char	*search;
	char	*result;

	if (!key || !data || !data->env)
		return (NULL);
	if (ft_strcmp(key, "?") == 0)
		return (handle_exit_status(data));
	i = 0;
	search = ft_strjoin(key, "=");
	if (!search)
		return (NULL);
	while (data->env[i])
	{
		if (ft_strncmp(data->env[i], search, ft_strlen(search)) == 0)
		{
			result = ft_strdup(data->env[i] + ft_strlen(search));
			free(search);
			return (result);
		}
		i++;
	}
	free(search);
	return (NULL);
}
