#include "minishell.h"

void free_heredoc_lines(t_heredoc_line *head)
{
    t_heredoc_line *current = head;
    t_heredoc_line *next;

    while (current)
    {
        next = current->next;
        free(current->content);
        free(current);
        current = next;
    }
}
void free_commands(t_command *cmd)
{
    t_command *current = cmd;
    t_command *next;

    while (current)
    {
        next = current->next;

        // Free args
        if (current->args)
            free_word_array(current->args);

        // Free input file
        if (current->input_files)
            free(current->input_files);

        // Free output files
        if (current->output_files)
        {
            int i = 0;
            while (i < current->output_count)
            {
                free(current->output_files[i]);
                i++;
            }
            free(current->output_files);
        }

        // Free append modes
        if (current->append_modes)
            free(current->append_modes);

        // Free heredoc delimiter
        if (current->heredoc_delimiter)
            free(current->heredoc_delimiter);

        // Free heredoc lines
        free_heredoc_lines(current->heredoc_lines);

        free(current);
        current = next;
    }
}

void signal_handler(int signum)
{
    if (signum == SIGINT)
    {
        // Yeni bir satır yazdır
        write(1, "\n", 1);
        // Readline'a yeni bir satırda olduğumuzu söyle bunun altına sanırım ihtiyacım yok
        rl_on_new_line();
        // Mevcut satır içeriğini temizle (isteğe bağlı ama güzel bir özellik)
        rl_replace_line("", 0);
        // Komut istemini tekrar göster
        rl_redisplay();
    }
    // SIGQUIT (Ctrl-\) için bir şey yapmıyoruz, çünkü interaktif modda
    // görmezden gelinmesini istiyoruz. Bu yüzden burada bir 'else if' bloğu yok.
}

// main fonksiyonunu güncelle
int main(int argc, char **argv, char **envp)
{
    char *line;
    t_data data;
    struct sigaction sa;

    sa.sa_handler = signal_handler; // Bizim yazdığımız fonksiyonu ata
    sigemptyset(&sa.sa_mask);       // Handler çalışırken başka sinyalleri bloklama
    sa.sa_flags = SA_RESTART;       // Sistem çağrılarının handler'dan sonra devam etmesini sağla

    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    // SIGQUIT (Ctrl-\) sinyalini görmezden gel (ignore)
    sa.sa_handler = SIG_IGN; // SIG_IGN: Ignore Signal
    if (sigaction(SIGQUIT, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    data.env = copy_env(envp);
    data.exit_value = 0;
    data.cmd = NULL;
    while (1)
    {
        line = readline("minishell$ ");

        if (!line)
        {
            return 0;
        }
        if (line && *line)
        {
            add_history(line);
        }

        if (syntax_check(line))
        {
            printf("syntax error\n");
            free(line);
            continue;
        }

        lexer(line, &data);

        expander(&data);

        data.cmd = parser(&data);
        execute_commmand(&data);
    }

    return 0;
}
