#include "minishell.h"

char ** copy_env(char **envp)
{
    int i = 0;
    while (envp[i])
        i++;
    char **new_env = Malloc(sizeof(char *) * (i + 1));
    i = 0;
    while (envp[i])
    {
        new_env[i] = ft_strdup(envp[i]);
        i++;
    }
    new_env[i] = NULL;
    return new_env;    
}

char ** add_to_env(char **env,char *input) //TODO: belki adı değişebilir export ile eklemek için senin kontrol etmen lazım 
{
    int i = 0;
    while (env[i])
        i++;
    char **new_env = Malloc(sizeof(char *) * (i + 2));
    i=0;
    while (env[i])
    {
        new_env[i] = ft_strdup(env[i]);
        i++;
    }
    new_env[i++] = ft_strdup(input);
    new_env[i] = NULL;
    //free(env); //TODO: char ** freleyen fonksion lazım
    return new_env;
}