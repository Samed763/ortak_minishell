#include "../minishell.h"

static int is_builtin(char *command)
{
    if (!command)
        return (0);
    if (ft_strcmp(command, "echo") == 0)
        return (1);
    if (ft_strcmp(command, "cd") == 0)
        return (1);
    if (ft_strcmp(command, "pwd") == 0)
        return (1);
    if (ft_strcmp(command, "export") == 0)
        return (1);
    if (ft_strcmp(command, "unset") == 0)
        return (1);
    if (ft_strcmp(command, "env") == 0)
        return (1);
    if (ft_strcmp(command, "exit") == 0)
        return (1);
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
    if (!splited_path)
    {
        *full_path = NULL;
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
        free(full_path); 
        cleanup_and_exit(data,1);
    }
}

static void single_execute(t_data *data, char **splitted_path)
{
    char *full_path;
    pid_t pid;
    int status;

    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        data->exit_value = 1;
        return;
    }
    if (pid == 0) // Çocuk Proses
    {
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);

        // Yönlendirmeleri uygula, hata olursa 1 ile çık.
        if (apply_input_redirection(data->cmd) == -1 ||
            apply_output_redirection(data->cmd) == -1)
            cleanup_and_exit(data, 1);

        // Eğer çalıştırılacak bir komut adı varsa (sadece '> out' değilse)
        if (data->cmd->args && data->cmd->args[0])
        {
            if (is_accessable(data->cmd->args[0], splitted_path, &full_path) == -1)
            {
                fprintf(stderr, "%s: command not found\n", data->cmd->args[0]);
                cleanup_and_exit(data, 127); // Standart hata kodu
            }
            execute(full_path, data);
        }
        // Komut olmasa bile (sadece yönlendirme), işlem başarılıdır. 0 ile çık.
        cleanup_and_exit(data, 0);
    }
    else // ------ PARENT PROCESS ------
    {
        signal(SIGINT, SIG_IGN);
        signal(SIGQUIT, SIG_IGN);

        wait(&status); // Child'ın bitmesini bekle

        if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
            write(STDOUT_FILENO, "\n", 1);

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
    char *path_val;
    // --- YENİ VE BASİT YAPI ---

    path_val = find_value_by_key(data, "PATH");
    data->splitted_path = ft_split(path_val, ':');
    if (path_val)
       free(path_val);
    // 1. Pipe'lı bir komut mu?
    if (data->cmd->next)
        pipe_execute(data, data->splitted_path);
    // 2. Tek bir built-in komut mu?
    // Not: data->cmd->args'ın NULL olma ihtimaline karşı kontrol ekliyoruz.
    else if (data->cmd->args && is_builtin(data->cmd->args[0]))
    {
        int original_stdin = dup(STDIN_FILENO);
        int original_stdout = dup(STDOUT_FILENO);
        if (original_stdin == -1 || original_stdout == -1)
        {
            perror("dup");
            data->exit_value = 1;
            if (original_stdin != -1) close(original_stdin);
            if (original_stdout != -1) close(original_stdout);
            return;
        }
        // Yönlendirmeleri uygula
        if (apply_input_redirection(data->cmd) == -1 ||
            apply_output_redirection(data->cmd) == -1)
            data->exit_value = 1;
        else
            data->exit_value = try_builtin(data->cmd, data, 1);
        
        // Standart girdi/çıktıyı geri yükle
         if (dup2(original_stdin, STDIN_FILENO) == -1 ||
            dup2(original_stdout, STDOUT_FILENO) == -1)
        {
            perror("dup2 restore");
            data->exit_value = 1; // Hata durumunda çıkış kodunu ayarla
        }
        close(original_stdin);
        close(original_stdout);
    }
    // 3. Geriye kalan her şey (dış komut VEYA sadece yönlendirme)
    else
        single_execute(data, data->splitted_path);
    
    // Her çalıştırmadan sonra PATH'i temizle
    if (data->splitted_path)
        free_word_array(data->splitted_path);
    data->splitted_path = NULL;
}
