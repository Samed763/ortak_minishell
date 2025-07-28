#include "../minishell.h"
// Bu fonksiyonları daha önce oluşturmuştuk, burada tekrar kullanacağız.
void free_heredoc_lines(t_heredoc_line *head);
void free_command_list(t_command *head);
void free_data_resources(t_data *data);
int builtin_exit(t_data *data);

/**
 * @brief Tüm ayrılmış kaynakları temizler ve programı sonlandırır.
 * @param data Ana veri yapısı.
 * @param exit_code Programın döneceği çıkış kodu.
 */
void    cleanup_and_exit(t_data *data, int exit_code)
{
    if (data)
    {
        // 1. Döngü içi ayrılan bellek (cmd, word_array, token)
        free_data_resources(data);

        // 2. PATH için ayrılan bellek
        if (data->splitted_path)
            free_word_array(data->splitted_path);
        data->splitted_path = NULL;

        // 3. Ortam değişkenleri için ayrılan bellek
        if (data->env)
            free_word_array(data->env);
        data->env = NULL;
    }

    // 4. Readline geçmişini temizle
    clear_history();

    // 5. Programı sonlandır
    exit(exit_code);
}
/**
 * @brief 'exit' built-in komutunu çalıştırır.
 * Programdan çıkmadan önce tüm kaynakları temizler.
 * @param data Ana veri yapısı.
 * @return Sadece "too many arguments" hatasında 1 döner, diğer durumlarda programdan çıkar.
 */
int builtin_exit(t_data *data)
{
    int     status;
    char    **args;

    args = data->cmd->args; // Argümanlara artık data yapısı üzerinden ulaşıyoruz
    printf("exit\n");

    // 1. Argüman yoksa: Son komutun çıkış koduyla temiz bir şekilde çık.
    if (!args[1])
    {
        status = data->exit_value;
        cleanup_and_exit(data, status);
    }

    // 2. Argümanın sayısal olup olmadığını kontrol et.
    for (int i = 0; args[1][i]; i++)
    {
        if (i == 0 && (args[1][i] == '-' || args[1][i] == '+'))
            continue;
        if (args[1][i] < '0' || args[1][i] > '9')
        {
            fprintf(stderr, "minishell: exit: %s: numeric argument required\n", args[1]);
            // Sayısal değilse, 255 koduyla temiz bir şekilde çık.
            cleanup_and_exit(data, 255);
        }
    }

    // 3. İkiden fazla argüman varsa: Hata ver ama programdan çıkma.
    if (args[2])
    {
        fprintf(stderr, "minishell: exit: too many arguments\n");
        data->exit_value = 1; // Hata kodunu ayarla
        return (1);           // Programdan çıkma, ana döngüye dön.
    }

    // 4. Tek bir sayısal argüman varsa: O sayıyla temiz bir şekilde çık.
    status = atoi(args[1]);
    cleanup_and_exit(data, status % 256); // bash gibi 0-255 arası bir değere modla

    return (0); // Bu satıra asla ulaşılamaz
}