/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   blt_exit.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 14:11:11 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/16 14:11:11 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "built_in.h"
#include <limits.h>
#include <unistd.h>
#include <stdio.h>
#include <readline/history.h>

static int	ft_atol(const char *str, long *result, int i)
{
	int				sign;
	unsigned long	num;

	*result = 0;
	sign = 1;
	num = 0;
	if (str[0] == '-' || str[0] == '+')
	{
		if (str[0] == '-')
			sign = -1;
		i++;
	}
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (1);
		num = num * 10 + (str[i] - '0');
		if ((sign == 1 && num > LONG_MAX)
			|| (sign == -1 && num > (unsigned long)LONG_MAX + 1))
			return (1);
		i++;
	}
	*result = (long)(sign * num);
	return (0);
}

void	cleanup_and_exit(int exit_code)
{
	t_data	*data;

	data = get_data_instance(NULL);
	if (data)
	{
		if (data->original_stdin != -1)
			close(data->original_stdin);
		if (data->original_stdout != -1)
			close(data->original_stdout);
		free_data_resources(data);
		if (data->splitted_path)
			free_word_array(data->splitted_path);
		data->splitted_path = NULL;
		if (data->env)
			free_word_array(data->env);
		data->env = NULL;
	}
	clear_history();
	exit(exit_code);
}

int	builtin_exit(char **args, t_data *data, int is_parent)
{
	long	status;

	if (is_parent)
		printf("exit\n");
	if (!args[1])
		status = data->exit_value;
	else if (ft_atol(args[1], &status, 0))
	{
		ft_putstr_fd("minishell: exit: ", 2);
		ft_putstr_fd(args[1], 2);
		ft_putendl_fd(": numeric argument required", 2);
		status = 2;
	}
	else if (args[2])
	{
		write(2, "minishell: exit: too many arguments\n", 37);
		if (is_parent)
		{
			data->exit_value = 1;
			return (1);
		}
		status = 1;
	}
	cleanup_and_exit(status % 256);
	return (0);
}
