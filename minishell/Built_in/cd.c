#include "../minishell.h"

static int check_cd_token(char **token)
{
    int i = 0;

    while (token[i])
        i++;

    if (i > 2)
    {
        printf("cd: too many arguments\n");
        return 1;
    }

    printf("hata yok");
    return 0;
}

int ft_cd(char ** token,t_data *data)
{
    char old_dir[1024];
    char current_dir[1024];
    char *path;
    
    // Save current directory
    if (getcwd(old_dir, sizeof(old_dir)) == NULL)
    {
        perror("cd: getcwd error");
        return 1;
    }
    
    // Check for valid arguments
    if (check_cd_token(token))
        return 1;
    
    // No arguments or ~ means go to HOME
    if (!token[1] || strcmp(token[1], "~") == 0)
    {
        path = get_value_by_key(data->env->env_dictionary, "HOME");
        if (!path)
        {
            printf("cd: HOME not set\n");
            return 1;
        }
    }
    // Handle previous directory with "-"
    else if (strcmp(token[1], "-") == 0)
    {
        path = get_value_by_key(data->env->env_dictionary, "OLDPWD");
        if (!path)
        {
            printf("cd: OLDPWD not set\n");
            return 1;
        }
        printf("%s\n", path);  // Print directory when using cd -
    }
    else
    {
        path = token[1];
    }
    
    // Change directory
    if (chdir(path) != 0)
    {
        perror("cd");
        return 1;
    }
    
    // Get new current directory
    if (getcwd(current_dir, sizeof(current_dir)) != NULL)
    {
        // Here you would update PWD and OLDPWD in environment
        // This requires implementing a function to update env variables
        // For now, just print the new directory
        printf("Changed to: %s\n", current_dir);
    }
    // In the ft_cd function, after getting current_dir:
    update_env_value(data->env->env_dictionary, "OLDPWD", old_dir);
    update_env_value(data->env->env_dictionary, "PWD", current_dir);
    return 0;
}