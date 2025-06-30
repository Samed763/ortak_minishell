#include "minishell.h"

t_commands *create_command_node(void)
{
    t_commands *cmd = Malloc(sizeof(t_commands));
    cmd->args = NULL;
    cmd->input_file = NULL;
    cmd->output_file = NULL;
    cmd->append_mode = 0;
    cmd->heredoc_delimiter = NULL;
    cmd->next = NULL;
    return cmd;
}

void add_argument_to_command(t_commands *cmd, char *arg)
{
    int count = 0;
    char **new_args;
    
    if (cmd->args)
    {
        while (cmd->args[count])
            count++;
    }
    
    new_args = Malloc(sizeof(char *) * (count + 2));
    
    if (cmd->args)
    {
        for (int i = 0; i < count; i++)
            new_args[i] = cmd->args[i];
        free(cmd->args);
    }
    
    new_args[count] = ft_strdup(arg);
    new_args[count + 1] = NULL;
    cmd->args = new_args;
}

t_commands *parse_commands(char **word_array, int *tokens)
{
    if (!word_array || !tokens)
        return NULL;
    
    t_commands *head = create_command_node();
    t_commands *current = head;
    int i = 0;
    
    while (word_array[i])
    {
        if (tokens[i] == TOKEN_COMMAND || tokens[i] == TOKEN_WORD)
        {
            add_argument_to_command(current, word_array[i]);
        }
        else if (tokens[i] == TOKEN_PIPE)
        {
            current->next = create_command_node();
            current = current->next;
        }
        else if (tokens[i] == TOKEN_REDIRECT_IN)
        {
            i++;
            if (word_array[i])
                current->input_file = ft_strdup(word_array[i]);
        }
        else if (tokens[i] == TOKEN_REDIRECT_OUT)
        {
            i++;
            if (word_array[i])
            {
                current->output_file = ft_strdup(word_array[i]);
                current->append_mode = 0;
            }
        }
        else if (tokens[i] == TOKEN_APPEND)
        {
            i++;
            if (word_array[i])
            {
                current->output_file = ft_strdup(word_array[i]);
                current->append_mode = 1;
            }
        }
        else if (tokens[i] == TOKEN_HEREDOC)
        {
            i++;
            if (word_array[i])
                current->heredoc_delimiter = ft_strdup(word_array[i]);
        }
        i++;
    }
    
    return head;
}

void print_parsed_commands(t_commands *commands)
{
    // Debug function - only enable if DEBUG is defined
    #ifdef DEBUG
    t_commands *current = commands;
    int cmd_num = 0;
    
    printf("\n=== Parsed Commands ===\n");
    while (current)
    {
        printf("Command %d:\n", cmd_num++);
        
        if (current->args)
        {
            printf("  Args: ");
            for (int i = 0; current->args[i]; i++)
                printf("%s ", current->args[i]);
            printf("\n");
        }
        
        if (current->input_file)
            printf("  Input: %s\n", current->input_file);
        if (current->output_file)
            printf("  Output: %s (append: %s)\n", current->output_file, 
                   current->append_mode ? "yes" : "no");
        if (current->heredoc_delimiter)
            printf("  Heredoc: %s\n", current->heredoc_delimiter);
            
        current = current->next;
        if (current)
            printf("  | (pipe to next command)\n");
    }
    printf("========================\n\n");
    #else
    (void)commands;
    #endif
}
