#include "../minishell.h"

void ft_pwd(void)
{
    char *pwd;
    pwd = getcwd(NULL, 0);

    if (pwd)
    {
        printf("%s\n", pwd);
        free(pwd);  // bellek sızıntısını önler
    }
    else
    {
        printf("pwd: %s\n", strerror(errno));
    }
}