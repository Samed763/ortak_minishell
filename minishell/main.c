/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 17:30:12 by sadinc            #+#    #+#             */
/*   Updated: 2025/06/02 14:15:16 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


// Removes surrounding quotes if present (both single and double quotes)
char *remove_surrounding_quotes(const char *str)
{
    int len;
    
    if (!str)
        return NULL;
    
    len = strlen(str);
    
    // Check if string has at least 2 characters and starts/ends with matching quotes
    if (len >= 2 && ((str[0] == '"' && str[len-1] == '"') || 
                     (str[0] == '\'' && str[len-1] == '\'')))
    {
        // Create a new string without the quotes
        char *result = malloc(len - 1); // -2 for quotes +1 for null terminator
        if (!result)
            return NULL;
        
        // Copy the string without the quotes
        strncpy(result, str + 1, len - 2);
        result[len - 2] = '\0';
        
        return result;
    }
    
    // No surrounding quotes, just duplicate the string
    return strdup(str);
}

int	ft_putchar(int c)
{
	return (write(1, &c, 1));
}

int	ft_putstr(char *s)
{
	int	i;

	if (!s)
	{
		if (ft_putstr("(null)") == -1)
			return (-1);
		return (6);
	}
	i = 0;
	while (s[i])
	{
		if (ft_putchar(s[i]) == -1)
			return (-1);
		i++;
	}
	return (i);
}

int ft_env(char ** token,t_data *data)
{
    if (token[1])
        return 1;
    print_env_dictionary(data->env->env_dictionary);
}


//ahhhhhhhhhhhhhhhhhhahhhhhhhhhhhhhhhhhhahhhhhhhhhhhhhhh//
int builtin_foncs(t_data * data,char ** token,int f)
{
    if (f == 1)
        ft_echo(token);
    if (f == 2)
        ft_cd(token,data);
    if (f == 3)
        ft_pwd();
    if (f == 4)
        ft_export(token,data);
    //if (f == 5)
    if (f == 6)
        ft_env(token,data);  

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
        is_dollar(token,&data);
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