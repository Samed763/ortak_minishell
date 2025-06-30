#ifndef MINISHELL_H
#define MINISHELL_H

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

#define TOKEN_WORD          0
#define TOKEN_COMMAND       1
#define TOKEN_PIPE          2    // |
#define TOKEN_REDIRECT_IN   3    // <
#define TOKEN_REDIRECT_OUT  4    // >
#define TOKEN_APPEND        5    // >>
#define TOKEN_HEREDOC       6    // <<

typedef struct s_commands
{
    char **args;           // Command arguments (including command itself)
    char *input_file;      // Input redirection file
    char *output_file;     // Output redirection file
    int append_mode;       // 1 if >>, 0 if >
    char *heredoc_delimiter; // Heredoc delimiter
    struct s_commands *next; // Next command in pipeline
} t_commands;


typedef struct s_data
{
    char **word_array;
    int *token;
    t_commands *commands;  // Parsed command pipeline
} t_data;


char	*ft_strchr(const char *s, int c);
size_t	ft_strlen(const char *str);
void *Malloc(size_t size);
void	*ft_memcpy(void *dst, const void *src, size_t n);
char	*ft_strdup(const char *s1);
char	*ft_substr(char const *s, unsigned int start, size_t len);
int ft_strcmp(const char *s1, const char *s2);
char	*ft_strtrim(char const *s1, char const *set);

// Lexer functions
char **split_by_quote(char *input);
void print_word_array(char **word_array);

// Tokenizer functions  
int *tokenize_words(char **word_array);
int get_token_type(const char *str);
void mark_commands(char **word_array, int *token_types, int size);

// Parser functions
t_commands *parse_commands(char **word_array, int *tokens);
t_commands *create_command_node(void);
void add_argument_to_command(t_commands *cmd, char *arg);
void print_parsed_commands(t_commands *commands);
void free_commands(t_commands *commands);

// Memory cleanup functions
void free_word_array(char **word_array);
void free_token_array(int *tokens);

// Executor functions
int execute_commands(t_commands *commands);
int execute_single_command(t_commands *cmd);
int execute_pipeline(t_commands *commands);
int setup_redirections(t_commands *cmd);
int is_builtin(char *command);
int execute_builtin(char **args);
char *find_command_path(char *command);

#endif