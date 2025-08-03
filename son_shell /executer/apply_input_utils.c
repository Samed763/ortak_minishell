/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apply_input_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 19:53:45 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/02 17:42:54 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	heredoc_child_process(t_data *data, int *pipefd, t_command *cmd)
{
	t_heredoc_line	*current_line;

	close(pipefd[0]);
	current_line = cmd->heredoc_lines;
	while (current_line)
	{
		write(pipefd[1], current_line->content,
			ft_strlen(current_line->content));
		write(pipefd[1], "\n", 1);
		current_line = current_line->next;
	}
	close(pipefd[1]);
	if (execve("/bin/true", (char *[]){"/bin/true", NULL}, (char *[]){NULL}))
	{
		perror("execve");
		cleanup_and_exit(data, 1);
	}
}

int	heredoc_parent_process(int *pipefd)
{
	close(pipefd[1]);
	if (dup2(pipefd[0], STDIN_FILENO) == -1)
	{
		perror("dup2 heredoc");
		close(pipefd[0]);
		return (-1);
	}
	close(pipefd[0]);
	wait(NULL);
	return (0);
}
