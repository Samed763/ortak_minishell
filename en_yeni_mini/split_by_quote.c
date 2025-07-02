#include "minishell.h"

typedef struct s_lwc
{
    int	d_quote;
    int	s_quote;
    int	i;
    int	word_count;
    int	in_word;
}t_lwc;


static int	lexer_word_count(char *input)
{
    t_lwc lwc_data;

    lwc_data.d_quote = 0;
    lwc_data.s_quote = 0;
    lwc_data.i = -1;
    lwc_data.word_count = 0;
    lwc_data.in_word = 0;
    while (input[++lwc_data.i])
    {
        if (input[lwc_data.i] == '\'' && !lwc_data.d_quote)
            lwc_data.s_quote = !lwc_data.s_quote;
        else if (input[lwc_data.i] == '"' && !lwc_data.s_quote)
            lwc_data.d_quote = !lwc_data.d_quote;
        if (input[lwc_data.i] != ' ' || lwc_data.s_quote || lwc_data.d_quote)
        {
            if (!lwc_data.in_word)
            {
                lwc_data.word_count++;
                lwc_data.in_word = 1;
            }
        }
        else
            lwc_data.in_word = 0;
    }
    return (lwc_data.word_count);
}

static void	handle_space(char *trimmed_input, int *i, int *j, 
            char **args, int *word)
{
    if (*i > *j)
    {
        args[*word] = ft_substr(trimmed_input, *j, *i - *j);
        (*word)++;
    }
    while (trimmed_input[*i] && trimmed_input[*i] == ' ')
        (*i)++;
    *j = *i;
}

static void	process_char(char *input, int *quotes, int *indices, char **args)
{
    if (input[indices[0]] == '\'' && !quotes[0])
        quotes[1] = !quotes[1];
    else if (input[indices[0]] == '"' && !quotes[1])
        quotes[0] = !quotes[0];
    if (input[indices[0]] == ' ' && !quotes[0] && !quotes[1])
    {
        handle_space(input, &indices[0], &indices[1], args, &indices[2]);
        return ;
    }
    indices[0]++;
}

static void	finalize_split(char *input, int *indices, char **args)
{
    if (indices[0] > indices[1])
    {
        args[indices[2]] = ft_substr(input, indices[1], 
            indices[0] - indices[1]);
        indices[2]++;
    }
    args[indices[2]] = NULL;
}

char	**split_by_quote(char *input)
{
    int		quotes[2];
    int		indices[3];
    char	**args;
    char	*trimmed_input;

    trimmed_input = ft_strtrim(input, " ");
    args = malloc(sizeof(char *) * (lexer_word_count(trimmed_input) + 1));
    quotes[0] = 0;
    quotes[1] = 0;
    indices[0] = 0;
    indices[1] = 0;
    indices[2] = 0;
    while (trimmed_input[indices[0]])
        process_char(trimmed_input, quotes, indices, args);
    finalize_split(trimmed_input, indices, args);
    free(trimmed_input);
    return (args);
}
