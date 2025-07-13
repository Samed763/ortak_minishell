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
    lwc_data.i = 0;
    lwc_data.word_count = 0;
    lwc_data.in_word = 0;
    
    while (input[lwc_data.i])
    {
        if (input[lwc_data.i] == '\'' && !lwc_data.d_quote)
            lwc_data.s_quote = !lwc_data.s_quote;
        else if (input[lwc_data.i] == '"' && !lwc_data.s_quote)
            lwc_data.d_quote = !lwc_data.d_quote;
        else if (!lwc_data.s_quote && !lwc_data.d_quote)
        {
            // Pipe karakteri için özel işlem
            if (input[lwc_data.i] == '|')
            {
                if (lwc_data.in_word)
                {
                    lwc_data.word_count++;
                    lwc_data.in_word = 0;
                }
                lwc_data.word_count++; // Pipe için ayrı token
            }
            // Redirection karakterleri için özel işlem
            else if (input[lwc_data.i] == '<' || input[lwc_data.i] == '>')
            {
                if (lwc_data.in_word)
                {
                    lwc_data.word_count++;
                    lwc_data.in_word = 0;
                }
                // >> ve << kontrolü
                if (input[lwc_data.i + 1] == input[lwc_data.i])
                {
                    lwc_data.i++; // İkinci karakteri atla
                }
                lwc_data.word_count++; // Redirection için ayrı token
            }
            else if (input[lwc_data.i] == ' ' || input[lwc_data.i] == '\t')
            {
                if (lwc_data.in_word)
                {
                    lwc_data.word_count++;
                    lwc_data.in_word = 0;
                }
            }
            else
            {
                lwc_data.in_word = 1;
            }
        }
        else
        {
            lwc_data.in_word = 1;
        }
        lwc_data.i++;
    }
    
    if (lwc_data.in_word)
        lwc_data.word_count++;
    
    return (lwc_data.word_count);
}

static void	handle_space(char *input, int *i, int *j, char **args, int *word_idx)
{
    if (*i > *j)
    {
        args[*word_idx] = ft_substr(input, *j, *i - *j);
        (*word_idx)++;
    }
    while (input[*i] && (input[*i] == ' ' || input[*i] == '\t'))
        (*i)++;
    *j = *i;
}

static void	handle_pipe(char *input, int *i, int *j, char **args, int *word_idx)
{
    // Önceki kelimeyi kaydet
    if (*i > *j)
    {
        args[*word_idx] = ft_substr(input, *j, *i - *j);
        (*word_idx)++;
    }
    
    // Pipe karakterini kaydet
    args[*word_idx] = ft_strdup("|");
    (*word_idx)++;
    
    // Sonraki karaktere geç
    (*i)++;
    *j = *i;
}

static void	handle_redirection(char *input, int *i, int *j, char **args, int *word_idx)
{
    // Önceki kelimeyi kaydet
    if (*i > *j)
    {
        args[*word_idx] = ft_substr(input, *j, *i - *j);
        (*word_idx)++;
    }
    
    // Redirection karakterini kaydet
    if (input[*i + 1] == input[*i]) // >> veya <<
    {
        if (input[*i] == '<')
            args[*word_idx] = ft_strdup("<<");
        else
            args[*word_idx] = ft_strdup(">>");
        (*i) += 2;
    }
    else // > veya <
    {
        if (input[*i] == '<')
            args[*word_idx] = ft_strdup("<");
        else
            args[*word_idx] = ft_strdup(">");
        (*i)++;
    }
    
    (*word_idx)++;
    *j = *i;
}

static void	process_char(char *input, int *quotes, int *indices, char **args)
{
    int i = indices[0];
    int j = indices[1];
    int word_idx = indices[2];
    
    if (input[i] == '\'' && !quotes[0])
        quotes[1] = !quotes[1];
    else if (input[i] == '"' && !quotes[1])
        quotes[0] = !quotes[0];
    
    if (!quotes[0] && !quotes[1])
    {
        if (input[i] == ' ' || input[i] == '\t')
        {
            handle_space(input, &i, &j, args, &word_idx);
        }
        else if (input[i] == '|')
        {
            handle_pipe(input, &i, &j, args, &word_idx);
        }
        else if (input[i] == '<' || input[i] == '>')
        {
            handle_redirection(input, &i, &j, args, &word_idx);
        }
        else
        {
            i++;
        }
    }
    else
    {
        i++;
    }
    
    indices[0] = i;
    indices[1] = j;
    indices[2] = word_idx;
}

static void	finalize_split(char *input, int *indices, char **args)
{
    if (indices[0] > indices[1])
    {
        args[indices[2]] = ft_substr(input, indices[1], indices[0] - indices[1]);
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

    trimmed_input = ft_strtrim(input, " \t");
    if (!trimmed_input)
        return (NULL);
    
    args = malloc(sizeof(char *) * (lexer_word_count(trimmed_input) + 1));
    if (!args)
    {
        free(trimmed_input);
        return (NULL);
    }
    
    quotes[0] = 0; // double quotes
    quotes[1] = 0; // single quotes
    indices[0] = 0; // current position
    indices[1] = 0; // word start
    indices[2] = 0; // word index
    
    while (trimmed_input[indices[0]])
        process_char(trimmed_input, quotes, indices, args);
    
    finalize_split(trimmed_input, indices, args);
    free(trimmed_input);
    return (args);
}