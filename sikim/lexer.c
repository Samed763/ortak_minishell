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

char **split_by_quote(char *input)
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
            if (i > j)
            {
                args[word] = ft_substr(trimmed_input, j, i - j);
                word++;
            }
            while (trimmed_input[i] && trimmed_input[i] == ' ')
                i++;
            j = i;
            continue;
        }
        i++;
    }
    
    if (i > j)
    {
        args[word] = ft_substr(trimmed_input, j, i - j);
        word++;
    }

    args[word] = NULL;
    free(trimmed_input);
    return args;
}

void print_word_array(char **word_array)
{
    int i = 0;
    
    if (!word_array)
    {
        printf("Array is NULL\n");
        return;
    }
    
    printf("Word array contents:\n");
    while (word_array[i])
    {
        printf("[%d]: %s\n", i, word_array[i]);
        i++;
    }
    printf("Total words: %d\n", i);
}
