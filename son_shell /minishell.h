/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 17:17:58 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/03 17:38:53 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/wait.h>
# include <termios.h>
# include <unistd.h>

# define TOKEN_WORD 0
# define TOKEN_PIPE 1         // |
# define TOKEN_REDIRECT_IN 2  // <
# define TOKEN_REDIRECT_OUT 3 // >
# define TOKEN_APPEND 4       // >>
# define TOKEN_HEREDOC 5      // <<

typedef struct s_heredoc_line
{
	char					*content;
	struct s_heredoc_line	*next;
}							t_heredoc_line;

typedef struct s_command
{
	char					**args;
	char					*input_files;
	char					**output_files;
	int						*append_modes;
	int						output_count;
	char					*heredoc_delimiter;
	int						should_expand_heredoc;
	struct s_command		*next;
	t_heredoc_line			*heredoc_lines;
}							t_command;

typedef struct s_expand_state
{
	int						s_quotes;
	int						d_quotes;
	int						i;
}							t_expand_state;

typedef struct s_data
{
	char					**word_array;
	char					**env;
	int						*token;
	int						exit_value;
	char					**splitted_path;
	t_command				*cmd;
}							t_data;
typedef struct s_pipe_data
{
	t_data					*data;
	t_command				*current;
	int						*pipefd;
	int						prev_fd;
}							t_pipe_data;
void						free_word_array(char **array);
void						*ft_memcpy(void *dst, const void *src, size_t n);
int							is_token(int c);
int							is_valid_after_pipe(int c);
int							is_valid_filename_char(int c);
int							ft_strcmp(const char *s1, const char *s2);
int							ft_strncmp(const char *s1, const char *s2,
								size_t n);
char						*ft_strdup(const char *s1);
char						*ft_strchr(const char *s, int c);
char						*ft_strjoin(char const *s1, char const *s2);
char						*find_value_by_key(t_data *data, char *key);
char						*ft_substr(char const *s, unsigned int start,
								size_t len);
char						**copy_env(char **envp);
size_t						ft_strlen(const char *str);
int							get_token_type(char *token);
char						*ft_itoa(int n);
int							try_builtin(t_command *current_cmd, t_data *data,
								int is_parent);
int							builtin_cd(char **args);
int							builtin_echo(char **args);
int							builtin_env(char **env);
int							builtin_exit(t_data *data);
int							builtin_pwd(void);
int	builtin_export(t_data *data);
int							builtin_unset(t_data *data);
int							is_valid_var(const char *str);
int							find_env_var(char **env, char *new_entry,
								int name_len);
int							add_new_var(t_data *data, char *new_entry);
int							syntax_check(char *line);
void						lexer(char *line, t_data *data);
t_command					*parser(t_data *data);
void						add_input_to_command(t_command *current,
								char *filename);
t_command					*create_list(void);
void						remove_quotes_parser_helper(const char *str,
								char **del, t_command *cur);
char						*remove_quotes_from_word(char *str);
void						add_output_to_command(t_command *curr,
								char *filename, int append_mode);
void						add_argument_to_command(t_command *cmd, char *word);
void						expander(t_data *data);
char						*expand_single_line(t_data *data, char *line);

int							is_identifier_char(int c);
void						update_quoting_state(char c, int *s_quotes,
								int *d_quotes);
char						*extract_variable_key(char *line, int start_pos,
								int *end_pos);
char						*put_var(char *line, char *var_value, int key_start,
								int key_end);
int							handle_heredoc(t_data *data, t_command *cmd);
void						expand_heredoc_lines(t_data *data, t_command *cmd);
void						read_from_pipe_and_fill_list(int pipe_read_fd,
								t_command *cmd);
char						**ft_split(char const *s, char c);
void						execute_command(t_data *data);
int							is_accessable(char *command, char **splited_path,
								char **full_path);
void						heredoc_child_process(t_data *data, int *pipefd,
								t_command *cmd);
int							heredoc_parent_process(int *pipefd);
int							is_builtin(char *command);
int							is_accessable(char *command, char **splited_path,
								char **full_path);
void						set_exit_status(t_data *data, int status);
void						restore_fds(int original_stdin,
								int original_stdout);
void						pipe_child_routine(t_pipe_data *p_data);
int							pipe_parent_routine(t_command *current, int *pipefd,
								int prev_fd);
void						handle_pipe_redirections(t_data *data,
								t_command *current, int *pipefd, int prev_fd);

void						wait_for_all_children(t_data *data);
int							apply_input_redirection(t_data *data,
								t_command *cmd);
int							apply_output_redirection(t_command *cmd);
void						pipe_execute(t_data *data);
void						signal_handler(int signum);
void						free_command_list(t_command *head);
void						free_heredoc_lines(t_heredoc_line *head);
void						free_data_resources(t_data *data);
void						cleanup_and_exit(t_data *data, int exit_code);
int							count_word(char *line);
int							setup_signals(void);
void						init_data(t_data *data, char **envp);
void						safe_dup2(int fd, int fd2, t_data *data);

#endif
