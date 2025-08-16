/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_in.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 14:11:52 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/16 14:15:53 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILT_IN_H
# define BUILT_IN_H

# include "../minishell.h"

char	*ft_strjoin3(char *s1, char *s2, char *s3);
char	**create_new_env(char **env, char *new_entry);
void	bubble_sort_env(char **copy, int count);
void	cleanup_and_return(char *var_name, char *var_value);
void	print_env_vars(char **copy, int count);
int		print_sorted_env(char **env);
int		find_exact_var(char **env, char *arg);
int		add_new_var(t_data *data, char *new_entry);
int		prepare_var_data(char *arg, char *eq, char **var_name,
			char **var_value);
int		update_existing_var(t_data *data, char *new_entry, int name_len);
int		ft_isalpha(int c);
int		ft_isalnum(int c);
void	ft_putstr_fd(char *s, int fd);
void	ft_putendl_fd(char *s, int fd);

#endif