#include "minishell.h"

int	is_valid_after_pipe(int c)
{
	return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || 
			(c >= '0' && c <= '9') || c == '_' || c == '-' || 
			c == '.' || c == '/' || c == '\'' || c == '"' || 
			c == '$' || c == ' ' || c == '\t');
}

int is_valid_filename_char(int c)
{
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || 
            (c >= '0' && c <= '9') || c == '_' || c == '-' || 
            c == '.' || c == '/' || c == '~' || c == '\'' || 
            c == '"' || c == '$');
}


char	*ft_strchr(const char *s, int c)
{
    while (*s != (char)c)
    {
        if (*s == '\0')
            return (NULL);
        s++;
    }
    return ((char *)s);
}

size_t	ft_strlen(const char *str)
{
    size_t	i;

    i = 0;
    while (str[i] != '\0')
        i++;
    return (i);
}

void *Malloc(size_t size)
{
    void *ptr;
    if (size == 0)
        return (NULL);
    ptr = malloc(size);
    if (!ptr)
    {
        perror("Malloc BOMM");
        exit(1);
    }   
    return (ptr);
}

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
    size_t	i;

    i = 0;
    if (!dst && !src)
        return (0);
    while (i < n)
    {
        ((unsigned char *)dst)[i] = ((unsigned char *)src)[i];
        i++;
    }
    return (dst);
}
// utils.c dosyasına ekle
void	free_word_array(char **array)
{
    int	i;

    if (!array)
        return ;
    i = 0;
    while (array[i])
    {
        free(array[i]);
        i++;
    }
    free(array);
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
    if (!s)
        return (NULL);
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

int ft_strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return (*(unsigned char*)s1 - *(unsigned char*)s2);
}

char	*ft_strtrim(char const *s1, char const *set)
{
    size_t	start;
    size_t	end;
    size_t	i;
    char	*mem;

    start = 0;
    if (!s1 || !set)
        return (NULL);
    end = ft_strlen(s1);
    while (s1[start] && ft_strchr(set, s1[start]))
        start++;
    while (end > start && ft_strchr(set, s1[end - 1]))
        end--;
    mem = (char *)malloc(end - start + 1);
    if (!mem)
        return (NULL);
    i = 0;
    while (start < end)
        mem[i++] = s1[start++];
    mem[i] = '\0';
    return (mem);
}


char	*ft_strjoin(char const *s1, char const *s2)
{
    int		i;
    int		j;
    char	*mem;

    if (!s1 || !s2)  // ✅ NULL kontrolü eklendi
        return (NULL);
    i = 0;
    j = 0;
    mem = (char *)malloc(ft_strlen(s1) + ft_strlen(s2) + 1);
    if (!mem)
        return (NULL);
    while (s1[i])
    {
        mem[i] = s1[i];
        i++;
    }
    while (s2[j])
    {
        mem[i + j] = s2[j];
        j++;
    }
    mem[i + j] = '\0';
    return (mem);
}
char *remove_d_quotes(char *str)
{
    int len = ft_strlen(str);
    int start = 0;
    int end = len - 1;
    
    // Başlangıç ve bitiş tırnak kontrolü
    if (len >= 2 && 
        ((str[0] == '"' && str[end] == '"')))
    {
        start = 1;
        end = len - 2;
    }
    
    // Yeni string oluştur
    char *result = ft_substr(str, start, end - start + 1);
    return result;
}

char	*find_value_by_key(t_data *data, char *key) // bulamazsa patlıyor
{
    int		i;
    char	*search;

    i = 0;
    search = ft_strjoin(key, "=");
    if (!search)
        return (NULL);
    while (data->env[i])
    {
        if (!strncmp(data->env[i], search, ft_strlen(search)))
        {
            free(search);
            return (ft_strdup(data->env[i] + ft_strlen(search)));
        }
        i++;
    }
    free(search);
    return (NULL);
}