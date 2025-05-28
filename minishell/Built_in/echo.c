#include "../minishell.h"

static int check_echo_flag(char *flag)
{
    int i = 1;
    
    if (flag[0] != '-')
        return 0;
        
    while (flag[i])
    {
        if (flag[i] != 'n')
            return 0;
        i++;
    }
    
    // If we made it here, the string is "-" followed by zero or more 'n's
    return (i > 1); // Return 1 only if there was at least one 'n'
}

int ft_echo(char **token)
{
    int i = 1;
    int n_flag = 0;
    
    // Process all -n flags
    while (token[i] && check_echo_flag(token[i]))
    {
        n_flag = 1;
        i++;
    }
        printf("\n");
    
    // Print all arguments
    while (token[i])
    {
        printf("%s", token[i]);
        if (token[i + 1])
            printf(" ");
        i++;
    }
    
    // Print newline if -n flag is not present
    if (!n_flag)
        printf("\n");
    
    return 0;
}