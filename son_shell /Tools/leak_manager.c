#include "../minishell.h"

void free_heredoc_lines(t_heredoc_line *head)
{
    t_heredoc_line *tmp;

    while (head)
    {
        tmp = head->next;
        free(head->content);
        free(head);
        head = tmp;
    }
}

void free_command_list(t_command *head)
{
    t_command *tmp;

    while (head)
    {
        tmp = head->next;
        // Argümanları temizle (args)
        free_word_array(head->args); // Bu fonksiyon zaten Tools/utils.c'de var
        
        // Yönlendirme dosyalarını temizle
        if (head->input_files)
            free(head->input_files);
        if (head->output_files)
            free_word_array(head->output_files); // output_files bir array
        if (head->append_modes)
            free(head->append_modes); // append_modes bir array

        // Heredoc delimiter ve satırlarını temizle
        if (head->heredoc_delimiter)
            free(head->heredoc_delimiter);
        if (head->heredoc_lines)
            free_heredoc_lines(head->heredoc_lines); // Heredoc için ayrı bir temizlik fonksiyonu

        // Komut yapısının kendisini temizle
        free(head);
        head = tmp;
    }
}

void free_data_resources(t_data *data)
{
    if (!data)
        return;
    
    // Lexer'dan gelen word_array ve token array'ini temizle
    if (data->word_array)
        free_word_array(data->word_array);
    data->word_array = NULL;

    if (data->token)
        free(data->token);
    data->token = NULL;

    // Parser'dan gelen komut listesini temizle
    if (data->cmd)
        free_command_list(data->cmd);
    data->cmd = NULL;
}