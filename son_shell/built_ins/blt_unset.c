/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   blt_unset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 11:22:28 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/07 14:49:22 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "built_in.h"

static int	find_env_index(char **env, char *arg)
{
	int	j;
	int	arg_len;

	arg_len = ft_strlen(arg);
	j = 0;
	while (env[j])
	{
		if (ft_strncmp(env[j], arg, arg_len) == 0 && env[j][arg_len] == '=')
			return (j);
		j++;
	}
	return (-1); // Standart olarak "bulunamadı" için -1 kullanalım.
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

int	builtin_unset(t_data *data)
{
	int	i;
	int	status;
	int	env_index;

	// Argümansız `unset` bir hata değildir, 0 döndürür.
	if (!data->cmd->args[1])
		return (0);
	i = 1;
	status = 0;
	while (data->cmd->args[i])
	{
		// `export` gibi, argümanın geçerli bir değişken adı olup olmadığını kontrol et.
		if (is_valid_var(data->cmd->args[i]))
		{
			write(2, "unset: `", 8);
			write(2, data->cmd->args[i], ft_strlen(data->cmd->args[i]));
			write(2, "': not a valid identifier\n", 26);
			status = 1; // Hata durumunu ayarla.
		}
		else
		{
			// Değişkeni bul ve sil. Bulunamazsa sorun değil.
			env_index = find_env_index(data->env, data->cmd->args[i]);
			if (env_index != -1)
				remove_env_var(data->env, env_index);
		}
		i++;
	}
	return (status);
}