/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 20:00:01 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/02 11:22:02 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	process_line(char *line, t_data *data)
{
	if (!line)
	{
		printf("exit\n");
		cleanup_and_exit(data, data->exit_value);
	}
	if (line && *line)
		add_history(line);
	if (syntax_check(line))
	{
		printf("syntax error\n");
		free(line);
		return ;
	}
	lexer(line, data);
	expander(data);
	data->cmd = parser(data);
	if (data->cmd != NULL)
		execute_command(data);
	free_data_resources(data);
	free(line);
}

static void	main_loop(t_data *data)
{
	char	*line;

	while (1)
	{
		line = readline("minishell$ ");
		process_line(line, data);
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_data	data;

	(void)argc;
	(void)argv;
	init_data(&data, envp);
	setup_signals();
	main_loop(&data);
	cleanup_and_exit(&data, data.exit_value);
	return (data.exit_value);
}
