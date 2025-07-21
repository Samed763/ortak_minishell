#include "built_in.h"

static int	is_valid_n_flag(char *str)
{
	int	i;

	if (!str || str[0] != '-')
		return (0);
	i = 1;
	while (str[i])
	{
		if (str[i] != 'n')
			return (0);
		i++;
	}
	return (i > 1);
}

static int	get_start_index(char **args)
{
	int	i;

	i = 1;
	while (args[i] && is_valid_n_flag(args[i]))
		i++;
	return (i);
}

static int	should_print_newline(char **args)
{
	int	i;

	i = 1;
	while (args[i] && is_valid_n_flag(args[i]))
	{
		return (0);
		i++;
	}
	return (1);
}

static char	*remove_quotes_echo(char *str)
{
	char	*result;
	int		len;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	if (len < 2)
		return (ft_strdup(str));
	if ((str[0] == '"' && str[len - 1] == '"') || 
		(str[0] == '\'' && str[len - 1] == '\''))
	{
		result = ft_strndup(str + 1, len - 2);
		return (result);
	}
	return (ft_strdup(str));
}

// Bu fonksiyonu güncelliyoruz
int	builtin_echo(char **args)
{
	int		i;
	int		newline;
	char	*clean_str;

	if (!args)
		return (1);
	newline = 1;
	i = 1;
	if (args[i] && is_valid_n_flag(args[i]))
	{
		newline = 0;
		while (args[i] && is_valid_n_flag(args[i]))
			i++;
	}
	while (args[i])
	{
		// 1. Argümandaki tırnakları kaldır
		clean_str = remove_quotes_echo(args[i]);
		if (clean_str)
		{
			// 2. Temizlenmiş string'i 'write' ile yazdır
			write(STDOUT_FILENO, clean_str, ft_strlen(clean_str));
			// 3. Bellek sızıntısını önlemek için ayrılan alanı serbest bırak
			free(clean_str);
		}
		if (args[i + 1])
			write(STDOUT_FILENO, " ", 1);
		i++;
	}
	if (newline)
		write(STDOUT_FILENO, "\n", 1);
	return (0);
}