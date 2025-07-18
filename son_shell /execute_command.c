#include "minishell.h"

static int is_builtin(char *command)
{
    if (!command)
        return (0);
    if (ft_strcmp(command, "echo") == 0) return (1);
    if (ft_strcmp(command, "cd") == 0) return (1);
    if (ft_strcmp(command, "pwd") == 0) return (1);
    if (ft_strcmp(command, "export") == 0) return (1);
    if (ft_strcmp(command, "unset") == 0) return (1);
    if (ft_strcmp(command, "env") == 0) return (1);
    if (ft_strcmp(command, "exit") == 0) return (1);
    return (0);
}

int is_accessable(char *command, char **splited_path, char **full_path)
{
    int i;
    char *temp_path;

    if (ft_strchr(command, '/'))
    {
        if (access(command, X_OK) == 0)
        {
            *full_path = ft_strdup(command);
            return (0);
        }
        return (-1);
    }
    i = 0;
    while (splited_path[i])
    {
        temp_path = ft_strjoin(splited_path[i], "/");
        *full_path = ft_strjoin(temp_path, command);
        free(temp_path);
        if (access(*full_path, X_OK) == 0)
            return (i);
        free(*full_path);
        i++;
    }
    *full_path = NULL;
    return (-1);
}

void execute(char *full_path, t_data *data)
{
    if (execve(full_path, data->cmd->args, data->env) == -1)
    {
        perror("execve");
        exit(1);
    }
}

static void single_execute(t_data *data, char **splitted_path)
{
    char *full_path;
    int status;
    pid_t pid;
    struct sigaction sa_child; // YENİ: Child için sinyal yapısı

    if (is_accessable(data->cmd->args[0], splitted_path, &full_path) == -1)
    {
        // ... command not found hatası ...
        return;
    }

    // YENİ: Child prosesin varsayılan sinyal davranışını ayarlıyoruz
    sa_child.sa_handler = SIG_DFL; // SIG_DFL = Default Signal Handler
    sa_child.sa_flags = 0;
    sigemptyset(&sa_child.sa_mask);

    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        free(full_path);
        return;
    }
    if (pid == 0) // ------ BURASI CHILD PROCESS ------
    {
        // YENİ: Sinyalleri varsayılan davranışlarına geri döndür.
        // Böylece Ctrl-C bu işlemi sonlandırabilir.
        sigaction(SIGINT, &sa_child, NULL);
        sigaction(SIGQUIT, &sa_child, NULL);

        // Mevcut kodun: Yönlendirmeleri uygula ve komutu çalıştır
        apply_input_redirection(data->cmd);
        apply_output_redirection(data->cmd);
        execute(full_path, data);
    }
    else // ------ PARENT PROCESS ------
{
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);

    free(full_path);
    wait(&status); // Child'ın bitmesini bekle

    // YENİ: Eğer child SIGINT ile sonlandıysa, yeni satır ekle
    if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
    {
        write(STDOUT_FILENO, "\n", 1);
    }

    signal(SIGINT, signal_handler);
    signal(SIGQUIT, SIG_IGN);

    if (WIFEXITED(status))
        data->exit_value = WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
        data->exit_value = 128 + WTERMSIG(status);
    else
        data->exit_value = 1;
}
}
void execute_commmand(t_data *data)
{
    char **splitted_path;

    if (!data->cmd || !data->cmd->args || !data->cmd->args[0] ||
        ft_strlen(data->cmd->args[0]) == 0)
        return;
    splitted_path = ft_split(find_value_by_key(data, "PATH"), ':');
    if (!splitted_path)
        return;
    
    if (is_builtin(data->cmd->args[0]) && !data->cmd->next)
    {
        // Yönlendirmeleri burada ele almamız gerekebilir (zor kısım)
        // Şimdilik basitçe çalıştıralım:
        data->exit_value = try_builtin(data, 1); // 1 = is_parent
        return;
    }
    else if (!data->cmd->next)
        single_execute(data, splitted_path);
    else
        pipe_execute(data, splitted_path);
}
