#include "minishell.h"

char *put_var(char *line, char *var_value, int key_start, int key_end)
{
    char *start = ft_substr(line, 0, key_start);
    char *end = ft_substr(line, key_end, ft_strlen(line));
    char *temp;
    char *ret_val;
    char *safe_value;
    
    // NULL value kontrolü - güvenli kopyalama
    if (!var_value)
        safe_value = ft_strdup("");
    else
        safe_value = ft_strdup(var_value);
    
    // Memory leak önleme
    temp = ft_strjoin(safe_value, end);
    ret_val = ft_strjoin(start, temp);
    
    // Memory temizleme
    free(start);
    free(end);
    free(temp);
    free(safe_value);
    free(line); // Eski string'i free et
    
    return ret_val;
}
int is_identifier_char(int c)
{
    return (c >= 'A' && c <= 'Z') ||
                    (c >= 'a' && c <= 'z') ||
                    (c >= '0' && c <= '9') ||
                    (c == '_');
}

char *expand_single_line(t_data *data, char *line)
{
    char *result = ft_strdup(line);
    int s_quotes = 0;
    size_t j = 0;

    while (result[j])
    {
        if (result[j] == '\'')
            s_quotes = !s_quotes;

        if (result[j] == '$' && s_quotes == 0)
        {
            char *key;
            char *value;
            int k = j + 1;

            // --- YENİ EKLENEN BLOK BAŞLANGICI ---
            if (result[k] == '?')
            {
                key = ft_strdup("?");
                k++; // '?' karakterini atla
            }
            // --- YENİ EKLENEN BLOK SONU ---
            else // Eski mantık
            {
                while (result[k] && is_identifier_char(result[k]))
                    k++;
                key = ft_substr(result, j + 1, k - j - 1);
            }
            
            value = find_value_by_key(data, key);
            result = put_var(result, value, j, k);
            
            free(key);
            if(value) // find_value_by_key'den dönen bellek sızıntısını önle
                free(value);

            j = -1; // Döngüyü baştan başlatmak için j'yi sıfırla (-1 + 1 = 0)
        }
        j++;
    }
    return result;
}
// Word array için variable expansion
void set_var(t_data *data)
{
    for (size_t i = 0; data->word_array[i]; i++)
    {
        char *expanded = expand_single_line(data,data->word_array[i]);
        free(data->word_array[i]);
        data->word_array[i] = expanded;
    }
}
void expander(t_data *data)
{
    int i = 0;
    char *old_word;
    char *expanded;

    while (data->word_array[i])
    {
        // --- GEREKSİZ IF BLOĞU KALDIRILDI ---
        
        // Önce çift tırnakları (varsa) kaldır
        old_word = data->word_array[i];
        data->word_array[i] = remove_d_quotes(data->word_array[i]);
        free(old_word);
        
        // Heredoc delimiter'ları hariç her kelimeyi genişletmeyi dene.
        // Tek tırnak kontrolünü expand_single_line zaten yapıyor.
        if (i > 0 && data->token[i-1] != TOKEN_HEREDOC)
        {
            expanded = expand_single_line(data, data->word_array[i]);
            free(data->word_array[i]);
            data->word_array[i] = expanded;
        }
        i++;
    }
}
