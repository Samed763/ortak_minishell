#include "minishell.h"

static const char	*get_token_name(int token)
{
    if (token == TOKEN_WORD)
        return ("WORD");
    else if (token == TOKEN_PIPE)
        return ("PIPE");
    else if (token == TOKEN_REDIRECT_IN)
        return ("REDIRECT_IN");
    else if (token == TOKEN_REDIRECT_OUT)
        return ("REDIRECT_OUT");
    else if (token == TOKEN_APPEND)
        return ("APPEND");
    else if (token == TOKEN_HEREDOC)
        return ("HEREDOC");
    else
        return ("UNKNOWN");
}

void	print_tokens(char **word_array, int *tokens)
{
    int	i;

    if (!word_array || !tokens)
        return ;
    i = 0;
    printf("\n=== TOKEN ANALYSIS ===\n");
    while (word_array[i] && tokens[i] != -1)
    {
        printf("[%d] %-15s -> %s\n", i, word_array[i], 
            get_token_name(tokens[i]));
        i++;
    }
    printf("======================\n\n");
}

int	get_token_type(const char *str)
{
    if (!str)
        return (TOKEN_WORD);
    if (ft_strcmp(str, "|") == 0)
        return (TOKEN_PIPE);
    else if (ft_strcmp(str, "<") == 0)
        return (TOKEN_REDIRECT_IN);
    else if (ft_strcmp(str, ">") == 0)
        return (TOKEN_REDIRECT_OUT);
    else if (ft_strcmp(str, ">>") == 0)
        return (TOKEN_APPEND);
    else if (ft_strcmp(str, "<<") == 0)
        return (TOKEN_HEREDOC);
    return (TOKEN_WORD);
}

int	*tokenize_words(char **word_array)
{
    int	i;
    int	word_count;
    int	*tokens;

    i = 0;
    word_count = 0;
    while (word_array && word_array[word_count])
        word_count++;
    tokens = malloc(sizeof(int) * (word_count + 1));
    if (!tokens)
        return (NULL);
    i = 0;
    while (i < word_count)
    {
        tokens[i] = get_token_type(word_array[i]);
        i++;
    }
    tokens[word_count] = -1;
    return (tokens);
}