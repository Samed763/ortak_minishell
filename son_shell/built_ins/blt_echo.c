/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   blt_echo.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 11:22:48 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/02 11:22:48 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "built_in.h"

static int	is_valid_n_flag(char *str)
{
	int	i;

	if (!str || str[0] != '-')
		return (0);
	i = 1;
	while (str[i])
	{
		if (str[i] != 'n')
			return (0);
		i++;
	}
	return (i > 1);
}

static int	has_valid_next_arg(char **args, int current_index)
{
	int	j;

	j = current_index + 1;
	while (args[j])
	{
		if (args[j] && ft_strlen(args[j]) > 0)
			return (1);
		j++;
	}
	return (0);
}

int	builtin_echo(char **args)// buraya data'yı al 
{
	int	i;
	int	newline;

	newline = 1;
	i = 1;
	if (args[i] && is_valid_n_flag(args[i]))
	{
		newline = 0;
		while (args[i] && is_valid_n_flag(args[i]))
			i++;
	}
	while (args[i])
	{
		if (args[i] && ft_strlen(args[i]) > 0)
		{
			write(STDOUT_FILENO, args[i], ft_strlen(args[i]));
			if (has_valid_next_arg(args, i))
				write(STDOUT_FILENO, " ", 1);
		}
		i++;
	}
	if (newline)
		write(STDOUT_FILENO, "\n", 1);
	return (0);
}
