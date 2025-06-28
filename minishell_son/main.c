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

int get_total_args(char **args)
{
    int count = 0;
    while (args[count])
        count++;
    return count;
}

void init_parser(t_data *data)
{
    data->parsed = Malloc(sizeof(t_parsed));
    data->parsed->cmd_count = 1;
    data->parsed->input_file = NULL;
    data->parsed->output_file = NULL;
    data->parsed->append = 0;
    data->parsed->heredoc = 0;
    data->parsed->heredoc_limiter = NULL;
    data->parsed->heredoc_content = NULL;
    data->parsed->commands.count = 0;
}

void set_cmd_count(t_data *data)
{
    int i = 0;
    data->parsed->cmd_count = 1;
    
    while (data->args[i])
    {
        if (ft_strcmp(data->args[i], "|") == 0)
            data->parsed->cmd_count++;
        i++;
    }
}

int is_redirection(char *token)
{
    return (ft_strcmp(token, "<") == 0 || 
            ft_strcmp(token, ">") == 0 || 
            ft_strcmp(token, ">>") == 0 || 
            ft_strcmp(token, "<<") == 0);
}

void handle_redirection(t_data *data, char *operator, char *file)
{
    if (ft_strcmp(operator, "<") == 0)
        data->parsed->input_file = ft_strdup(file);
    else if (ft_strcmp(operator, ">") == 0)
    {
        data->parsed->output_file = ft_strdup(file);
        data->parsed->append = 0;
    }
    else if (ft_strcmp(operator, ">>") == 0)
    {
        data->parsed->output_file = ft_strdup(file);
        data->parsed->append = 1;
    }
    else if (ft_strcmp(operator, "<<") == 0)
    {
        data->parsed->heredoc = 1;
        data->parsed->heredoc_limiter = ft_strdup(file);
    }
}

void parse_single_command(t_data *data, int cmd_index, int start, int end)
{
    int arg_count = 0;
    int j = start;
    
    // Arg sayısını hesapla (redirection'ları çıkar)
    while (j < end)
    {
        if (is_redirection(data->args[j]))
        {
            handle_redirection(data, data->args[j], data->args[j + 1]);
            j += 2; // redirection + file
        }
        else
        {
            arg_count++;
            j++;
        }
    }
    
    // Bu komut için memory allocate et
    data->parsed->commands.arg[cmd_index] = Malloc(sizeof(char *) * (arg_count + 1));
    
    // Args'ları doldur
    j = start;
    int arg_index = 0;
    while (j < end)
    {
        if (is_redirection(data->args[j]))
        {
            j += 2;
        }
        else
        {
            data->parsed->commands.arg[cmd_index][arg_index] = ft_strdup(data->args[j]);
            arg_index++;
            j++;
        }
    }
    data->parsed->commands.arg[cmd_index][arg_index] = NULL;
}

void parser(t_data *data)
{
    init_parser(data);
    set_cmd_count(data);
    
    // arg array'ini allocate et (cmd_count kadar char** pointer)
    data->parsed->commands.arg = Malloc(sizeof(char **) * data->parsed->cmd_count);
    
    int i = 0;
    int cmd_index = 0;
    int arg_start = 0;
    
    // Her komut için args'ları ayır
    while (i <= get_total_args(data->args))
    {
        if (data->args[i] == NULL || ft_strcmp(data->args[i], "|") == 0)
        {
            // Komut bitti, parse et
            parse_single_command(data, cmd_index, arg_start, i);
            cmd_index++;
            arg_start = i + 1;
        }
        i++;
    }
    
    data->parsed->commands.count = data->parsed->cmd_count;
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
        if (!input)
            break;
        if (*input)
            add_history(input);
            
        data.args = lexer(input);
        printf("=== Lexer Output ===\n");
        print_d_pointer(data.args);
        
        parser(&data);
        printf("=== Parser Output ===\n");
        for (int i = 0; i < data.parsed->cmd_count; i++)
        {
            printf("Command %d:\n", i);
            print_d_pointer(data.parsed->commands.arg[i]);
        }
        
        free(input);
    }
    return 0;
}