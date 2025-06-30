#include "minishell.h"

void print_d_pointer(char **args)
{
    int i = 0;

    if (!args)
    {
        printf("args is NULL\n");
        return;
    }
    
    while (args[i])
    {
        printf("arg[%d] = %s \n", i, args[i]);
        i++;
    }
}


int main(int argc, char **argv, char **envp)
{
    (void)argc;
    (void)argv;
    (void)envp;

    t_data data;
    char *input;

    while (1)
    {
        input = readline("minishell$ ");
        data.args = lexer(input);
        printf("=== Lexer Output ===\n");
        print_d_pointer(data.args);
        
        
        free(input);
    }
    return 0;
}