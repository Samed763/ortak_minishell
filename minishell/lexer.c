#include "minishell.h"

char * put_value_in_dollar(char * token_arg,t_data *data)
{
    int i = 0, j = 1, k = 0;
    char *key;
    char *value;
	while (token_arg[i])
		i++;

	key = malloc(i);
	if (!key)
        return NULL;
	while (token_arg[j])
	{
		key[k] = token_arg[j];
		k++;
		j++;
	}
	key[k] = '\0';
	value = get_value_by_key(data->env->env_dictionary, key);
    free(key);
	return	value;
}

int is_dollar(char ** token,t_data * data)
{
	int i = 0;
	while (token[i])
	{
		if (token[i][0] == '$')
			token[i] = ft_strdup(put_value_in_dollar(token[i],data));
		i++;
	}
	return 0;
}

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	size_t	i;

	i = 0;
	if (!dst && !src)
		return (0);
	while (i < n)
	{
		((char *)dst)[i] = ((char *)src)[i];
		i++;
	}
	return (dst);
}

int	ft_strlen(const char *str)
{
	int	i;

	i = 0;
	while (str[i] != '\0')
		i++;
	return (i);
}

char	*ft_strdup(const char *s1)
{
	char	*s2;
	size_t	len;

	len = ft_strlen(s1);
	s2 = (char *)malloc(len + 1);
	if ((s2) == NULL)
		return (NULL);
	ft_memcpy(s2, s1, len + 1);
	return (s2);
}



char	*ft_substr(char const *s, unsigned int start, size_t len)
{
    char	*res;
	size_t	i;
	size_t	slen;

	i = 0;
	slen = ft_strlen(s);
	if (slen <= start)
		return (ft_strdup(""));
	if (len > slen - start)
		len = slen - start;
	res = (char *)malloc(len + 1);
	if (!res)
		return (NULL);
	while (s[start + i] && i < len)
	{
		res[i] = s[start + i];
		i++;
	}
	res[i] = '\0';
	return (res);
}

char **lexer(char *input) // echo uzak "dur salih  'samed' " dinç
{
    char **parts;
    int i = 0, j = 0, word = 0;
    int in_dquote = 0;  // Double quote flag
    int in_squote = 0;  // Single quote flag
    
    // Allocate memory for parts array
    parts = (char **)malloc(sizeof(char *) * 1024);
    if (!parts)
        return NULL;
    
    while (input[i])
    {
        // Handle quotes
        if (input[i] == '"' && !in_squote)
            in_dquote = !in_dquote;
        else if (input[i] == '\'' && !in_dquote)
            in_squote = !in_squote;
        
        // Only split on spaces when not inside quotes
        if (input[i] == ' ' && !in_dquote && !in_squote)
        {
            if (i > j)  // Only create token if length > 0
            {
                parts[word] = ft_substr(input, j, i - j);
                word++;
            }
            j = i + 1;  // Move start pointer past the space
        }
        i++;
    }
    
    // Add the last token if any
    if (i > j)
    {
        parts[word] = ft_substr(input, j, i - j);
        word++;
    }
    parts[word] = NULL;  // Null-terminate the array
    return parts;
}