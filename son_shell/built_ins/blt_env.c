/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   blt_env.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 11:22:46 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/02 11:22:46 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "built_in.h"
# include <stdio.h>

int	builtin_env(char **env)
{
	int		i;
	char	*eq;

	i = 0;
	while (env[i])
	{
		eq = ft_strchr(env[i], '=');
		// Sadece değeri olan variable'ları yazdır
		if (eq && *(eq + 1) != '\0')
		{
			printf("%s\n", env[i]);
		}
		i++;
	}
	return (0);
}
