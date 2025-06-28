#include "minishell.h"

static int lexer_word_count(char *input)
{
    int d_quote = 0;
    int s_quote = 0;
    int i = 0;
    int word_count = 0;
    int in_word = 0;

    while (input[i])
    {
        if (input[i] == '\'' && !d_quote)
            s_quote = !s_quote;
        else if (input[i] == '"' && !s_quote)
            d_quote = !d_quote;

        if (input[i] != ' ' || s_quote || d_quote)
        {
            if (!in_word)
            {
                word_count++;
                in_word = 1;
            }
        }
        else
            in_word = 0;
        i++;
    }
    return word_count;
}

char **lexer(char *input)
{
    int d_quote = 0;
    int s_quote = 0;
    int j = 0;
    int i = 0;
    int word = 0;
    char **args;
    char *trimmed_input;

    trimmed_input = ft_strtrim(input, " ");
    args = Malloc(sizeof(char *) * (lexer_word_count(trimmed_input) + 1));

    while (trimmed_input[i])
    {
        if (trimmed_input[i] == '\'' && !d_quote)
            s_quote = !s_quote;
        else if (trimmed_input[i] == '"' && !s_quote)
            d_quote = !d_quote;

        if (trimmed_input[i] == ' ' && !d_quote && !s_quote)
        {
            if (i > j)  // Token varsa
            {
                args[word] = ft_substr(trimmed_input, j, i - j);
                word++;
            }
            // Boşlukları atla
            while (trimmed_input[i] && trimmed_input[i] == ' ')
                i++;
            j = i;  // Yeni token başlangıcı
            continue;
        }
        i++;
    }
    
    // Son token'ı ekle
    if (i > j)
    {
        args[word] = ft_substr(trimmed_input, j, i - j);
        word++;
    }

    args[word] = NULL;
    free(trimmed_input);  // Memory leak'i önle
    return args;
}