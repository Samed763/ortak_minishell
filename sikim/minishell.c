#include "minishell.h"

int ft_isalpha(int c)
{
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

int ft_isalnum(int c)
{
    return (ft_isalpha(c) || (c >= '0' && c <= '9'));
}

// Komut başında kaç adet değişken tanımı olduğunu bulur
int count_variable_assignments(char **word_array)
{
    int i = 0;
    
    if (!word_array)
        return (0);
    
    while (word_array[i])
    {
        if (ft_strchr(word_array[i], '='))
        {
            char *equal_pos = ft_strchr(word_array[i], '=');
            int var_name_len = equal_pos - word_array[i];
            
            if (var_name_len > 0)
            {
                int j = 0;
                int valid_var = 1;
                
                // İlk karakter harf veya _ olmalı
                if (!ft_isalpha(word_array[i][0]) && word_array[i][0] != '_')
                    valid_var = 0;
                
                // Diğer karakterler harf, rakam veya _ olmalı
                while (j < var_name_len && valid_var)
                {
                    if (!ft_isalnum(word_array[i][j]) && word_array[i][j] != '_')
                        valid_var = 0;
                    j++;
                }
                
                if (valid_var)
                    i++;
                else
                    break;
            }
            else
                break;
        }
        else
            break;
    }
    
    return (i);
}

// Değişkenleri shell değişken tablosuna ekler
void add_to_shell_variables(char **word_array, int var_count)
{
    int i = 0;
    
    while (i < var_count)
    {
        printf("Adding shell variable: %s\n", word_array[i]);
        // TODO: Shell değişken tablosuna ekle
        i++;
    }
}

// Geçici ortam değişkenleri ile komut çalıştırır
void execute_with_temp_env(char **word_array, int var_count)
{
    printf("Executing command with temp env:\n");
    
    // Değişkenleri göster
    int i = 0;
    while (i < var_count)
    {
        printf("Temp var: %s\n", word_array[i]);
        i++;
    }
    
    // Komutu göster
    printf("Command: %s", word_array[var_count]);
    i = var_count + 1;
    while (word_array[i])
    {
        printf(" %s", word_array[i]);
        i++;
    }
    printf("\n");
    
    // TODO: Geçici ortam ile komut çalıştır
}

// Ana değişken işleme fonksiyonu
int process_variables_and_commands(t_data *data)
{
    int var_count = count_variable_assignments(data->word_array);
    
    if (var_count == 0)
    {
        // Hiç değişken tanımı yok, normal komut işleme
        return (0);
    }
    
    // Değişken tanımları var
    if (data->word_array[var_count] == NULL)
    {
        // Sadece değişken tanımları var, komut yok
        // Shell değişken tablosuna ekle
        add_to_shell_variables(data->word_array, var_count);
        return (1);
    }
    else
    {
        // Değişken tanımları + komut var
        // Geçici ortam ile komut çalıştır
        execute_with_temp_env(data->word_array, var_count);
        return (1);
    }
}

int any_variable(t_data *data)
{
    return (count_variable_assignments(data->word_array) > 0);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    char *input;
    t_data data;
    
    while (1)
    {
        input = readline("minishell$ ");
        
        // Check for exit condition
        if (!input || ft_strcmp(input, "exit") == 0)
        {
            if (input)
                free(input);
            break;
        }
        
        // Skip empty input
        if (ft_strlen(input) == 0)
        {
            free(input);
            continue;
        }
        
        // Add to history
        add_history(input);
        
        // Process input
        data.word_array = split_by_quote(input);
        
        // Değişken ve komut işleme
        if (process_variables_and_commands(&data))
        {
            // Değişken işleme yapıldı, normal komut işleme atla
        }
        else
        {
            // Normal komut işleme
            data.token = tokenize_words(data.word_array);
            data.commands = parse_commands(data.word_array, data.token);
            print_word_array(data.word_array);
            execute_commands(data.commands);
            free_commands(data.commands);
            free_token_array(data.token);
        }
        
        // Clean up
        //free_word_array(data.word_array);
        free(input);
    }
    
    return 0;
}