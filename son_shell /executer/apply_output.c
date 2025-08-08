/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apply_output.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 19:43:21 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/08 10:02:52 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// static int	create_empty_output_files(t_command *cmd)
// {
// 	int	fd;
// 	int	i;

// 	i = 0;
// 	while (i < cmd->output_count - 1)
// 	{
// 		if (cmd->append_modes[i])
// 			fd = open(cmd->output_files[i], O_WRONLY | O_CREAT | O_APPEND,
// 					0644);
// 		else
// 			fd = open(cmd->output_files[i], O_WRONLY | O_CREAT | O_TRUNC, 0644);
// 		if (fd == -1)
// 		{
// 			perror(cmd->output_files[i]);
// 			return (-1);
// 		}
// 		close(fd);
// 		i++;
// 	}
// 	return (0);
// }

// int	apply_output_redirection(t_command *cmd)
// {
// 	int		fd;
// 	char	*last_file;

// 	if (!cmd || !cmd->output_files || cmd->output_count == 0)
// 		return (0);
// 	if (create_empty_output_files(cmd) == -1)
// 		return (-1);
// 	last_file = cmd->output_files[cmd->output_count - 1];
// 	if (cmd->append_modes[cmd->output_count - 1])
// 		fd = open(last_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
// 	else
// 		fd = open(last_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
// 	if (fd == -1)
// 	{
// 		perror(last_file);
// 		return (-1);
// 	}
// 	if (dup2(fd, STDOUT_FILENO) == -1)
// 	{
// 		perror("dup2");
// 		close(fd);
// 		return (-1);
// 	}
// 	close(fd);
// 	return (0);
// }
