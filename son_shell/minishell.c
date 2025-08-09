/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 20:00:01 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/09 22:26:27 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	g_signal_received = 0;

t_data	*get_data_instance(t_data *data_to_set)
{
	static t_data	*data_instance = NULL;

	if (data_to_set)
		data_instance = data_to_set;
	return (data_instance);
}

// YENİ: Programın başında çağrılarak singleton'ı başlatır.
void	init_data_instance(t_data *data)
{
	get_data_instance(data);
}
static void	process_line(char *line, t_data *data)
{
	if (!line)
	{
		printf("exit\n");
		cleanup_and_exit(data->exit_value);
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
		g_signal_received = 0;
		line = readline("minishell$ ");
		if (g_signal_received == SIGINT)
			data->exit_value = 130; 
		process_line(line, data);
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_data	data;

	(void)argc;
	(void)argv;
	init_data(&data, envp);
	init_data_instance(&data);
	if (setup_signals() == -1)
		cleanup_and_exit(1);
	main_loop(&data);
	cleanup_and_exit(data.exit_value);
	return (data.exit_value);
}
