#include "minishell.h"

int	builtin_cd(char **args)
{
	if (!args[1])
	{
		fprintf(stderr, "minishell: cd: missing operand\n");
		return (1);
	}
	if (chdir(args[1]) != 0)
	{
		perror("minishell: cd");
		return (1);
	}
	return (0);
}
