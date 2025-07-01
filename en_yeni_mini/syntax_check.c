#include "minishell.h"

static int	in_pipe_check(char *input, int i)
{
    int	j;

    if (input[i + 1] == '\0' || input[i + 1] == '|')
        return (0);
    j = i + 1;
    while (input[j] == ' ' || input[j] == '\t')
        j++;
    if (input[j] == '\0' || !is_valid_after_pipe(input[j]))
        return (0);
    return (1);
}

static int	in_redirection_check(char *input, int i)
{
    int	j;

    j = i;
    if (input[i] == '>' && input[i + 1] == '>')
        j = i + 2;
    else if (input[i] == '<' && input[i + 1] == '<')
        j = i + 2;
    else
        j = i + 1;
    while (input[j] == ' ' || input[j] == '\t')
        j++;
    if (input[j] == '\0' || !is_valid_filename_char(input[j]))
        return (0);
    return (1);
}

static int	syntax_check_while(char *input, int i, int *quotes)
{
    if (input[i] == '\'' && quotes[1] == 0)
        quotes[0] = !quotes[0];
    else if (input[i] == '"' && quotes[0] == 0)
        quotes[1] = !quotes[1];
    if (quotes[0] == 0 && quotes[1] == 0)
    {
        if (input[i] == '|')
        {
            if (!in_pipe_check(input, i))
                return (0);
        }
        if (input[i] == '>' || input[i] == '<')
        {
            if (!in_redirection_check(input, i))
                return (0);
        }
    }
    return (1);
}

int	syntax_check(char *input)
{
    int	i;
    int	quotes[2];

    if (input[0] == '\0')
        return (0);
    i = 0;
    quotes[0] = 0;
    quotes[1] = 0;
    while (input[i] != '\0')
    {
        if (!syntax_check_while(input, i, quotes))
            return (0);
        i++;
    }
    if (quotes[0] || quotes[1])
        return (0);
    return (1);
}