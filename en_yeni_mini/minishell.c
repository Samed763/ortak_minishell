#include "minishell.h"

int g_signal_received = 0;


void free_command_list(t_command *head)
{
    t_command *current = head;
    t_command *next;
    
    while (current)
    {
        next = current->next;
        
        // Args temizle
        if (current->args)
            free_word_array(current->args);
        
        // Input files temizle
        if (current->input_files)
            free_word_array(current->input_files);
        
        // Output files temizle
        if (current->output_files)
            free_word_array(current->output_files);
        
        // Append modes temizle
        if (current->append_modes)
            free(current->append_modes);
        
        // Heredoc delimiter temizle
        if (current->heredoc_delimiter)
            free(current->heredoc_delimiter);
        
        // Heredoc lines temizle
        if (current->heredoc_lines)
            free_heredoc_lines(current->heredoc_lines);
        
        free(current);
        current = next;
    }
}

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
           // Redirections
        if (current->input_files && current->input_files[0])
            printf("    Input: '%s'\n", current->input_files[0]);
    
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

void signal_handler(int sig)
{
    if (sig == SIGINT)
    {
        g_signal_received = SIGINT;
        write(STDOUT_FILENO, "\n", 1);
        rl_on_new_line();
        rl_replace_line("", 0);
        rl_redisplay();
    }
}

void setup_signals(void)
{
    struct sigaction sa;
    
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;    
    
    sigaction(SIGINT, &sa, NULL);
    signal(SIGQUIT, SIG_IGN);
}

int main(int argc, char **argv, char **envp)
{
    char *input;
    t_data data;
    
    setup_signals();
    (void)argc;
    (void)argv;
    
    // ENV'i sadece bir kez copy et
    data.env = copy_env(envp);
    data.exit_value = 0;
    
    while (1)
    {
        input = readline("-->");
        
        if (!input)
        {
            printf("exit\n");
            break;
        }
        
        // Signal check
        if (g_signal_received == SIGINT)
        {
            g_signal_received = 0;
            free(input);
            continue;
        }
        
        if (input[0] == '\0')
        {
            free(input);
            continue;
        }
        
        if (!strcmp(input, "exit"))
        {
            free(input);
            break;
        }
        
        add_history(input);
        
        // Syntax check
        if (!syntax_check(input))
        {
            printf("syntax error\n");
            free(input);
            continue;
        }
        
        data.word_array = split_by_quote(input);
        if (!data.word_array)
        {
            free(input);
            continue;
        }
        
        expander(&data);
        data.token = tokenize_words(data.word_array);
        data.cmd = parse_commands(data.word_array, data.token, &data);
        
        debug_parsed_data(&data);
        printf("\n");
        
        execute_command(&data);
        printf("Exit_value :%d\n", data.exit_value);
        
        // Cleanup
        if (data.cmd)
            free_command_list(data.cmd);
        if (data.token)
            free(data.token);
        if (data.word_array)
            free_word_array(data.word_array);
        
        free(input);
    }
    
    // Çıkıştan önce env'i temizle
    free_word_array(data.env);
    return (0);
}