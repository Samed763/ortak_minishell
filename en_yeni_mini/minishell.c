#include "minishell.h"



int syntax_check(char *input)
{
    int i = 0;
    
    // Sadece boşluk kontrolü
    while (input[i] == ' ' || input[i] == '\t')
        i++;
    if (input[i] == '\0')
        return 0;
    
    int d_quotes = 0;
    int s_quotes = 0;
    i = 0;

    while (input[i] != '\0')
    {
        // Quote kontrolü - sadece açık quote yokken diğer quote'u kontrol et
        if (input[i] == '\'' && d_quotes == 0)  // çift tırnak içinde değilken
            s_quotes = !s_quotes;
        else if (input[i] == '"' && s_quotes == 0)   // tek tırnak içinde değilken
            d_quotes = !d_quotes;
        
        // Quote içindeyken syntax kontrol etme
        if (s_quotes == 0 && d_quotes == 0)
        {
            // Pipe kontrolü
            if (input[i] == '|')
            {
                if (input[i + 1] == '\0' || input[i + 1] == '|')
                    return 0;
                int j = i + 1;
                while (input[j] == ' ' || input[j] == '\t')
                    j++;
                if (input[j] == '\0' || !is_valid_after_pipe(input[j]))
                    return 0;
            }
            
            // Redirection kontrolü aynı kalabilir
            if (input[i] == '>' || input[i] == '<')
            {
                int j = i;
                if (input[i] == '>' && input[i + 1] == '>')
                    j = i + 2;
                else if (input[i] == '<' && input[i + 1] == '<')
                    j = i + 2;
                else
                    j = i + 1;
                while (input[j] == ' ' || input[j] == '\t')
                    j++; 
                if (input[j] == '\0' || !is_valid_filename_char(input[j]))
                    return 0;
            }
        }
        i++;
    }
    
    // Quote'lar kapalı mı kontrol et
    if (s_quotes || d_quotes)
        return 0;
    
    return 1;
}


int main(int argc, char **argv)
{
    char *input;
    t_data data;

    while (1)
    {
        input = readline("-->");
        
        if (!input)  // Ctrl+D kontrolü
            break;
            
        if (!strcmp(input, "exit"))
        {
            free(input);
            break;
        }
        
        syntax_check(input); //TODO: if koy returna göre işlem yap
        

        free(input);  // Memory leak önleme
    }
    
    return 0;
}
