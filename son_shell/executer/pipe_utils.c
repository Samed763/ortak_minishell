/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 19:53:31 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/08 11:38:15 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	handle_redirections(t_command *cmd)
{
	t_redir *redir;
	int		fd;

	redir = cmd->redirs;
	while (redir)
	{
		if (redir->type == TOKEN_REDIRECT_OUT || redir->type == TOKEN_APPEND)
		{
			fd = open(redir->filename,
					(O_WRONLY | O_CREAT | (O_TRUNC * (redir->type
								== TOKEN_REDIRECT_OUT)) | (O_APPEND * (redir->type
								== TOKEN_APPEND))), 0644);
			if (fd == -1)
				return (perror(redir->filename), -1);
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (redir->type == TOKEN_REDIRECT_IN)
		{
			fd = open(redir->filename, O_RDONLY);
			if (fd == -1)
				return (perror(redir->filename), -1);
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		else if (redir->type == TOKEN_HEREDOC)
			apply_input_redirection(cmd);
		redir = redir->next;
	}
	return (0);
}


// handle_pipe_redirections, her bir pipe adımı için I/O'yu ayarlar.
void	handle_pipe_redirections(t_data *data, t_command *current, int *pipefd,
		int prev_fd)
{
    // Önceki komutun çıktısını (prev_fd) bu komutun girdisine bağla.
	if (prev_fd != -1)
	{
		safe_dup2(prev_fd, STDIN_FILENO, data);
		close(prev_fd);
	}
    // Bu komutun çıktısını bir sonraki komutun girdisine (pipe) bağla.
	if (current->next)
	{
		close(pipefd[0]);
		safe_dup2(pipefd[1], STDOUT_FILENO, data);
		close(pipefd[1]);
	}
    // Dosya yönlendirmelerini uygula (bunlar pipe yönlendirmelerini ezer).
	if (handle_redirections(current) == -1)
		cleanup_and_exit(data, 1);
}

void	pipe_child_routine(t_pipe_data *p_data)
{
	char	*full_path;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	handle_pipe_redirections(p_data->data, p_data->current, p_data->pipefd,
		p_data->prev_fd);
	if (!p_data->current->args || !p_data->current->args[0])
		cleanup_and_exit(p_data->data, 0);
	if (try_builtin(p_data->current, p_data->data, 0))
		cleanup_and_exit(p_data->data, p_data->data->exit_value);
	if (is_accessable(p_data->current->args[0], p_data->data->splitted_path,
			&full_path) == -1)
	{
		write(2, p_data->current->args[0], ft_strlen(p_data->current->args[0]));
		write(2, ": command not found\n", 20);
		cleanup_and_exit(p_data->data, 127);
	}
	if (execve(full_path, p_data->current->args, p_data->data->env) == -1)
	{
		perror("execve");
		free(full_path);
		cleanup_and_exit(p_data->data, 1);
	}
}

// ... (dosyanın geri kalanı aynı)
int	pipe_parent_routine(t_command *current, int *pipefd, int prev_fd)
{
	if (prev_fd != -1)
		close(prev_fd);
	if (current->next)
	{
		close(pipefd[1]);
		return (pipefd[0]);
	}
	return (-1);
}
static int	get_last_pid_and_count(t_command *cmd, pid_t *last_pid)
{
	t_command	*iter;
	int			count;

	iter = cmd;
	count = 0;
	*last_pid = -1;
	while (iter)
	{
		count++;
		if (iter->next == NULL)
			*last_pid = iter->pid;
		iter = iter->next;
	}
	return (count);
}
void	wait_for_all_children(t_data *data)
{
	int		cmd_count;
	pid_t	last_pid;
	pid_t	waited_pid;
	int		status;
	int		last_status;

	if (!data || !data->cmd)
		return ;
	cmd_count = get_last_pid_and_count(data->cmd, &last_pid);
	last_status = 0;
	while (cmd_count > 0)
	{
		waited_pid = wait(&status);
		if (waited_pid == last_pid)
			last_status = status;
		cmd_count--;
	}
	set_exit_status(data, last_status);
}