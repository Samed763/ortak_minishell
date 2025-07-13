#include "minishell.h"

static t_command *create_list(void)
{
    t_command *cmd;

    cmd = malloc(sizeof(t_command));
    cmd->args = NULL;
    cmd->input_files = NULL;
    cmd->output_files = NULL;
    cmd->append_modes = NULL;
    cmd->output_count = 0;
    cmd->heredoc_delimiter = NULL;
    cmd->next = NULL;
    return(cmd);

}

static void add_argument_to_command(t_command *cmd,char *word)
{
    char **new_args;
    
    int i = 0;
    int j = 0;
    
    while (cmd->args != NULL && cmd->args[i])
        i++;
    new_args = Malloc(sizeof(char *) * (i + 2));

    while (cmd->args != NULL && cmd->args[j])
    {
        new_args[j] = cmd->args[j];
        j++;
    }
    if (cmd->args != NULL)
        free(cmd->args);

    new_args[j] = ft_strdup(word);
    new_args[j + 1] = NULL;

    cmd->args = new_args;
}

static void	add_input_to_command(t_command *current, char *filename)
{
    if (current->input_files)
        free(current->input_files);
    current->input_files = ft_strdup(filename);
}

static void	add_output_to_command(t_command *current, char *filename, int append_mode)
{
    //BURADA KALDIM
}

static void handle_redirections(t_data *data,t_command *current,int *i)
{
    if(data->token[*i] == TOKEN_REDIRECT_IN)
    {
        if (data->word_array[*++i])
            add_input_to_command(current,data->word_array[*i]);
    }
    else if(data->token[*i] == TOKEN_REDIRECT_OUT)
    {
        if (data->word_array[*++i])
            add_output_to_command(current,data->word_array[*i],0);
    }
}

t_command parser(t_data *data)
{
    t_command *head;
    t_command *current;

    head = create_list();
    current = head;
    int i = 0;
    while (data->word_array[i])
    {
        if (data->token == TOKEN_WORD)
            add_argument_to_command(current,data->word_array[i]);
        else if (data->token == TOKEN_PIPE)
        {
            current->next = create_list();
            current = current->next;
        }
        else
            handle_redirections(data,current,&i);
        i++;
    }
    

    
}

// main fonksiyonunu güncelle
int main(int argc, char **argv, char **envp)
{
    char *line;
    t_data data;

    while (1)
    {
        line = readline("minishell$ ");
        if (!line)
        {
            return 0;
        }
        
        if (syntax_check(line))
        {
            printf("syntax error\n");
            free(line);
            continue;
        }
        
        lexer(line,&data);
        //expander mantıklı olan bu sanki
        parser(&data);
        // Debug çıktısı
        print_tokens_with_types(data.word_array,data.token);
        
        // Memory cleanup
        free_word_array(data.word_array);
        free(line);
    }

    return 0;
}