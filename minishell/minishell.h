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


typedef struct s_echo
{
    char * echo_input;
} t_echo;

typedef struct s_cd
{
    char * cd_input;
} t_cd;

typedef struct s_export
{
    char * key;
    char * value; 
    //int is_valid; eğer gerekirse ?----- 
} t_export;

typedef struct s_unset
{
    char * unset_name; 
    //int is_valid; eğer varsa  ?----- 
} t_unset;

typedef struct s_exit
{
    int exit_value; 
} t_exit;

typedef struct s_env
{
    char ***env_dictionary;
    /*
        env[i][0]
        [i][0] == KEY
        [i][1] == Value
    */

    //------------//düzen çek
    char **env_copy;
    char *path;
} t_env;

typedef struct s_data
{
    t_echo *echo;
    t_cd *cd;
    t_export *export;
    t_unset *unset;
    t_exit * exit;
    t_env *env;
} t_data;

//kotnrol et
char * find_path(char **env);
char **copy_env(char **envp);
void print_env(char **env);//geçici//geçici
void update_env_value(char ***env_dict, const char *key, const char *value);
char ***make_env_dictionary(char **envp);//küçük dizeltme lazım
int	ft_strlen(const char *str);
//tahminimce kalır
char *get_value_by_key(char ***env_dict, const char *key);
void print_env_dictionary(char ***env);
void print_export(char ***env);
char	**ft_split(char const *s, char c);
// minishell.h içine ekleyin
char *find_executable(const char *command, char ***env_dict);
int is_builtin(char *input); //find the built in fonk and return his value
char **lexer(char *input); // echo uzak "dur salih  'samed' " dinç
void	*ft_memcpy(void *dst, const void *src, size_t n);


//utils
char	*ft_strdup(const char *s1);
char	*ft_substr(char const *s, unsigned int start, size_t len);
char	*ft_strjoin(char const *s1, char const *s2);
void update_env_value(char ***env_dict, const char *key, const char *value);
char ***add_env_variable(char ***env_dict, t_export *st_export);

//built-in foncs
int ft_echo(char **token);

int ft_cd(char ** token,t_data *data);

void ft_pwd(void);

#endif