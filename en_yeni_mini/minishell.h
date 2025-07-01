#ifndef MINISHELL_H
#define MINISHELL_H

#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct s_data
{
    char **word_array;
}t_data;

//syntax_check.c
int	syntax_check(char *input);

//split_by_quote.c
char **split_by_quote(char *input);

//utils.c
int	    is_valid_after_pipe(int c);
int     is_valid_filename_char(int c);
int     ft_strcmp(const char *s1, const char *s2);
void    *Malloc(size_t size);
void	*ft_memcpy(void *dst, const void *src, size_t n);
char	*ft_strdup(const char *s1);
char	*ft_strchr(const char *s, int c);
char	*ft_strtrim(char const *s1, char const *set);
char	*ft_substr(char const *s, unsigned int start, size_t len);
size_t	ft_strlen(const char *str);




#endif
