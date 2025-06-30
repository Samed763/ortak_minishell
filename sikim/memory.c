#include "minishell.h"

void free_word_array(char **word_array)
{
    if (!word_array)
        return;
    
    for (int i = 0; word_array[i]; i++)
        free(word_array[i]);
    free(word_array);
}

void free_token_array(int *tokens)
{
    if (tokens)
        free(tokens);
}

void free_commands(t_commands *commands)
{
    while (commands)
    {
        t_commands *next = commands->next;
        
        if (commands->args)
        {
            for (int i = 0; commands->args[i]; i++)
                free(commands->args[i]);
            free(commands->args);
        }
        
        if (commands->input_file)
            free(commands->input_file);
        if (commands->output_file)
            free(commands->output_file);
        if (commands->heredoc_delimiter)
            free(commands->heredoc_delimiter);
            
        free(commands);
        commands = next;
    }
}
