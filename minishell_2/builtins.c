#include "minishell.h"

int cmd_echo(char **cmd)
{
    int i = 1;
    int newline = 1;

    // Check for -n flag
    if (cmd[1] && ft_strcmp(cmd[1], "-n") == 0)
    {
        newline = 0;
        i = 2;
    }

    // Print arguments
    while (cmd[i])
    {
        printf("%s", cmd[i]);
        if (cmd[i + 1])
            printf(" ");
        i++;
    }

    if (newline)
        printf("\n");

    return 0;
}

int cmd_cd(t_data *data, char **cmd)
{
    char *path;
    char *home;

    if (!cmd[1] || ft_strcmp(cmd[1], "~") == 0)
    {
        // No argument or ~ - go to HOME
        home = get_value_by_key(data->env->env_dictionary, "HOME");
        if (!home)
        {
            printf("cd: HOME not set\n");
            return 1;
        }
        path = home;
    }
    else
    {
        path = cmd[1];
    }

    if (chdir(path) == -1)
    {
        perror("cd");
        return 1;
    }

    // Update PWD environment variable
    char *pwd = getcwd(NULL, 0);
    if (pwd)
    {
        // Find PWD in env and update it
        int i = 0;
        while (data->env->env_dictionary[i])
        {
            if (ft_strcmp(data->env->env_dictionary[i][0], "PWD") == 0)
            {
                free(data->env->env_dictionary[i][1]);
                data->env->env_dictionary[i][1] = ft_strdup(pwd);
                break;
            }
            i++;
        }
        free(pwd);
    }

    return 0;
}

int cmd_pwd()
{
    char *pwd = getcwd(NULL, 0);
    if (!pwd)
    {
        perror("pwd");
        return 1;
    }
    printf("%s\n", pwd);
    free(pwd);
    return 0;
}

int cmd_env(t_data *data)
{
    print_env_dictionary(data->env->env_dictionary);
    return 0;
}

int cmd_export(t_data *data, char **cmd)
{
    if (!cmd[1])
    {
        // Print all environment variables in export format
        print_env_dictionary(data->env->env_dictionary);
        return 0;
    }

    int i = 1;
    while (cmd[i])
    {
        char *equal_pos = strchr(cmd[i], '=');
        if (equal_pos)
        {
            // Variable assignment
            *equal_pos = '\0';
            char *key = cmd[i];
            char *value = equal_pos + 1;

            // Find if variable already exists
            int j = 0;
            int found = 0;
            while (data->env->env_dictionary[j])
            {
                if (ft_strcmp(data->env->env_dictionary[j][0], key) == 0)
                {
                    free(data->env->env_dictionary[j][1]);
                    data->env->env_dictionary[j][1] = ft_strdup(value);
                    found = 1;
                    break;
                }
                j++;
            }

            if (!found)
            {
                // Add new variable - need to expand env array
                int count = 0;
                while (data->env->env_dictionary[count])
                    count++;

                data->env->env_dictionary = realloc(data->env->env_dictionary, 
                    sizeof(char **) * (count + 2));
                data->env->env_dictionary[count] = malloc(sizeof(char *) * 3);
                data->env->env_dictionary[count][0] = ft_strdup(key);
                data->env->env_dictionary[count][1] = ft_strdup(value);
                data->env->env_dictionary[count][2] = NULL;
                data->env->env_dictionary[count + 1] = NULL;
            }

            *equal_pos = '='; // Restore the string
        }
        i++;
    }

    return 0;
}

int cmd_unset(t_data *data, char **cmd)
{
    if (!cmd[1])
        return 0;

    int i = 1;
    while (cmd[i])
    {
        // Find and remove the variable
        int j = 0;
        while (data->env->env_dictionary[j])
        {
            if (ft_strcmp(data->env->env_dictionary[j][0], cmd[i]) == 0)
            {
                // Free the key-value pair
                free(data->env->env_dictionary[j][0]);
                free(data->env->env_dictionary[j][1]);
                free(data->env->env_dictionary[j]);

                // Shift remaining elements
                while (data->env->env_dictionary[j + 1])
                {
                    data->env->env_dictionary[j] = data->env->env_dictionary[j + 1];
                    j++;
                }
                data->env->env_dictionary[j] = NULL;
                break;
            }
            j++;
        }
        i++;
    }

    return 0;
}

int cmd_exit(t_data *data, char **cmd)
{
    int exit_code = data->last_exit_status;

    if (cmd[1])
    {
        // Check if argument is numeric
        char *endptr;
        long num = strtol(cmd[1], &endptr, 10);
        
        if (*endptr != '\0')
        {
            printf("exit: %s: numeric argument required\n", cmd[1]);
            exit_code = 255;
        }
        else
        {
            exit_code = (int)(num % 256); // Keep only lower 8 bits
        }
    }

    printf("exit\n");
    exit(exit_code);
}
