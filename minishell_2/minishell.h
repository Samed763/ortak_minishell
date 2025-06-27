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


// Global variable for signal handling
extern int g_signal_received;


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

typedef struct s_parsed
{
    char ***args;           // komut ve argümanlar
    char *input_file;      // < file
    char *output_file;     // > file veya >> file
    int   append;          // >> varsa 1, yoksa 0
    int   heredoc;         // << varsa 1
    char *heredoc_limiter; // << limiter
    char *heredoc_content; // heredoc content
} t_parsed;

typedef struct s_data
{
    t_parsed *parsed;
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
char	*ft_substr(char const *s, unsigned int start, size_t len);
char    *trim_quotes(char *arg, int free_old,int take_dollar);
void	*ft_memcpy(void *dst, const void *src, size_t n);
void    *Malloc(size_t size);

//geçiciler
void print_tokens(char **tokens);
void print_parsed_commands(t_data *data);

// Executor functions
void executor(t_data *data);
void execute_pipeline(t_data *data, int cmd_count);
int execute_builtin(t_data *data, char **cmd, int cmd_idx);
void execute_external(t_data *data, char **cmd, char **envp);
int is_builtin(char *cmd);
void setup_redirections(t_data *data);
void setup_pipes(t_data *data, int cmd_idx, int cmd_count, int *pipe_fds);
void handle_heredoc(t_data *data);
int cmd_exit(t_data *data, char **cmd);
int cmd_echo(char **cmd);
int cmd_cd(t_data *data, char **cmd);
int cmd_pwd();
int cmd_env(t_data *data);
int cmd_export(t_data *data, char **cmd);
int cmd_unset(t_data *data, char **cmd);

// Signal handling functions
void setup_signals(void);
void signal_handler_interactive(int sig);
void signal_handler_heredoc(int sig);
void signal_handler_execution(int sig);
void setup_signals_for_execution(void);
void setup_signals_for_heredoc(void);
void restore_signals(void);

// Memory cleanup functions
void free_parsed(t_parsed *parsed);
void free_tokens(char **tokens);
void free_env_dictionary(char ***env_dict);

#endif