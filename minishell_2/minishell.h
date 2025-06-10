#ifndef MINISHELL_H
#define MINISHELL_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>  
#include <errno.h>


typedef struct s_env
{
    char ***env_dictionary;
    /*
        env[i][0]
        [i][0] == KEY
        [i][1] == Value
    */

    //------------//düzen çek
} t_env;

typedef struct s_data
{
    t_env *env;
    char ** args;
    int last_exit_status;
} t_data;


char	**ft_split(char const *s, char c);
char    **lexer(char *input);

char    ***make_env_dictionary(char **envp);
char    *get_value_by_key(char ***env_dict, const char *key);
void    print_env_dictionary(char ***env);

int	    ft_strlen(const char *str);
int	    ft_strcmp(const char *s1, const char *s2);
char	*ft_strdup(const char *s1);
char	*ft_strjoin(char *s1, char *s2);
char    *trim_quotes(char *arg,int free_old);
void	*ft_memcpy(void *dst, const void *src, size_t n);
void    *Malloc(size_t size);

//geçiciler
void print_tokens(char **tokens);

#endif