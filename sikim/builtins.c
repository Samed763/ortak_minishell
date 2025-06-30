#include "minishell.h"

extern char **environ;

int is_builtin(char *command)
{
    if (!command)
        return 0;
    
    if (ft_strcmp(command, "echo") == 0 ||
        ft_strcmp(command, "cd") == 0 ||
        ft_strcmp(command, "pwd") == 0 ||
        ft_strcmp(command, "export") == 0 ||
        ft_strcmp(command, "unset") == 0 ||
        ft_strcmp(command, "env") == 0 ||
        ft_strcmp(command, "exit") == 0)
        return 1;
    
    return 0;
}

int execute_builtin(char **args)
{
    if (!args || !args[0])
        return 1;
    
    if (ft_strcmp(args[0], "echo") == 0)
    {
        int i = 1;
        int n_flag = 0;
        
        // Check for -n flag
        if (args[i] && ft_strcmp(args[i], "-n") == 0)
        {
            n_flag = 1;
            i++;
        }
        
        while (args[i])
        {
            printf("%s", args[i]);
            if (args[i + 1])
                printf(" ");
            i++;
        }
        
        if (!n_flag)
            printf("\n");
        return 0;
    }
    else if (ft_strcmp(args[0], "pwd") == 0)
    {
        char *cwd = getcwd(NULL, 0);
        if (cwd)
        {
            printf("%s\n", cwd);
            free(cwd);
            return 0;
        }
        return 1;
    }
    else if (ft_strcmp(args[0], "env") == 0)
    {
        char **env = environ;
        while (env && *env)
        {
            printf("%s\n", *env);
            env++;
        }
        return 0;
    }
    else if (ft_strcmp(args[0], "cd") == 0)
    {
        char *path = args[1];
        if (!path)
            path = getenv("HOME");
        
        if (chdir(path) == -1)
        {
            perror("cd");
            return 1;
        }
        return 0;
    }
    else if (ft_strcmp(args[0], "exit") == 0)
    {
        int exit_code = 0;
        if (args[1])
            exit_code = atoi(args[1]);
        exit(exit_code);
    }
    
    return 1;
}
