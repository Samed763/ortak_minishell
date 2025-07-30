#include "../minishell.h"

int	builtin_cd(char **args)
{
	if (!args[1])
	{
		write(2, "minishell: cd: missing operand\n", 32);
		return (1);
	}
	if (chdir(args[1]) != 0)
	{
		perror("minishell: cd");
		return (1);
	}
	return (0);
}
