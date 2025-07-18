#include "minishell.h"

int builtin_exit(char **args, int exit_code)
{
	int status;

	printf("exit\n");
	// Eğer "exit" dışında argüman yoksa, son komutun çıkış koduyla çık
	if (!args[1])
		exit(exit_code);

	// Argümanın sayısal olup olmadığını kontrol et
	// (Basit bir kontrol, daha iyisi yapılabilir)
	for (int i = 0; args[1][i]; i++)
	{
		if (i == 0 && (args[1][i] == '-' || args[1][i] == '+'))
			continue;
		if (args[1][i] < '0' || args[1][i] > '9')
		{
			fprintf(stderr, "minishell: exit: %s: numeric argument required\n", args[1]);
			exit(255); // bash bu durumda 255 ile çıkar
		}
	}

	// Eğer ikiden fazla argüman varsa (örn: exit 1 2)
	if (args[2])
	{
		fprintf(stderr, "minishell: exit: too many arguments\n");
		return (1); // Hata ver ama shell'den çıkma
	}

	status = atoi(args[1]);
	exit(status);
	return (0); // Bu satıra asla ulaşılmaz
}
