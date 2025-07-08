#include "minishell.h"

void debug_parsed_data(t_data *data)
{
    t_command *current;
    int cmd_num = 0;
    
    printf("\n======= DEBUG: Parsed Data =======\n");
    
    // Word array'i göster
    printf("Word Array:\n");
    if (data->word_array)
    {
        for (int i = 0; data->word_array[i]; i++)
            printf("  [%d]: '%s'\n", i, data->word_array[i]);
    }
    else
        printf("  (NULL)\n");
    
    // Token array'i göster
    printf("\nTokens:\n");
    if (data->token && data->word_array)
    {
        for (int i = 0; data->word_array[i]; i++)
        {
            printf("  [%d]: %s -> ", i, data->word_array[i]);
            switch(data->token[i])
            {
                case TOKEN_WORD: printf("WORD\n"); break;
                case TOKEN_PIPE: printf("PIPE\n"); break;
                case TOKEN_REDIRECT_IN: printf("REDIRECT_IN\n"); break;
                case TOKEN_REDIRECT_OUT: printf("REDIRECT_OUT\n"); break;
                case TOKEN_APPEND: printf("APPEND\n"); break;
                case TOKEN_HEREDOC: printf("HEREDOC\n"); break;
                default: printf("UNKNOWN(%d)\n", data->token[i]); break;
            }
        }
    }
    else
        printf("  (NULL)\n");
    
    // Command yapısını göster
    printf("\nParsed Commands:\n");
    current = data->cmd;
    while (current)
    {
        printf("  Command %d:\n", cmd_num++);
        
        // Arguments
        if (current->args)
        {
            printf("    Args: ");
            for (int i = 0; current->args[i]; i++)
                printf("'%s' ", current->args[i]);
            printf("\n");
        }
        else
            printf("    Args: (NULL)\n");
        
        // Redirections
        if (current->input_file)
            printf("    Input: '%s'\n", current->input_file);
        if (current->output_count > 0)
        {
            printf("    Outputs: ");
            for (int i = 0; i < current->output_count; i++)
                printf("'%s'(%s) ", current->output_files[i], 
                    current->append_modes[i] ? "append" : "trunc");
            printf("\n");
        }
        if (current->heredoc_delimiter)
            printf("    Heredoc: '%s'\n", current->heredoc_delimiter);
        
        current = current->next;
        if (current)
            printf("    | (pipe to next)\n");
    }
    
    printf("==================================\n\n");
}


int	main(int argc, char **argv, char **envp)
{
    char	*input;
    t_data	data;

    (void)argc;
    (void)argv;
    while (1)
    {
        input = readline("-->");
        if (!input)
            break ;
        if (!strcmp(input, "exit"))
        {
            free(input);
            break ;
        }
        add_history(input);
        syntax_check(input); //kontrol ekle
        data.env = copy_env(envp);
        data.word_array = split_by_quote(input);
        expander(&data);
        data.token = tokenize_words(data.word_array);
        data.cmd = parse_commands(data.word_array, data.token);
        debug_parsed_data(&data);
        execute_command(&data);
        free(data.token);
        free_word_array(data.word_array);
        free_word_array(data.env);
        free(input);
    }
    return (0);
}