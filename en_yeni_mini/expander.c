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
char *expand_single_line(char *line, t_data *data)
{
    char *result = ft_strdup(line);
    int s_quotes = 0;
    
    for (size_t j = 0; result[j]; j++)
    {
        if (result[j] == '\'')
            s_quotes = !s_quotes;
        if (result[j] == '$' && s_quotes == 0)
        {
            int k = j + 1;
            while (result[k] && 
                   ((result[k] >= 'A' && result[k] <= 'Z') ||
                    (result[k] >= 'a' && result[k] <= 'z') ||
                    (result[k] >= '0' && result[k] <= '9') ||
                    result[k] == '_'))
                k++;
            
            char *key = ft_substr(result, j + 1, k - j - 1);
            char *value = find_value_by_key(data, key);
            
            if (!value)
                value = ft_strdup("");
            
            result = put_var(result, value, j, k);
            free(key);
            free(value);
            
            j = 0; // Reset j
        }
    }
    
    return result;
}

// Word array için variable expansion
void set_var(t_data *data)
{
    for (size_t i = 0; data->word_array[i]; i++)
    {
        char *expanded = expand_single_line(data->word_array[i], data);
        free(data->word_array[i]);
        data->word_array[i] = expanded;
    }
}
void expander(t_data *data)
{
    int i = 0;
    char *old_word;
    
    // Çift tırnakları kaldır
    while (data->word_array[i])
    {
        old_word = data->word_array[i];
        data->word_array[i] = remove_d_quotes(data->word_array[i]);
        free(old_word); // Eski string'i free et
        i++;
    }
    
    // Variable expansion
    set_var(data);
}