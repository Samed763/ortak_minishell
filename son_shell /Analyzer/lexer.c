#include "../minishell.h"


int count_word(char *line)
{
    int i = 0;
    int	in_single_quote = 0;
    int	in_double_quote = 0;
    int wc = 0;
    int word = 0;
    
    while (line[i])
    {
        if (line[i] == '\'' && !(in_double_quote))
        {
            in_single_quote = !(in_single_quote);
            if (!word)
                word = 1;
        }
        else if (line[i] == '"' && !(in_single_quote))
        {
            in_double_quote = !(in_double_quote);
            if (!word)
                word = 1;
        }
        else if (line[i] == '\0' || 
            (!in_single_quote && !in_double_quote && 
            (line[i] == ' ' || is_token(line[i]))))
        {
            if (word)
            {
                wc++;
                word = 0;
            }
            // Çift karakter token kontrolü
            if (is_token(line[i]) && line[i + 1] && line[i] == line[i + 1])
            {
                wc++;
                i++; // Çift karakteri atla
            }
            else if (is_token(line[i]))
            {
                wc++;
            }
        }
        else
        {
            if (!word)
                word = 1;
        }
        i++;
    }
    
    // Son kelimeyi say
    if (word)
        wc++;
    
    return wc;
}

static char *extract_token(char *line, int *start)
{
    int len = 1;
    
    // Çift karakter token kontrolü (<<, >>)
    if ((line[*start] == '<' && line[*start + 1] == '<') ||
        (line[*start] == '>' && line[*start + 1] == '>'))
    {
        len = 2;
        *start += 2;
    }
    else
        (*start)++;
    
    return ft_substr(line, *start - len, len);
}

static char *extract_word(char *line, int *start)
{
    int word_start = *start;
    int in_single_quote = 0;
    int in_double_quote = 0;
    
    while (line[*start])
    {
        if (line[*start] == '\'' && !in_double_quote)
            in_single_quote = !in_single_quote;
        else if (line[*start] == '"' && !in_single_quote)
            in_double_quote = !in_double_quote;
        else if (!in_single_quote && !in_double_quote && 
                (line[*start] == ' ' || is_token(line[*start])))
            break;
        (*start)++;
    }
    
    return ft_substr(line, word_start, *start - word_start);
}

static void skip_whitespace(char *line, int *pos)
{
    while (line[*pos] && (line[*pos] == ' ' || line[*pos] == '\t'))
        (*pos)++;
}

static int get_token_type(char *token)
{
    if (!token)
        return TOKEN_WORD;
    
    if (ft_strcmp(token, "|") == 0)
        return TOKEN_PIPE;
    else if (ft_strcmp(token, "<") == 0)
        return TOKEN_REDIRECT_IN;
    else if (ft_strcmp(token, ">") == 0)
        return TOKEN_REDIRECT_OUT;
    else if (ft_strcmp(token, ">>") == 0)
        return TOKEN_APPEND;
    else if (ft_strcmp(token, "<<") == 0)
        return TOKEN_HEREDOC;
    else
        return TOKEN_WORD;
}

void lexer(char *line, t_data *data)
{
    int i = 0;
    int word_index = 0;
    
    int word_count = count_word(line);
    data->word_array = Malloc(sizeof(char *) * (word_count + 1));
    data->token = Malloc(sizeof(int) * (word_count + 1));

    while (line[i])
    {
        skip_whitespace(line, &i);
        if (!line[i])
            break;
        if (is_token(line[i]))
            data->word_array[word_index] = extract_token(line, &i);
        else
            data->word_array[word_index] = extract_word(line, &i);
        data->token[word_index] = get_token_type(data->word_array[word_index]);
        word_index++;
    }
    data->word_array[word_index] = NULL;
    data->token[word_index] = -1; // End marker
}
