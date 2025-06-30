#include "minishell.h"

int get_token_type(const char *str)
{
    if (!str)
        return TOKEN_WORD;
    
    if (ft_strcmp(str, "|") == 0)
        return TOKEN_PIPE;
    else if (ft_strcmp(str, "<") == 0)
        return TOKEN_REDIRECT_IN;
    else if (ft_strcmp(str, ">") == 0)
        return TOKEN_REDIRECT_OUT;
    else if (ft_strcmp(str, ">>") == 0)
        return TOKEN_APPEND;
    else if (ft_strcmp(str, "<<") == 0)
        return TOKEN_HEREDOC;
    
    return TOKEN_WORD;
}

void mark_commands(char **word_array, int *token_types, int size)
{
    int i = 0;
    int expect_command = 1;
    
    while (i < size && word_array[i])
    {
        if (expect_command && token_types[i] == TOKEN_WORD)
        {
            token_types[i] = TOKEN_COMMAND;
            expect_command = 0;
        }
        
        if (token_types[i] == TOKEN_PIPE)
            expect_command = 1;
        
        if (token_types[i] == TOKEN_REDIRECT_IN || 
            token_types[i] == TOKEN_REDIRECT_OUT ||
            token_types[i] == TOKEN_APPEND || 
            token_types[i] == TOKEN_HEREDOC)
            expect_command = 0;
            
        i++;
    }
}

int *tokenize_words(char **word_array)
{
    int i = 0;
    int word_count = 0;
    
    while (word_array && word_array[word_count])
        word_count++;
    
    int *tokens = Malloc(sizeof(int) * (word_count + 1));
    if (!tokens)
        return NULL;
    
    for (i = 0; i < word_count; i++)
        tokens[i] = get_token_type(word_array[i]);
    
    tokens[word_count] = -1;
    mark_commands(word_array, tokens, word_count);
    
    return tokens;
}
