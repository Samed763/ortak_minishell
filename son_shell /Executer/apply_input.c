#include "../minishell.h"


static void heredoc_child_process(int *pipefd, t_command *cmd)
{
    t_heredoc_line *current_line;
    
    close(pipefd[0]);
    current_line = cmd->heredoc_lines;
    while (current_line)
    {
        write(pipefd[1], current_line->content, ft_strlen(current_line->content));
        write(pipefd[1], "\n", 1);
        current_line = current_line->next;
    }
    close(pipefd[1]);
    exit(0);
}

// Parent process olduğu için exit'leri kaldırıp hata döndürüyoruz.
static int heredoc_parent_process(int *pipefd)
{
    close(pipefd[1]);
    if (dup2(pipefd[0], STDIN_FILENO) == -1)
    {
        perror("dup2 heredoc");
        close(pipefd[0]);
        return (-1); // HATA BİLDİR
    }
    close(pipefd[0]);
    wait(NULL);
    return (0); // BAŞARILI
}

// Fonksiyonu hata döndürecek şekilde güncelliyoruz.
static int input_heredoc(t_command *cmd)
{
    pid_t pid;
    int pipefd[2];
    
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return (-1);
    }
    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        return (-1);
    }
    else if (pid == 0)
        heredoc_child_process(pipefd, cmd);
    else
        return (heredoc_parent_process(pipefd)); // Parent'ın sonucunu döndür
    return (0);
}

// exit'leri kaldırıp hata döndürüyoruz.
static int input_file(t_command *cmd)
{
    int fd;

    fd = open(cmd->input_files, O_RDONLY);
    if (fd == -1)
    {
        perror(cmd->input_files); // Hata mesajında dosya adını göster
        return (-1); // HATA BİLDİR
    }
    if (dup2(fd, STDIN_FILENO) == -1)
    {
        perror("dup2");
        close(fd);
        return (-1); // HATA BİLDİR
    }
    close(fd);
    return (0); // BAŞARILI
}

// Ana fonksiyonu, alt fonksiyonlardan gelen hataları döndürecek şekilde güncelliyoruz.
int apply_input_redirection(t_command *cmd)
{
    if(cmd->heredoc_delimiter)
    {
        if (cmd->heredoc_lines != NULL)
            return (input_heredoc(cmd));
        else
        {
            int fd = open("/dev/null", O_RDONLY);
            if (fd == -1)
            {
                perror("/dev/null");
                return (-1);
            }
            if (dup2(fd, STDIN_FILENO) == -1)
            {
                 perror("dup2");
                 close(fd);
                 return (-1);
            }
            close(fd);
        }
    }
    else if(cmd->input_files != NULL)
        return (input_file(cmd));
    return (0); // Yönlendirme yoksa veya başarılıysa
}