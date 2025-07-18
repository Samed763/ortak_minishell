#include "minishell.h"

static void create_empty_output_files(t_command *cmd)
{
    int fd;
    int i;
    i = 0;

    while (i < cmd->output_count - 1)
    {
        if (cmd->append_modes[i])
            fd = open(cmd->output_files[i], O_WRONLY | O_CREAT | O_APPEND, 0644);
        else
            fd = open(cmd->output_files[i], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        
        if (fd != -1)
            close(fd);
        i++;
    }
}

void apply_output_redirection(t_command *cmd)
{
    int fd;
    if (!cmd || !cmd->output_files || cmd->output_count == 0)
        return;
    create_empty_output_files(cmd);
    if (cmd->append_modes[cmd->output_count - 1])
        fd = open(cmd->output_files[cmd->output_count - 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
    else
        fd = open(cmd->output_files[cmd->output_count - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    
    if(fd == -1)
    {
        perror("open");
        exit(1);
    }
    else if (dup2(fd, STDOUT_FILENO) == -1)
    {
        perror("dup2");
        close(fd);
        exit(1);
    }
    close(fd);
}
