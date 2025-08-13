/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 17:17:58 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/13 18:41:02 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H


# include <readline/history.h>
# include <readline/readline.h>
# include <stdio.h>

# include <signal.h>
# include <sys/wait.h>
# include <unistd.h>
#include <sys/stat.h>

# include <fcntl.h>
# include <stdlib.h>

# define TOKEN_WORD 0
# define TOKEN_PIPE 1
# define TOKEN_REDIRECT_IN 2
# define TOKEN_REDIRECT_OUT 3
# define TOKEN_APPEND 4
# define TOKEN_HEREDOC 5

typedef struct s_heredoc_line
{
	char					*content;
	struct s_heredoc_line	*next;
}							t_heredoc_line;

typedef struct s_heredoc
{
	char					*delimiter;
	int						should_expand;
	t_heredoc_line			*lines;
	struct s_heredoc		*next;
}							t_heredoc;

// YENİ: Her bir yönlendirmeyi temsil eden struct
typedef struct s_redir
{
	char			*filename;      // Dosya adı veya heredoc için delimiter
	int				type;           // TOKEN_REDIRECT_IN, OUT, APPEND, HEREDOC
	struct s_redir	*next;
}					t_redir;


// GÜNCELLENMİŞ: t_command struct'ı
typedef struct s_command
{
	char				**args;
	pid_t				pid;
	int					pipe_fd[2];		// Pipe file descriptors for this command
	t_redir				*redirs; // Bütün yönlendirmeler artık burada, sırayla tutulacak.
	t_heredoc			*heredocs; // Heredoc içeriklerini tutmak için bu yapıyı koruyabiliriz.
	struct s_command	*next;
}						t_command;

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
	int						original_stdin;
	int						original_stdout;
	t_command				*cmd;
}							t_data;

typedef struct s_pipe_data
{
	t_data					*data;
	t_command				*current;
	t_command				*prev_cmd;
}							t_pipe_data;

void	cleanup_and_exit(int exit_code);
int	syntax_check(char *line);
void	lexer(char *line, t_data *data);
void	expander(t_data *data);
t_command	*parser(t_data *data);void execute_command(t_data *data);
void	free_data_resources(t_data *data);
int	setup_signals(void);
char	**copy_env(char **envp);
t_data	*get_data_instance(t_data *data_to_set);
char	*extract_variable_key(char *line, int start_pos, int *end_pos);
char	*find_value_by_key(t_data *data, char *key);
char	*put_var(char *line, char *var_value, int key_start, int key_end);
char	*ft_strdup(const char *s1);
void	update_quoting_state(char c, int *s_quotes, int *d_quotes);
int	is_valid_to_expand(int c);
int	is_identifier_char(int c);
char	*ft_substr(char const *s, unsigned int start, size_t len);
char	*ft_strjoin(char const *s1, char const *s2);
size_t	ft_strlen(const char *str);
void	*ft_memcpy(void *dst, const void *src, size_t n);
int	ft_strcmp(const char *s1, const char *s2);
void	read_pipe_fill_list(int pipe_read_fd, t_heredoc *heredoc);
void	expand_heredoc_lines(t_data *data, t_command *cmd);
char	*expand_single_line(t_data *data, char *line, int f);
char	*ft_itoa(int n);
int	ft_strncmp(const char *s1, const char *s2, size_t n);
char	*remove_quotes(char *str);
void	free_word_array(char **array);
char	**ft_split(char const *s, char c);
void	pipe_execute(t_data *data);
int	is_builtin(char *command);
int	handle_redirections(t_command *cmd);
int	is_accessable(char *command, char **splited_path, char **full_path);
int	try_builtin(t_command *current_cmd, t_data *data, int is_parent);
void	restore_fds(t_data *data);
void	set_exit_status(t_data *data, int status);
int	handle_heredoc(t_data *data, t_command *cmd);
void	pipe_child_routine(t_pipe_data *p_data);
void	cleanup_pipe_fds(t_data *data);
void	wait_for_all_children(t_data *data);
void write_error_and_exit(int exit_val, char *arg, char *error);
char	*ft_strchr(const char *s, int c);
int	is_token(int c);
int	get_token_type(char *token);
int	count_word(char *line);
int	add_heredoc_to_command(t_command *cmd, char *raw_word);
void	add_redir_to_list(t_command *cmd, char *filename, int type);
t_command	*create_list(void);
void	free_command_list(t_command *head);
void	add_argument_to_command(t_command *cmd, char *word);
char	*remove_quotes_from_word(char *str, int *should_expand);
char	*ft_strndup(const char *s, size_t n);
int	find_env_var(char **env, char *new_entry, int name_len);
int	is_valid_var(const char *str);
int	builtin_exit(t_command *cmd, t_data *data, int is_parent);
int	builtin_env(char **env);
int	builtin_unset(t_data *data);
int	builtin_export(t_data *data);
int	builtin_pwd(void);
int	builtin_cd(t_data *data, char **args);
int	builtin_echo(char **args);
int	apply_specific_heredoc(t_heredoc *heredoc);
void	check_error(int access_ret, t_data *data);

#endif
