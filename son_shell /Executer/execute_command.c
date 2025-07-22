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
        cleanup_and_exit(data,1);
    }
}

static void single_execute(t_data *data, char **splitted_path)
{
    char *full_path;
    int status;
    pid_t pid;
    struct sigaction sa_child;

    // YENİ: Child prosesin varsayılan sinyal davranışını ayarlıyoruz
    sa_child.sa_handler = SIG_DFL;
    sa_child.sa_flags = 0;
    sigemptyset(&sa_child.sa_mask);

    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        data->exit_value = 1;
        return;
    }
    if (pid == 0) // ------ CHILD PROCESS ------
    {
        // YENİ: Sinyalleri varsayılan davranışlarına geri döndür.
        sigaction(SIGINT, &sa_child, NULL);
        sigaction(SIGQUIT, &sa_child, NULL);

        // **** EN ÖNEMLİ DEĞİŞİKLİK BURADA ****
        // Yönlendirmeleri uygula ve DÖNÜŞ DEĞERLERİNİ KONTROL ET
        if (apply_input_redirection(data->cmd) == -1 ||
            apply_output_redirection(data->cmd) == -1)
        {
            cleanup_and_exit(data,1); // Yönlendirme hatası varsa çocuk proses 1 ile çıksın.
        }

        // Eğer çalıştırılacak bir komut varsa (sadece yönlendirme değilse)...

        if (data->cmd->args && data->cmd->args[0])
        {
            if (is_accessable(data->cmd->args[0], splitted_path, &full_path) == -1)
            {
                fprintf(stderr, "%s: command not found\n", data->cmd->args[0]);
                cleanup_and_exit(data,127);
            }
            execute(full_path, data);
            free(full_path); // Bu satıra sadece execve başarısız olursa ulaşılır.
        }
        exit(0);
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
    char **splitted_path;

    if (!data->cmd)
        return;
    if (!data->cmd->args || !data->cmd->args[0])
    {
        if (data->cmd->input_files || data->cmd->output_files || data->cmd->heredoc_delimiter)
        {
            // Bu durumu tek komut gibi çalıştır, ama çalıştıracak komut yok.
            single_execute(data, NULL);
        }
        return; // Yönlendirme de yoksa hiçbir şey yapma
    }

    splitted_path = ft_split(find_value_by_key(data, "PATH"), ':');

    if (!data->cmd->next && is_builtin(data->cmd->args[0]))
    {
        int original_stdin = dup(STDIN_FILENO);
        int original_stdout = dup(STDOUT_FILENO);
        int redir_error = 0;

        if (original_stdin == -1 || original_stdout == -1)
        {
            perror("dup");
            data->exit_value = 1;
            if (original_stdin != -1)
                close(original_stdin);
            if (original_stdout != -1)
                close(original_stdout);
            free_word_array(splitted_path);
            return;
        }

        if (apply_input_redirection(data->cmd) == -1)
            redir_error = 1;

        if (redir_error == 0 && apply_output_redirection(data->cmd) == -1)
            redir_error = 1;

        if (redir_error)
            data->exit_value = 1;
        else
            data->exit_value = try_builtin(data, 1);

        dup2(original_stdin, STDIN_FILENO);
        dup2(original_stdout, STDOUT_FILENO);
        close(original_stdin);
        close(original_stdout);
    }
    else if (!data->cmd->next)
        single_execute(data, splitted_path);
    else
        pipe_execute(data, splitted_path);

    free_word_array(splitted_path);
}
