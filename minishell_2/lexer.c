#include "minishell.h"



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