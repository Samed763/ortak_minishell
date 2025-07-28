#include "../minishell.h"

/**
 * @brief Bir kelime içindeki tüm dış tırnak katmanlarını kaldırır.
 * Örnek: "hello"'world' -> helloworld
 * @param str Temizlenecek kelime.
 * @return Tırnaklardan arındırılmış yeni bir string.
 */
static char *remove_quotes_from_word(char *str)
{
    char *new_str;
    int i;
    int j;
    char quote_char;

    if (!str)
        return (NULL);
    new_str = (char *)malloc(ft_strlen(str) + 1);
    if (!new_str)
        return (NULL);
    i = 0;
    j = 0;
    while (str[i])
    {
        // Bir tırnak karakteriyle karşılaşırsak
        if (str[i] == '\'' || str[i] == '"')
        {
            quote_char = str[i];
            i++; // Açılış tırnağını atla
            // Eşleşen kapanış tırnağını bulana kadar kopyala
            while (str[i] && str[i] != quote_char)
            {
                new_str[j++] = str[i++];
            }
            if (str[i] == quote_char)
                i++; // Kapanış tırnağını atla
        }
        else // Tırnak değilse karakteri direkt kopyala
        {
            new_str[j++] = str[i++];
        }
    }
    new_str[j] = '\0';
    return (new_str);
}

static void remove_quotes_parser_helper(const char *str, char **delimiter, t_command *current)
{
    size_t len;

    if (!str)
        return ;
    len = ft_strlen(str);
    if (len >= 2 && ((str[0] == '"' && str[len - 1] == '"') ||
                     (str[0] == '\'' && str[len - 1] == '\'')))
    {
        // ft_substr kullanarak tırnakları atla
        *delimiter = ft_substr(str, 1, len - 2); // tırnaksız hali
        current->should_expand_heredoc = 0;
        return ;
    }
    *delimiter = ft_strdup(str);

    current->should_expand_heredoc = 1;
    return ;
}

static t_command *create_list(void)
{
    t_command *cmd;

    cmd = malloc(sizeof(t_command));
    cmd->args = NULL;
    cmd->input_files = NULL;
    cmd->output_files = NULL;
    cmd->append_modes = NULL;
    cmd->output_count = 0;
    cmd->heredoc_delimiter = NULL;
    cmd->heredoc_lines = NULL;
    cmd->next = NULL;
    cmd->should_expand_heredoc = 1;
    return (cmd);
}

static void add_argument_to_command(t_command *cmd, char *word)
{
    char **new_args;
    char *cleaned_word; // Temizlenmiş kelime için yeni değişken
    int i = 0;
    int j = 0;

    while (cmd->args != NULL && cmd->args[i])
        i++;
    new_args = Malloc(sizeof(char *) * (i + 2));

    while (cmd->args != NULL && cmd->args[j])
    {
        new_args[j] = cmd->args[j];
        j++;
    }
    if (cmd->args != NULL)
        free(cmd->args);

    // --- DEĞİŞİKLİK BURADA ---
    // Argümanı listeye eklemeden önce tırnaklarını temizle.
    cleaned_word = remove_quotes_from_word(word);
    new_args[j] = cleaned_word; // Yeni, temizlenmiş string'i ata
    // --- DEĞİŞİKLİK SONU ---

    new_args[j + 1] = NULL;
    cmd->args = new_args;
}


static void add_input_to_command(t_command *current, char *filename)
{
    if (current->input_files)
        free(current->input_files);
    current->input_files = ft_strdup(filename);
}

static void add_output_to_command(t_command *current, char *filename, int append_mode)
{
    char **new_output_files;
    int *new_append_modes;
    int i;

    new_output_files = malloc(sizeof(char *) * (current->output_count + 1));
    new_append_modes = malloc(sizeof(int) * (current->output_count + 1));

    i = 0;
    while (i < current->output_count)
    {
        new_output_files[i] = current->output_files[i];
        new_append_modes[i] = current->append_modes[i];
        i++;
    }
    new_output_files[current->output_count] = ft_strdup(filename);
    new_append_modes[current->output_count] = append_mode;
    if (current->output_files)
        free(current->output_files);
    if (current->append_modes)
        free(current->append_modes);
    current->output_files = new_output_files;
    current->append_modes = new_append_modes;
    current->output_count++;
}

static int handle_redirections(t_data *data, t_command *current, int *i)
{
    if (data->token[*i] == TOKEN_REDIRECT_IN)
    {
        (*i)++;                   // ✅ Önce artır
        if (data->word_array[*i]) // ✅ Sonra kontrol et
            add_input_to_command(current, data->word_array[*i]);
    }
    else if (data->token[*i] == TOKEN_REDIRECT_OUT)
    {
        (*i)++;                   // ✅ Önce artır
        if (data->word_array[*i]) // ✅ Sonra kontrol et
            add_output_to_command(current, data->word_array[*i], 0);
    }
    else if (data->token[*i] == TOKEN_APPEND)
    {
        (*i)++;                   // ✅ Önce artır
        if (data->word_array[*i]) // ✅ Sonra kontrol et
            add_output_to_command(current, data->word_array[*i], 1);
    }
    else if (data->token[*i] == TOKEN_HEREDOC)
    {
        (*i)++;
        if (data->word_array[*i])
        {
            remove_quotes_parser_helper(data->word_array[*i], &current->heredoc_delimiter, current);
            // handle_heredoc'un dönüş değerini kontrol et
            if (handle_heredoc(data, current) == -1)
            {
                return (-1);
            }
        }
    }
    return (0);
}

t_command *parser(t_data *data)
{
    t_command *head;
    t_command *current;

    head = create_list();
    current = head;
    int i = 0;
    while (data->word_array[i])
    {
        if (data->token[i] == TOKEN_WORD)
            add_argument_to_command(current, data->word_array[i]);
        else if (data->token[i] == TOKEN_PIPE)
        {
            current->next = create_list();
            current = current->next;
        }
        else if (handle_redirections(data, current, &i) == -1)
        {
            // Heredoc iptal edildi, parsing işlemine devam etme.
            free_command_list(head); // Oluşturulan listeyi temizle.
            return (NULL); // veya head (main'de kontrol edilecek)
        }
        i++;
    }
    return head;
}
