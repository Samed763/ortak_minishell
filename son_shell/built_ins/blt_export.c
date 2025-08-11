/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   blt_export.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 11:22:34 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/02 11:22:34 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "built_in.h"

static int	handle_var_without_value(t_data *data, char *arg)
{
	int		found;
	char	*new_entry;
	char	**new_env;

	found = find_exact_var(data->env, arg);
	if (found == -1)
	{
		new_entry = ft_strjoin(arg, "=");
		if (!new_entry)
		{
			perror("export");
			return 1;
		}
		new_env = create_new_env(data->env, new_entry);
		if (!new_env)
		{
			free(new_entry);
			perror("export");
			return 1;
		}
		free(data->env);
		data->env = new_env;
		
	}
	return 0;
}

static int	handle_var_with_value(t_data *data, char *arg, char *eq)
{
	char	*var_name;
	char	*var_value;
	char	*new_entry;
	int		name_len;

	name_len = prepare_var_data(arg, eq, &var_name, &var_value);
	if (!name_len)
		return 1;
	new_entry = ft_strjoin3(var_name, "=", var_value);
	cleanup_and_return(var_name, var_value);
	if (!new_entry)
	{
		perror("export");
		return 1;
	}
	if (!update_existing_var(data, new_entry, name_len))
		add_new_var(data, new_entry);
	return 0;
}

static int	validate_and_process_arg(t_data *data, char *arg)
{
	char	*eq;
	char	*temp_str;

	eq = ft_strchr(arg, '=');
	if (eq)
		temp_str = ft_strndup(arg, eq - arg);
	else
		temp_str = ft_strdup(arg);
	if (!is_valid_var(temp_str))
	{
		write(2, "export: '", 9);
		write(2, arg, ft_strlen(arg));
		write(2, "': not a valid identifier\n", 26);
		free(temp_str);
		return (1);
	}
	free(temp_str);
	if (eq && handle_var_with_value(data, arg, eq))
		return(1);
	if(!eq && handle_var_without_value(data, arg))
		return 1;
	return (0);
}

//başarılı = 0  , başarısız = 1
int	builtin_export(t_data *data) //export başaralı başarız return dönücek şu anda yanlış 
{
	int	i;
	if (!data || !data->cmd || !data->cmd->args || !data->env )
	{
		write(2, "export: internal error\n", 23);
		return 1;
	}
	if (!data->cmd->args[1])
	{
		if(print_sorted_env(data->env))
			return 1;
	}
	i = 1;
	while (data->cmd->args[i])
	{
		if(validate_and_process_arg(data, data->cmd->args[i]))
			return 1;
		i++;
	}
	return 0;
}

