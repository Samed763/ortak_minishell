/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apply_input.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/16 16:27:54 by sadinc            #+#    #+#             */
/*   Updated: 2025/07/16 18:39:49 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void heredoc_child_process(int *pipefd, t_command *cmd)
{
    t_heredoc_line *current_line;
    
    close(pipefd[0]);  // Read end kapat
    
    current_line = cmd->heredoc_lines;
    while (current_line)
    {
        write(pipefd[1], current_line->content, ft_strlen(current_line->content));
        write(pipefd[1], "\n", 1);
        current_line = current_line->next;
    }
    
    close(pipefd[1]);  // Write end kapat
    exit(0);
}

static void heredoc_parent_process(int *pipefd, pid_t pid)
{
    close(pipefd[1]);  // Write end kapat
    
    if (dup2(pipefd[0], STDIN_FILENO) == -1)
    {
        perror("dup2 heredoc");
        close(pipefd[0]);
        exit(1);
    }
    
    close(pipefd[0]);  // Read end kapat
    wait(NULL);        // Child'ı bekle
}

static void input_heredoc(t_command *cmd)
{
    pid_t pid;
    int pipefd[2];
    
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        exit(1);
    }
    
    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(1);
    }
    else if (pid == 0)
        heredoc_child_process(pipefd, cmd);
    else
        heredoc_parent_process(pipefd, pid);
}

static void input_file(t_command *cmd)
{
    int fd;
    fd = open(cmd->input_files,O_RDONLY);
    if (fd == -1)
    {
        perror("open input file");
        exit(1);
    }
    if (dup2(fd, STDIN_FILENO) == -1)
    {
        perror("dup2 input");
        close(fd);
        exit(1);
    }
    close(fd);
}

void apply_input_redirection(t_command *cmd)
{
    if(cmd->heredoc_delimiter)
    {
        if (cmd->heredoc_lines != NULL)
            input_heredoc(cmd);
        else
        {
            // Boş heredoc durumunda /dev/null'a yönlendir
            int fd = open("/dev/null", O_RDONLY);
            if (fd != -1)
            {
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
        }
    }
    else if(cmd->input_files != NULL)
        input_file(cmd);
}
