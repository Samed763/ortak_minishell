#include "minishell.h"

extern char **environ;

char *find_command_path(char *command)
{
    char *paths[] = {"/bin/", "/usr/bin/", "/usr/local/bin/", NULL};
    char *full_path;
    int i = 0;
    
    if (!command)
        return NULL;
    
    if (ft_strchr(command, '/'))
        return ft_strdup(command);
    
    while (paths[i])
    {
        full_path = Malloc(ft_strlen(paths[i]) + ft_strlen(command) + 1);
        ft_memcpy(full_path, paths[i], ft_strlen(paths[i]));
        ft_memcpy(full_path + ft_strlen(paths[i]), command, ft_strlen(command));
        full_path[ft_strlen(paths[i]) + ft_strlen(command)] = '\0';
        
        if (access(full_path, X_OK) == 0)
            return full_path;
        
        free(full_path);
        i++;
    }
    
    return NULL;
}

int setup_redirections(t_commands *cmd)
{
    int fd;
    
    if (cmd->input_file)
    {
        fd = open(cmd->input_file, O_RDONLY);
        if (fd == -1)
        {
            perror(cmd->input_file);
            return -1;
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }
    
    if (cmd->output_file)
    {
        if (cmd->append_mode)
            fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        else
            fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        
        if (fd == -1)
        {
            perror(cmd->output_file);
            return -1;
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
    
    return 0;
}

int execute_single_command(t_commands *cmd)
{
    pid_t pid;
    int status;
    char *command_path;
    
    if (!cmd || !cmd->args || !cmd->args[0])
        return 1;
    
    if (is_builtin(cmd->args[0]))
        return execute_builtin(cmd->args);
    
    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        return 1;
    }
    
    if (pid == 0)
    {
        if (setup_redirections(cmd) == -1)
            exit(1);
        
        command_path = find_command_path(cmd->args[0]);
        if (!command_path)
        {
            printf("minishell: %s: command not found\n", cmd->args[0]);
            exit(127);
        }
        
        execve(command_path, cmd->args, environ);
        perror("execve");
        free(command_path);
        exit(1);
    }
    else
    {
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
}

int execute_pipeline(t_commands *commands)
{
    t_commands *current = commands;
    int pipe_fd[2];
    int prev_fd = -1;
    pid_t pid;
    int status = 0;
    
    while (current)
    {
        if (current->next)
        {
            if (pipe(pipe_fd) == -1)
            {
                perror("pipe");
                return 1;
            }
        }
        
        pid = fork();
        if (pid == -1)
        {
            perror("fork");
            return 1;
        }
        
        if (pid == 0)
        {
            if (prev_fd != -1)
            {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }
            
            if (current->next)
            {
                dup2(pipe_fd[1], STDOUT_FILENO);
                close(pipe_fd[0]);
                close(pipe_fd[1]);
            }
            
            setup_redirections(current);
            
            if (is_builtin(current->args[0]))
            {
                exit(execute_builtin(current->args));
            }
            else
            {
                char *command_path = find_command_path(current->args[0]);
                if (!command_path)
                {
                    printf("minishell: %s: command not found\n", current->args[0]);
                    exit(127);
                }
                execve(command_path, current->args, environ);
                perror("execve");
                exit(1);
            }
        }
        else
        {
            if (prev_fd != -1)
                close(prev_fd);
            
            if (current->next)
            {
                close(pipe_fd[1]);
                prev_fd = pipe_fd[0];
            }
            
            if (!current->next)
                waitpid(pid, &status, 0);
        }
        
        current = current->next;
    }
    
    while (wait(NULL) > 0);
    return WEXITSTATUS(status);
}

int execute_commands(t_commands *commands)
{
    if (!commands)
        return 1;
    
    if (!commands->next)
        return execute_single_command(commands);
    
    return execute_pipeline(commands);
}
