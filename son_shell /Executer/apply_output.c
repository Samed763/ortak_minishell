// DOSYA: apply_output.c

#include "../minishell.h"


// Bu fonksiyon da artık hata döndürebilir.
static int create_empty_output_files(t_command *cmd)
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
        
        if (fd == -1)
        {
            perror(cmd->output_files[i]); // Hata mesajını bas
            return (-1); // HATA BİLDİR
        }
        close(fd);
        i++;
    }
    return (0); // BAŞARILI
}

// Ana fonksiyonu, hata kontrolü yapacak ve hata döndürecek şekilde güncelliyoruz.
int apply_output_redirection(t_command *cmd)
{
    int fd;
    char *last_file;

    if (!cmd || !cmd->output_files || cmd->output_count == 0)
        return (0);
    
    if (create_empty_output_files(cmd) == -1)
        return (-1); // Alt fonksiyondan hata geldiyse devam etme

    last_file = cmd->output_files[cmd->output_count - 1];
    if (cmd->append_modes[cmd->output_count - 1])
        fd = open(last_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    else
        fd = open(last_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    
    if (fd == -1)
    {
        perror(last_file);
        return (-1); // HATA BİLDİR
    }
    
    if (dup2(fd, STDOUT_FILENO) == -1)
    {
        perror("dup2");
        close(fd);
        return (-1); // HATA BİLDİR
    }
    
    close(fd);
    return (0); // BAŞARILI
}