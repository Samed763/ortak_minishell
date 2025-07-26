#include "../minishell.h"

extern struct termios g_original_termios;
extern volatile sig_atomic_t g_heredoc_interrupted; // Bu satırı da eklemek iyi bir pratik.

static void expand_heredoc_lines(t_data *data,t_command *cmd)
{
    t_heredoc_line *current = cmd->heredoc_lines;
    
    while (current)
    {
        char *expanded = expand_single_line(data,current->content);
        free(current->content);
        current->content = expanded;
        current = current->next;
    }
}

// static t_heredoc_line *create_heredoc_node(char *line)
// {
//     t_heredoc_line *node;
//     node = Malloc(sizeof(t_heredoc_line));
    
//     node->content = ft_strdup(line);
//     if (!node->content)  // ← strdup kontrolü
//     {
//         free(node);
//         return (NULL);
//     }
//     node->next = NULL;
//     return node;
// }

/**
 * @brief Çocuk prosesin görevi: Kullanıcıdan girdi okuyup pipe'a yazmak.
 */
static void heredoc_child_routine(int pipe_write_fd, char *delimiter)
{
    char *line;

    // Çocuğun sinyal yöneticileri varsayılan (default) olmalı ki Ctrl+C ile sonlanabilsin.
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);

    while (1)
    {
        line = readline("> ");
        if (!line || ft_strcmp(line, delimiter) == 0)
        {
            if (line)
                free(line);
            break;
        }
        // Okunan satırı ve bir newline karakterini pipe'a yaz.
        write(pipe_write_fd, line, ft_strlen(line));
        write(pipe_write_fd, "\n", 1);
        free(line);
    }
    close(pipe_write_fd);
    exit(0); // Başarıyla tamamlandı.
}

static void read_from_pipe_and_fill_list(int pipe_read_fd, t_command *cmd)
{
    char    buffer[4097];
    ssize_t bytes_read;
    char    *full_content = ft_strdup("");
    char    *temp;
    char    **lines;
    int     i = 0;

    // Pipe'taki tüm içeriği tek bir string'e topla
    while ((bytes_read = read(pipe_read_fd, buffer, 4096)) > 0)
    {
        buffer[bytes_read] = '\0';
        temp = ft_strjoin(full_content, buffer);
        free(full_content);
        full_content = temp;
    }
    close(pipe_read_fd);

    // Toplanan string'i satırlara ayır ve bağlı listeye ekle
    lines = ft_split(full_content, '\n');
    free(full_content);
    if (!lines)
        return;
    while(lines[i])
    {
        // create_heredoc_node gibi bir fonksiyonunuz olmalı
        t_heredoc_line *new_node = Malloc(sizeof(t_heredoc_line));
        new_node->content = ft_strdup(lines[i]);
        new_node->next = NULL;
        
        // Add to list logic... (Bu kısmı kendi listenize göre uyarlayın)
        if (cmd->heredoc_lines == NULL)
            cmd->heredoc_lines = new_node;
        else
        {
            t_heredoc_line *current = cmd->heredoc_lines;
            while(current->next)
                current = current->next;
            current->next = new_node;
        }
        free(lines[i]);
        i++;
    }
    free(lines);
}

/**
 * @brief Heredoc işlemini fork ve pipe kullanarak yöneten ana fonksiyon.
 * @return 0 başarılı, -1 Ctrl+C ile kesildi.
 */
/**
 * @brief Heredoc işlemini yöneten ana fonksiyon.
 */
int handle_heredoc(t_data *data, t_command *cmd)
{
    int     pipefd[2];
    int     status;
    pid_t   pid;

    if (!cmd->heredoc_delimiter)
        return (0);
    if (pipe(pipefd) == -1)
        return (perror("pipe"), -1);

    signal(SIGINT, SIG_IGN);
    pid = fork();
    if (pid == -1)
        return (perror("fork"), -1);
    if (pid == 0)
        heredoc_child_routine(pipefd[1], cmd->heredoc_delimiter);

    close(pipefd[1]);
    waitpid(pid, &status, 0);
    read_from_pipe_and_fill_list(pipefd[0], cmd);

    signal(SIGINT, signal_handler);

    if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
    {
        write(STDOUT_FILENO, "\n", 1);
        data->exit_value = 130;
        cmd->heredoc_lines = NULL; // Bellek sızıntısı olmaması için listeyi temizle
        return (-1);
    }

    // --- EN ÖNEMLİ DEĞİŞİKLİK VE ÇÖZÜM BURADA ---
    // Eğer delimiter tırnak içinde DEĞİLSE (`should_expand_heredoc` 1 ise) genişlet.
    if (cmd->should_expand_heredoc)
    {
        expand_heredoc_lines(data, cmd);
    }
    // --- DEĞİŞİKLİK SONU ---
    return (0);
}
