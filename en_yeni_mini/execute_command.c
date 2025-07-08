#include "minishell.h"



void apply_output_redirection(t_command *cmd)
{
    int fd;
    int i;
    
    if (!cmd->output_files || cmd->output_count == 0)
        return;
    
    // İlk dosyalardan sonuncusu hariç hepsini aç ve kapat (boş dosya oluştur)
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
    
    // Son dosyayı stdout'a yönlendir
    if (cmd->append_modes[cmd->output_count - 1])
        fd = open(cmd->output_files[cmd->output_count - 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
    else
        fd = open(cmd->output_files[cmd->output_count - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }
    
    if (dup2(fd, STDOUT_FILENO) == -1)
    {
        perror("dup2");
        close(fd);
        exit(1);
    }
    close(fd);
}



int	is_accessable(char **splited_path, char *command)
{
    int		i;
    char	*temp_path;
    char	*full_path;

    // Eğer command ./ veya / ile başlıyorsa PATH'te arama
    if (ft_strchr(command, '/'))
    {
        if (access(command, X_OK) == 0)
            return (-2); // Özel return değeri: direkt path kullan
        return (-1);
    }

    i = 0;
    while (splited_path[i])
    {
        temp_path = ft_strjoin(splited_path[i], "/");
        full_path = ft_strjoin(temp_path, command);
        free(temp_path);
        if (access(full_path, X_OK) == 0)
        {
            free(full_path);
            return (i);
        }
        free(full_path);
        i++;
    }
    return (-1);
}

void	execute(char *full_path, t_data *data)
{
    if (execve(full_path, data->cmd->args, data->env) == -1)
    {
        perror("execve");
        exit(1);
    }
}

void	pipe_execute(t_data *data, char **splitted_path)
{
    t_command *current;
    int pipefd[2];
    int prev_fd = -1;
    pid_t pid;
    char *full_path;
    char *temp_path;
    int path_i;

    current = data->cmd;
    
    while (current)
    {
        // Son command değilse pipe oluştur
        if (current->next)
        {
            if (pipe(pipefd) == -1)
            {
                perror("pipe");
                return;
            }
        }

        pid = fork();
        if (pid == -1)
        {
            perror("fork");
            return;
        }

        if (pid == 0) // Child process
        {
            // Input redirection (önceki pipe'tan)
            if (prev_fd != -1)
            {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }

            // Output redirection (sonraki pipe'a veya dosyaya)
            if (current->next)
            {
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
                close(pipefd[0]);
            }
            else
            {
                // Son command - output redirection uygula
                apply_output_redirection(current);
            }

            // Command'ı execute et
            if (!current->args || !current->args[0])
                exit(1);
                
            path_i = is_accessable(splitted_path, current->args[0]);
            if (path_i == -1)
            {
                printf("\nminishell: %s: command not found\n", current->args[0]);
                exit(1);
            }

            // Direkt path kullan
            if (path_i == -2)
                full_path = ft_strdup(current->args[0]);
            else
            {
                temp_path = ft_strjoin(splitted_path[path_i], "/");
                full_path = ft_strjoin(temp_path, current->args[0]);
                free(temp_path);
            }
        }
        else // Parent process
        {
            // Önceki pipe'ı kapat
            if (prev_fd != -1)
                close(prev_fd);

            // Pipe'ın write end'ini kapat, read end'ini sakla
            if (current->next)
            {
                close(pipefd[1]);
                prev_fd = pipefd[0];
            }
        }

        current = current->next;
    }

    // Son pipe'ı kapat
    if (prev_fd != -1)
        close(prev_fd);

    // Tüm child process'leri bekle
    current = data->cmd;
    while (current)
    {
        wait(NULL);
        current = current->next;
    }
}



void	single_execute(t_data *data, char **splitted_path)
{
    char	*full_path;
    char	*temp_path;
    int		path_i;
    pid_t	pid;

    if (!data->cmd || !data->cmd->args || !data->cmd->args[0])
        return ;
    
    path_i = is_accessable(splitted_path, data->cmd->args[0]);
    if (path_i == -1)
    {
        printf("\nminishell: %s: command not found\n", data->cmd->args[0]);
        return ;
    }
    
    // Direkt path kullan (./minishell, /bin/ls gibi)
    if (path_i == -2)
        full_path = ft_strdup(data->cmd->args[0]);
    else
    {
        temp_path = ft_strjoin(splitted_path[path_i], "/");
        full_path = ft_strjoin(temp_path, data->cmd->args[0]);
        free(temp_path);
    }
    
    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        free(full_path);
        return ;
    }
    if (pid == 0)
    {
        apply_output_redirection(data->cmd);
        execute(full_path, data);
    }
    else
    {
        free(full_path);
        wait(NULL);
    }
}

void	execute_command(t_data *data)
{
    char	**splitted_path;

    splitted_path = ft_split(find_value_by_key(data, "PATH"), ':');
    if (!splitted_path)
        return ;
    if (!data->cmd->next)
        single_execute(data, splitted_path);
    else
        pipe_execute(data, splitted_path);
    free_word_array(splitted_path);
}