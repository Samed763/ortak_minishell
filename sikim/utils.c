#include "minishell.h"

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