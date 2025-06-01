/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 17:30:12 by sadinc            #+#    #+#             */
/*   Updated: 2025/05/31 19:01:57 by sadinc           ###   ########.fr       */
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

void fill_export(char **token, t_export *st_export)
{
    int equals_pos = -1;
    int i = 0;
    
    // Check if token[1] exists
    if (!token[1]) {
        st_export->key = NULL;
        st_export->value = NULL;
        return;
    }
    
    // Find the '=' character
    while (token[1][i] != '\0') {
        if (token[1][i] == '=') {
            equals_pos = i;
            break;
        }
        i++;
    }
    
    // Handle cases based on whether '=' was found
    if (equals_pos == -1) {
        // No '=' found, only a variable name to export
        st_export->key = strdup(token[1]);
        st_export->value = ft_strdup("");  // Empty value
    } else {
        // '=' found, extract key and value
        st_export->key = malloc(equals_pos + 1);
        if (st_export->key) {
            strncpy(st_export->key, token[1], equals_pos);
            st_export->key[equals_pos] = '\0';
        }
        
        // Get value after '=' and remove surrounding quotes if present
        char *raw_value = token[1] + equals_pos + 1;
        st_export->value = remove_surrounding_quotes(raw_value);
    }
}



char ***sort_env(char ***env_dic)
{
    int i, j, count = 0;
    char **temp;
    
    // Count environment variables
    while (env_dic[count] != NULL)
        count++;
    
    // Create a copy of the array to sort
    char ***sorted = malloc(sizeof(char **) * (count + 1));
    if (!sorted)
        return NULL;
    
    // Copy pointers to sorted array
    for (i = 0; i < count; i++)
        sorted[i] = env_dic[i];
    
    // Null terminate
    sorted[count] = NULL;
    
    // Simple bubble sort by key name
    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - i - 1; j++) {
            // Compare keys and swap if needed
            if (strcmp(sorted[j][0], sorted[j + 1][0]) > 0) {
                temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }
    
    return sorted;
}


int ft_export(char **token, t_data *data)
{
    int i;
    int env_count = 0;
    char ***display_array;
    
    // If no arguments, print all environment variables in declared format
    if (!token[1]) {
        display_array = sort_env(data->env->env_dictionary);
        if (display_array) {
            print_export(display_array);
            free(display_array); // Free the sorted array (but not its contents)
        }
    } else {
        // Process each argument (token[1], token[2], etc.)
        for (i = 1; token[i] != NULL; i++) {
            // Allocate memory for st_export
            t_export *st_export = malloc(sizeof(t_export));
            if (!st_export)
                return 1;
            
            // Initialize to NULL to prevent issues if fill_export fails
            st_export->key = NULL;
            st_export->value = NULL;
            
            // Create a temporary token array with just this argument
            char *temp_tokens[3];  // [command, arg, NULL]
            temp_tokens[0] = token[0];
            temp_tokens[1] = token[i];
            temp_tokens[2] = NULL;
            
            // Fill export values for this argument
            fill_export(temp_tokens, st_export);
            
            // Add to environment dictionary
            data->env->env_dictionary = add_env_variable(data->env->env_dictionary, st_export);
            
            // Free the temporary structure (but not its contents which are now part of env_dict)
            free(st_export);
        }
    }
    
    return 0;
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