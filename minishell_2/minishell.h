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


// String manipulation and utility functions
char	**ft_split(char const *s, char c);               // Split string by delimiter
char    **lexer(char *input);                            // Tokenize input into command tokens

// Environment variable management
char    ***make_env_dictionary(char **envp);             // Convert envp to dictionary format
char    *get_value_by_key(char ***env_dict, const char *key);  // Get environment variable value
void    print_env_dictionary(char ***env);               // Print all environment variables

// Basic string utilities
int	    ft_strlen(const char *str);                      // Calculate string length
int	    ft_strcmp(const char *s1, const char *s2);       // Compare two strings
char	*ft_strdup(const char *s1);                      // Duplicate string
char	*ft_strjoin(char *s1, char *s2);                  // Join two strings
char	*ft_substr(char const *s, unsigned int start, size_t len);  // Extract substring
char    *trim_quotes(char *arg, int free_old,int take_dollar);    // Remove quotes from string
void	*ft_memcpy(void *dst, const void *src, size_t n); // Copy memory
void    *Malloc(size_t size);                            // Safe malloc wrapper

// Debug and utility functions
void print_tokens(char **tokens);                        // Print tokenized input (debug)
void print_parsed_commands(t_data *data);                // Print parsed command structure (debug)

// Command execution functions
void executor(t_data *data);                             // Main executor function
void execute_pipeline(t_data *data, int cmd_count);      // Execute piped commands
int execute_builtin(t_data *data, char **cmd, int cmd_idx);  // Execute builtin commands
void execute_external(t_data *data, char **cmd, char **envp);  // Execute external commands
int is_builtin(char *cmd);                               // Check if command is builtin

// I/O redirection and pipe management
void setup_redirections(t_data *data);                   // Setup input/output redirections
void setup_pipes(t_data *data, int cmd_idx, int cmd_count, int *pipe_fds);  // Setup pipes for command
void handle_heredoc(t_data *data);                       // Handle heredoc input

// Builtin command implementations
int cmd_exit(t_data *data, char **cmd);                  // Exit command
int cmd_echo(char **cmd);                                // Echo command
int cmd_cd(t_data *data, char **cmd);                    // Change directory command
int cmd_pwd();                                           // Print working directory command
int cmd_env(t_data *data);                               // Print environment variables
int cmd_export(t_data *data, char **cmd);                // Export environment variables
int cmd_unset(t_data *data, char **cmd);                 // Unset environment variables

// Signal handling functions
void setup_signals(void);                                // Setup signals for interactive mode
void signal_handler_interactive(int sig);                // Signal handler for interactive mode
void signal_handler_heredoc(int sig);                    // Signal handler for heredoc mode
void signal_handler_execution(int sig);                  // Signal handler for command execution
void setup_signals_for_execution(void);                  // Setup signals for execution mode
void setup_signals_for_heredoc(void);                    // Setup signals for heredoc mode
void restore_signals(void);                              // Restore default signal handlers

// Memory cleanup functions
void free_parsed(t_parsed *parsed);                      // Free parsed command structure
void free_tokens(char **tokens);                         // Free token array
void free_env_dictionary(char ***env_dict);              // Free environment dictionary

#endif