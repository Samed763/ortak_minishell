#ifndef BUILT_IN_H
# define BUILT_IN_H
// --- DEĞİŞİKLİKLER BURADA BAŞLIYOR ---

// 1. Bu satırı SİL veya yorum satırı yap:
// # include "../minishell.h" 

// 2. Onun yerine bu satırları EKLE:
// Standart kütüphaneler (size_t için)
#include "minishell.h"

// Derleyiciye t_data'nın bir yapı olduğunu önceden bildiriyoruz.
// Buna "Forward Declaration" denir.
struct s_data;

// --- DEĞİŞİKLİKLER BURADA BİTİYOR ---
char	*ft_strjoin3(const char *s1, const char *s2, const char *s3);
char	*ft_strndup(const char *s, size_t n);
char	*remove_quotes(const char *str);
char	**create_new_env(char **env, char *new_entry);
void	bubble_sort_env(char **copy, int count);
void	cleanup_and_return(char *var_name, char *var_value);
void	print_env_vars(char **copy, int count);
void	print_sorted_env(char **env);
int		find_env_var(char **env, char *new_entry, int name_len);
int		find_exact_var(char **env, char *arg);
int		is_valid_var(const char *str);
int		add_new_var(t_data *data, char *new_entry);
int		prepare_var_data(char *arg, char *eq, char **var_name, char **var_value);// 82 karakter,  sınırı geçmekte.
int		update_existing_var(t_data *data, char *new_entry, int name_len);
int		ft_isalpha(int c);
int		ft_isalnum(int c);

#endif