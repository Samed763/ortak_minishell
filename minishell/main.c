/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 17:30:12 by sadinc            #+#    #+#             */
/*   Updated: 2025/05/28 15:24:58 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


char	*ft_strjoin(char const *s1, char const *s2)
{
	int		i;
	int		j;
	char	*mem;

	i = 0;
	j = 0;
	mem = (char *)malloc(ft_strlen(s1) + ft_strlen(s2) + 1);
	if (!mem)
		return (NULL);
	while (s1[i])
	{
		mem[i] = s1[i];
		i++;
	}
	while (s2[j])
	{
		mem[i + j] = s2[j];
		j++;
	}
	mem[i + j] = '\0';
	return (mem);
}

int check_cd_token(char **token)
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

void update_env_value(char ***env_dict, const char *key, const char *value)
{
    int i = 0;
    
    // Find the key in the environment
    while (env_dict[i] != NULL)
    {
        if (strcmp(env_dict[i][0], key) == 0)
        {
            // Free old value and set new value
            free(env_dict[i][1]);
            env_dict[i][1] = strdup(value);
            return;
        }
        i++;
    }
    
    // Key not found (could implement adding it here)
    printf("Warning: Could not update %s, key not found\n", key);
    
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

int builtin_foncs(t_data * data,char ** token,int f)
{
    if (f == 1)
        ft_echo(token);
    if (f == 2)
        ft_cd(token,data);
    
    return 0;
}

void print_tokens(char **tokens)
{
    
    int i = 0;
    

    if (!tokens)
        return;
    
    printf("\n--- Command Tokens ---\n");
    while (tokens[i])
    {
        printf("Token %d: [%s]\n", i, tokens[i]);
        i++;
    }
    printf("--------------------\n\n");

}

int to_do_list(t_data *data, char **envp)
{
    data->env = malloc(sizeof(t_env));
    if (!data->env)
        return (1);
    data->env->env_copy = copy_env(envp);
    if (!data->env->env_copy)
        return (1);
    data->env->path = malloc(9999); /*geçici---

     env yazdır
     print_env(data->env->env_copy);
     printf("\n");
     data->env->path = find_path(data->env->env_copy);
     printf("\n");
     printf("%s",data->env->path);
     printf("\n");*/
    data->env->env_dictionary = make_env_dictionary(envp);
    //print_env_dictionary(data->env->env_dictionary);
    //printf("PATH = %s \n", get_value_by_key(data->env->env_dictionary, "PATH"));
    return (0);
}

int main(int argc, char **argv, char **envp)
{
    (void)argc;
    (void)argv;
    char *input;
    char ** token;
    int built_token;
    pid_t pid;
    t_data data;
    to_do_list(&data, envp);
    char ** ayrık;
    while (1)
    {
        input = readline("\nBir komut girin: ");
        token = lexer(input);
        print_tokens(token);
        
        // Check if input is not empty
        if (input && *input && token && token[0])
        {
            // First check if it's a built-in command
            built_token = is_builtin(token[0]);
            
            if (built_token)
            {
                // Execute built-in commands in the parent process
                builtin_foncs(&data, token, built_token);
            }
            else
            {
                // Only fork for external commands
                pid = fork();
                
                if (pid == 0)
                {
                    // Child process for external commands
                    char *args[] = {"/bin/bash", "-c", input, NULL};
                    execve("/bin/bash", args, NULL);
                    perror("execve failed");
                    exit(1);    
                }
                else if (pid > 0)
                {
                    // Parent process
                    wait(NULL);
                }
                else
                {
                    perror("fork failed");
                }
            }
            
            // Free memory
            free(input);
        }
    }
    
    return 0;
}