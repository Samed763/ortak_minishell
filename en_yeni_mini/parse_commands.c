#include "minishell.h"

void	add_argument_to_command(t_command *cmd, char *arg)
{
    char	**new_args;
    int		i;
    int		arg_count;

    // Mevcut argüman sayısını bul
    arg_count = 0;
    if (cmd->args)
    {
        while (cmd->args[arg_count])
            arg_count++;
    }

    // Yeni array oluştur (bir fazla + NULL için)
    new_args = malloc(sizeof(char *) * (arg_count + 2));
    if (!new_args)
        return;

    // Eski argümanları kopyala
    i = 0;
    if (cmd->args)
    {
        while (cmd->args[i])
        {
            new_args[i] = cmd->args[i];
            i++;
        }
        free(cmd->args); // Sadece array'i free et, içeriği değil
    }

    // Yeni argümanı ekle
    new_args[i] = ft_strdup(arg);
    new_args[i + 1] = NULL;

    cmd->args = new_args;
}

t_command	*create_command_node(void)
{
    t_command	*cmd;

    cmd = malloc(sizeof(t_command));
    cmd->args = NULL;
    cmd->input_file = NULL;
    cmd->output_files = NULL;
    cmd->append_modes = NULL;
    cmd->output_count = 0;
    cmd->heredoc_delimiter = NULL;
    cmd->next = NULL;
    return (cmd);
}

void	add_output_to_command(t_command *cmd, char *filename, int append_mode)
{
    char	**new_output_files;
    int		*new_append_modes;
    int		i;

    new_output_files = malloc(sizeof(char *) * (cmd->output_count + 1));
    new_append_modes = malloc(sizeof(int) * (cmd->output_count + 1));
    
    i = 0;
    while (i < cmd->output_count)
    {
        new_output_files[i] = cmd->output_files[i];
        new_append_modes[i] = cmd->append_modes[i];
        i++;
    }
    
    new_output_files[cmd->output_count] = ft_strdup(filename);
    new_append_modes[cmd->output_count] = append_mode;
    
    if (cmd->output_files)
        free(cmd->output_files);
    if (cmd->append_modes)
        free(cmd->append_modes);
    
    cmd->output_files = new_output_files;
    cmd->append_modes = new_append_modes;
    cmd->output_count++;
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
            add_output_to_command(current, word_array[*i], 0);
    }
    else if (tokens[*i] == TOKEN_APPEND)
    {
        (*i)++;
        if (word_array[*i])
            add_output_to_command(current, word_array[*i], 1);
    }
    else if (tokens[*i] == TOKEN_HEREDOC)
    {
        (*i)++;
        if (word_array[*i])
            current->heredoc_delimiter = ft_strdup(word_array[*i]);
    }
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
        if (current->output_count > 0)
        {
            printf("  Outputs: ");
            i = 0;
            while (i < current->output_count)
            {
                printf("%s(%s) ", current->output_files[i], 
                    current->append_modes[i] ? "append" : "truncate");
                i++;
            }
            printf("\n");
        }
        if (current->heredoc_delimiter)
            printf("  Heredoc: %s\n", current->heredoc_delimiter);
        current = current->next;
        if (current)
            printf("  | (pipe to next command)\n");
    }
    printf("========================\n\n");
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

