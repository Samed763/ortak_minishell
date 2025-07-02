#include "minishell.h"

t_command	*create_command_node(void)
{
    t_command	*cmd;

    cmd = malloc(sizeof(t_command));
    cmd->args = NULL;
    cmd->input_file = NULL;
    cmd->output_file = NULL;
    cmd->append_mode = 0;
    cmd->heredoc_delimiter = NULL;
    cmd->next = NULL;
    return (cmd);
}

void	add_argument_to_command(t_command *cmd, char *arg)
{
    int		count;
    char	**new_args;
    int		i;

    count = 0;
    if (cmd->args)
    {
        while (cmd->args[count])
            count++;
    }
    new_args = malloc(sizeof(char *) * (count + 2));
    if (cmd->args)
    {
        i = 0;
        while (i < count)
        {
            new_args[i] = cmd->args[i];
            i++;
        }
        free(cmd->args);
    }
    new_args[count] = ft_strdup(arg);
    new_args[count + 1] = NULL;
    cmd->args = new_args;
}

static void	handle_redirections(t_command *current, char **word_array, 
            int *tokens, int *i)
{
    if (tokens[*i] == TOKEN_REDIRECT_IN)
    {
        (*i)++;
        if (word_array[*i])
            current->input_file = ft_strdup(word_array[*i]);
    }
    else if (tokens[*i] == TOKEN_REDIRECT_OUT)
    {
        (*i)++;
        if (word_array[*i])
        {
            current->output_file = ft_strdup(word_array[*i]);
            current->append_mode = 0;
        }
    }
    else if (tokens[*i] == TOKEN_APPEND)
    {
        (*i)++;
        if (word_array[*i])
        {
            current->output_file = ft_strdup(word_array[*i]);
            current->append_mode = 1;
        }
    }
    else if (tokens[*i] == TOKEN_HEREDOC)
    {
        (*i)++;
        if (word_array[*i])
            current->heredoc_delimiter = ft_strdup(word_array[*i]);
    }
}

t_command	*parse_commands(char **word_array, int *tokens)
{
    t_command	*head;
    t_command	*current;
    int			i;

    if (!word_array || !tokens)
        return (NULL);
    head = create_command_node();
    current = head;
    i = 0;
    while (word_array[i])
    {
        if (tokens[i] == TOKEN_WORD)
            add_argument_to_command(current, word_array[i]);
        else if (tokens[i] == TOKEN_PIPE)
        {
            current->next = create_command_node();
            current = current->next;
        }
        else
            handle_redirections(current, word_array, tokens, &i);
        i++;
    }
    return (head);
}

void	print_parsed_commands(t_command *commands)
{
    t_command	*current;
    int			cmd_num;
    int			i;

    current = commands;
    cmd_num = 0;
    printf("\n=== Parsed Commands ===\n");
    while (current)
    {
        printf("Command %d:\n", cmd_num++);
        if (current->args)
        {
            printf("  Args: ");
            i = 0;
            while (current->args[i])
            {
                printf("%s ", current->args[i]);
                i++;
            }
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
}
