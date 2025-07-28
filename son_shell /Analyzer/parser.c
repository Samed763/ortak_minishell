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

    if (!str || !delimiter || !current)
        return;
    len = ft_strlen(str);
    if (len >= 2 && ((str[0] == '"' && str[len - 1] == '"') ||
                     (str[0] == '\'' && str[len - 1] == '\'')))
    {
        *delimiter = ft_substr(str, 1, len - 2);
        current->should_expand_heredoc = 0;
    }
    else
    {
        *delimiter = ft_strdup(str);
        current->should_expand_heredoc = 1;
    }
    if (!*delimiter)
        *delimiter = ft_strdup("");
}

static t_command *create_list(void)
{
    t_command *cmd;

    cmd = malloc(sizeof(t_command));
    if (!cmd)
        return (NULL);
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
    char *cleaned_word;
    int old_argc = 0;
    int i = 0;

    // Argümanların geçerli olup olmadığını kontrol et.
    if (!cmd || !word)
        return;

    // 1. ADIM (GÜVENLİK): Yeni kelimeyi *önceden* hazırla.
    // Eğer bu adım başarısız olursa, orijinal `cmd->args`'a hiç dokunmamış oluruz.
    cleaned_word = remove_quotes_from_word(word);
    if (!cleaned_word)
        return;
    // 2. ADIM: Mevcut argüman sayısını bul.
    while (cmd->args && cmd->args[old_argc])
        old_argc++;

    // 3. ADIM: Yeni argüman dizisi için bellek ayır.
    // Senin Malloc'un çıktığı için bu kontrolü standart malloc'a göre yapıyorum.
    new_args = malloc(sizeof(char *) * (old_argc + 2));
    if (!new_args)
    {
        // Bellek ayrılamadı. 1. adımda ayırdığımız cleaned_word'ü serbest bırakmalıyız.
        free(cleaned_word);
        return;
    }

    // 4. ADIM: Eski argüman işaretçilerini yeni diziye kopyala.
    while (i < old_argc)
    {
        new_args[i] = cmd->args[i];
        i++;
    }

    // 5. ADIM: Yeni temizlenmiş argümanı ve NULL sonlandırıcıyı ekle.
    new_args[i] = cleaned_word;
    new_args[i + 1] = NULL;

    // 6. ADIM: Eski argüman dizisini (sadece işaretçileri tutan diziyi) serbest bırak.
    if (cmd->args)
        free(cmd->args);

    // 7. ADIM: Komutun argümanlarını yeni diziyle güncelle.
    cmd->args = new_args;
}

static void add_input_to_command(t_command *current, char *filename)
{
    char *new_input_file;

    if (!current || !filename)
        return;

    new_input_file = ft_strdup(filename);
    if (!new_input_file)
        return; // Hata durumunda çık
    if (current->input_files)
        free(current->input_files);
    current->input_files = new_input_file;
}

static void add_output_to_command(t_command *current, char *filename, int append_mode)
{
    char **new_output_files;
    int *new_append_modes;
    int i;

    if (!current || !filename)
        return;
    new_output_files = malloc(sizeof(char *) * (current->output_count + 1));
    if (!new_output_files)
        return;
    new_append_modes = malloc(sizeof(int) * (current->output_count + 1));
    if (!new_append_modes)
    {
        free(new_output_files);
        return;
    }
    i = 0;
    while (i < current->output_count)
    {
        new_output_files[i] = current->output_files[i];
        new_append_modes[i] = current->append_modes[i];
        i++;
    }
    new_output_files[current->output_count] = ft_strdup(filename);
    if (!new_output_files[current->output_count])
    {
        free(new_output_files);
        free(new_append_modes);
        return;
    }
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
    int token_type = data->token[*i];
    (*i)++; // İndeksi bir sonraki kelime/token için ilerlet.

    if (!data->word_array[*i])
        return (-1);
    if (token_type == TOKEN_REDIRECT_IN)
        add_input_to_command(current, data->word_array[*i]);
    else if (token_type == TOKEN_REDIRECT_OUT)
        add_output_to_command(current, data->word_array[*i], 0);
    else if (token_type == TOKEN_APPEND)
        add_output_to_command(current, data->word_array[*i], 1);
    else if (token_type == TOKEN_HEREDOC)
    {
        remove_quotes_parser_helper(data->word_array[*i], &current->heredoc_delimiter, current);
        if (handle_heredoc(data, current) == -1)
            return (-1);
    }
    return (0);
}

t_command *parser(t_data *data)
{
    t_command *head;
    t_command *current;
    int i = 0;

    if (!data || !data->word_array)
        return (NULL);
    head = create_list();
    if (!head)
        return (NULL);
    current = head;
    while (data->word_array[i])
    {
        if (data->token[i] == TOKEN_WORD)
            add_argument_to_command(current, data->word_array[i]);
        else if (data->token[i] == TOKEN_PIPE)
        {
            current->next = create_list();
            if (!current->next)
            {
                free_command_list(head);
                return (NULL);
            }
            current = current->next;
        }
        else
        {
            if (handle_redirections(data, current, &i) == -1)
            {
                free_command_list(head);
                return (NULL);
            }
        }
        i++;
    }
    return head;
}