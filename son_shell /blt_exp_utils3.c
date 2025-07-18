#include "built_in.h"

void	print_env_vars(char **copy, int count)
{
	int		i;
	char	*eq;

	i = 0;
	while (i < count)
	{
		eq = ft_strchr(copy[i], '=');
		if (eq)
			printf("declare -x %.*s=\"%s\"\n", (int)(eq - copy[i]), 
				copy[i], eq + 1);
		else
			printf("declare -x %s\n", copy[i]);
		free(copy[i]);
		i++;
	}
}

void	cleanup_and_return(char *var_name, char *var_value)
{
	free(var_name);
	free(var_value);
}

void	print_sorted_env(char **env)
{
	int		count;
	int		i;
	char	**copy;

	if (!env)
		return ;
	count = 0;
	while (env[count])
		count++;
	copy = malloc((count + 1) * sizeof(char *));
	if (!copy)
		return ;
	i = 0;
	while (i < count)
	{
		copy[i] = ft_strdup(env[i]);
		i++;
	}
	copy[count] = NULL;
	bubble_sort_env(copy, count);
	print_env_vars(copy, count);
	free(copy);
}

int	add_new_var(t_data *data, char *new_entry)
{
	char	**new_env;

	new_env = create_new_env(data->env, new_entry);
	if (!new_env)
	{
		free(new_entry);
		perror("export");
		return (0);
	}
	free(data->env);
	data->env = new_env;
	return (1);
}

char	*remove_quotes(const char *str)
{
	size_t	len;
	char	*result;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	if (len < 2)
		return (ft_strdup(str));
	if ((str[0] == '"' && str[len - 1] == '"') ||
		(str[0] == '\'' && str[len - 1] == '\''))
	{
		result = ft_strndup(str + 1, len - 2);
		if (!result)
			return (NULL);
		return (result);
	}
	return (ft_strdup(str));
}
