#ifndef MINISHELL_H
#define MINISHELL_H

#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

#define TOKEN_WORD          0
#define TOKEN_PIPE          1    // |
#define TOKEN_REDIRECT_IN   2    // <
#define TOKEN_REDIRECT_OUT  3    // >
#define TOKEN_APPEND        4    // >>
#define TOKEN_HEREDOC       5    // <<

typedef struct s_command
{
    char **args;           // Command arguments (including command itself)
    char *input_file;      // Input redirection file
    char *output_file;     // Output redirection file
    int append_mode;       // 1 if >>, 0 if >
    char *heredoc_delimiter; // Heredoc delimiter
    struct s_command *next; // Next command in pipeline
}t_command;


typedef struct s_data
{
    char **word_array;
    char **env;
    int *token;
    t_command *cmd;
}t_data;



int try_builtin(char **args, char **env);

//syntax_check.c
int	syntax_check(char *input);

//split_by_quote.c
char **split_by_quote(char *input);

//utils.c
int	    is_valid_after_pipe(int c);
int     is_valid_filename_char(int c);
int     ft_strcmp(const char *s1, const char *s2);
void    *Malloc(size_t size);
void	free_word_array(char **array);
void	*ft_memcpy(void *dst, const void *src, size_t n);
char	*ft_strdup(const char *s1);
char	*ft_strchr(const char *s, int c);
char	*ft_strjoin(char const *s1, char const *s2);
char	*ft_strtrim(char const *s1, char const *set);
char	*ft_substr(char const *s, unsigned int start, size_t len);
size_t	ft_strlen(const char *str);

//tokenize_word
int		*tokenize_words(char **word_array);
void	print_tokens(char **word_array, int *tokens);

//parse_commands
t_command	*parse_commands(char **word_array, int *tokens);
void	print_parsed_commands(t_command *commands);

//env.c
char ** copy_env(char **envp);
char ** add_to_env(char **env,char *input);

//check_start_var.c
void	check_start_var(t_data *data);

//split.c
char	**ft_split(char const *s, char c);

//execute_command.c
void	execute_command(t_data *data);
char	*find_value_by_key(t_data *data, char *key);



#endif
