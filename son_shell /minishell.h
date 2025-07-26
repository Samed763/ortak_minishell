#ifndef MINISHELL_H
#define MINISHELL_H

#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <termios.h>


#define TOKEN_WORD 0
#define TOKEN_PIPE 1         // |
#define TOKEN_REDIRECT_IN 2  // <
#define TOKEN_REDIRECT_OUT 3 // >
#define TOKEN_APPEND 4       // >>
#define TOKEN_HEREDOC 5      // <<

extern volatile sig_atomic_t g_heredoc_interrupted;

typedef struct s_heredoc_line
{
    char *content;
    struct s_heredoc_line *next;
} t_heredoc_line;

typedef struct s_command
{
    char **args;             // Command arguments (including command itself)
    char *input_files;       // Input redirection file
    char **output_files;     // Multiple output files array
    int *append_modes;       // Append modes for each output file
    int output_count;        // Number of output files
    char *heredoc_delimiter; // Heredoc delimiter
    int  should_expand_heredoc;  //1 ise genişlet, 0 ise genişletme
    struct s_command *next;  // Next command in pipeline
    t_heredoc_line *heredoc_lines;
} t_command;

typedef struct s_data
{
    char **word_array;
    char **env;
    int *token;
    int exit_value;
    char **splitted_path;
    t_command *cmd;
} t_data;

// utils.c
void *Malloc(size_t size);
void free_word_array(char **array);
void *ft_memcpy(void *dst, const void *src, size_t n);
int is_token(int c);
int is_valid_after_pipe(int c);
int is_valid_filename_char(int c);
int ft_strcmp(const char *s1, const char *s2);
int ft_strncmp(const char *s1, const char *s2, size_t n);
char *ft_strdup(const char *s1);
char *remove_d_quotes(char *str);
char *ft_strchr(const char *s, int c);
char *ft_strcpy(char *dest, const char *src);
char *ft_strjoin(char const *s1, char const *s2);
char *find_value_by_key(t_data *data, char *key);
char *ft_strtrim(char const *s1, char const *set);
char *ft_substr(char const *s, unsigned int start, size_t len);
char **copy_env(char **envp);
size_t ft_strlen(const char *str);



// Built-in fonksiyonları için prototipler
int try_builtin(t_command *current_cmd, t_data *data, int is_parent);
int	builtin_cd(char **args);
int	builtin_echo(char **args);
int	builtin_env(char **env);
int builtin_exit(t_data *data); //
int	builtin_pwd(void);
void builtin_export(t_data *data);
int	builtin_unset(t_data *data);
int	is_valid_var(const char *str);
int	find_env_var(char **env, char *new_entry, int name_len);
int	add_new_var(t_data *data, char *new_entry);

// syntax_check.c
int syntax_check(char *line);

// lexer.c
void lexer(char *line, t_data *data);

// parser.c
t_command *parser(t_data *data);

// expander.c
void expander(t_data *data);
char *expand_single_line(t_data *data, char *line);

// heredoc.c
int handle_heredoc(t_data *data, t_command *cmd);

// split.c
char **ft_split(char const *s, char c);

// execute_commad.c
void execute_commmand(t_data *data);
void execute(char *full_path, t_data *data);
int is_accessable(char *command, char **splited_path, char **full_path);

// apply_input.c
int apply_input_redirection(t_command *cmd);


// apply_output.c
int apply_output_redirection(t_command *cmd);

// pipe_execute.c
void pipe_execute(t_data *data, char **splitted_path);

//minishell.c
void    signal_handler(int signum);
void    heredoc_signal_handler(int signum);

//leak_manager.c
void free_command_list(t_command *head);
void free_heredoc_lines(t_heredoc_line *head);
void free_data_resources(t_data *data);
void    cleanup_and_exit(t_data *data, int exit_code);

#endif
