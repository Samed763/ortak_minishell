#include "minishell.h"
#include <sys/ioctl.h>

volatile sig_atomic_t g_heredoc_interrupted = 0;

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
    (void)argc;
    (void)argv;

    // --- YENİ EKLENEN SATIR ---
    // Program başlarken terminalin "sağlıklı" ayarlarını global değişkene kaydet.
    

    // ... main fonksiyonunuzun geri kalanı tamamen aynı kalabilir ...
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction for SIGINT");
        exit(1);
    }
    
    // SIGQUIT (Ctrl-\) sinyalini görmezden gel (ignore)
    sa.sa_handler = SIG_IGN;
    if (sigaction(SIGQUIT, &sa, NULL) == -1)
    {
        perror("sigaction for SIGQUIT");
        exit(1);
    }

    // YENİ: Ortam değişkenleri kopyalanamazsa, bu kritik bir hatadır.
    data.env = copy_env(envp);
    if (!data.env)
    {
        perror("Failed to copy environment variables");
        exit(1);
    }
    data.exit_value = 0;
    data.cmd = NULL;
    data.word_array = NULL;
    data.token = NULL;
    data.splitted_path = NULL;
    while (1)
    {
        line = readline("minishell$ ");

        if (!line)
        {
            // Ctrl-D (EOF) durumu, shell'den çık. [cite: 117]
            printf("exit\n");
            cleanup_and_exit(&data, data.exit_value);
        }
        if (line && *line)
            add_history(line);

        if (syntax_check(line))
        {
            printf("syntax error\n");
            free(line);
            continue;
        }

        lexer(line, &data); //count_word kısaltılcak onun dışında oke
        expander(&data); // kısaltılcak onun dışında oke

        
        data.cmd = parser(&data); // kısaltılcak onun dışında oke

        if (data.cmd != NULL && !g_heredoc_interrupted)
        {
            // Eğer heredoc iptal edildiyse, komutu çalıştırma.
                execute_commmand(&data);
        }

        //free_data_resources(&data);
        free(line);                // readline ile ayrılan satırı temizle
        g_heredoc_interrupted = 0; // Global bayrağı sıfırla
    }
    cleanup_and_exit(&data, data.exit_value);
    return (data.exit_value);
}
