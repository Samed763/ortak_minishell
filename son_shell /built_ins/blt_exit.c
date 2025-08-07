/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   blt_exit.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 11:22:44 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/02 11:22:44 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <limits.h>

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

void	cleanup_and_exit(t_data *data, int exit_code)
{
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

int	builtin_exit(t_data *data)
{
	long	status;
	char	**args;

	args = data->cmd->args;
	printf("exit\n");
	if (!args[1])
	{
		status = data->exit_value;
		cleanup_and_exit(data, status);
	}
	if (ft_atol(data->cmd->args[1], &status, 0))
	{
		write(2, "minishell: exit: ", 17);
		write(2, args[1], ft_strlen(args[1]));
		write(2, ": numeric argument required\n", 28);
		cleanup_and_exit(data, 2);
	}
	if (args[2])
	{
		write(2, "minishell: exit: too many arguments\n", 37);
		data->exit_value = 1;
		return (1);
	}
	cleanup_and_exit(data, status % 256);
	return (0);
}
