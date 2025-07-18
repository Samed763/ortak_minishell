#include "built_in.h"

int	prepare_var_data(char *arg, char *eq, char **var_name, char **var_value)
{
	int	name_len;

	name_len = eq - arg;
	*var_name = ft_strndup(arg, name_len);
	*var_value = remove_quotes(eq + 1);
	if (!*var_name || !*var_value)
	{
		cleanup_and_return(*var_name, *var_value);
		return (0);
	}
	return (name_len);
}

int	update_existing_var(t_data *data, char *new_entry, int name_len)
{
	int	found;

	found = find_env_var(data->env, new_entry, name_len);
	if (found >= 0)
	{
		free(data->env[found]);
		data->env[found] = new_entry;
		return (1);
	}
	return (0);
}
