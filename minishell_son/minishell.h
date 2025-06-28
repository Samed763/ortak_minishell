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
#include <fcntl.h>
#include <signal.h>

// Temel token türleri (bonus değil)
#define TOKEN_WORD          0
#define TOKEN_PIPE          1    // |
#define TOKEN_REDIRECT_IN   2    // <
#define TOKEN_REDIRECT_OUT  3    // >
#define TOKEN_APPEND        4    // >>
#define TOKEN_HEREDOC       5    // <<

typedef struct s_parsed_arg
{
    char ***arg;     // Her komut için ayrı char** array
    int  *tokens;    // Her kelimenin token türü
    int   count;     // Toplam kelime sayısı
} t_parsed_arg;

typedef struct s_parsed
{
    t_parsed_arg commands;   // Tek struct (array değil)
    char *input_file;        // < file
    char *output_file;       // > file veya >> file
    int   append;            // >> varsa 1, yoksa 0
    int   heredoc;           // << varsa 1
    char *heredoc_limiter;   // << limiter
    char *heredoc_content;   // heredoc content
    int   cmd_count;         // Toplam komut sayısı
} t_parsed;

typedef struct s_data
{
    t_parsed *parsed;
    char **env;
    char **args;
    int exit_status;
} t_data;

//utils.c
void *Malloc(size_t size);
size_t	ft_strlen(const char *str);
char	*ft_strdup(const char *s1);
char	*ft_strchr(const char *s, int c);
int     ft_strcmp(const char *s1, const char *s2);
char	*ft_strtrim(char const *s1, char const *set);
void	*ft_memcpy(void *dst, const void *src, size_t n);
char	*ft_substr(char const *s, unsigned int start, size_t len);

//lexer.c
char **lexer(char *input);

//main.c parser functions
void parser(t_data *data);
void parse_single_command(t_data *data, int cmd_index, int start, int end);
int is_redirection(char *token);
void handle_redirection(t_data *data, char *operator, char *file);
int get_total_args(char **args);
void print_d_pointer(char **args);
void init_parser(t_data *data);
void set_cmd_count(t_data *data);

#endif