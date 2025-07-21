#include "../minishell.h"


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
    cmd->heredoc_lines = NULL;
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
        char	**new_output_files;
    int		*new_append_modes;
    int		i;

    new_output_files = Malloc(sizeof(char *) * (current->output_count + 1));
    new_append_modes = Malloc(sizeof(int) * (current->output_count + 1));
    
    i = 0;
    while (i < current->output_count)
    {
        new_output_files[i] = current->output_files[i];
        new_append_modes[i] = current->append_modes[i];
        i++;
    }
    new_output_files[current->output_count] = ft_strdup(filename);
    new_append_modes[current->output_count] = append_mode;
    if (current->output_files)
        free(current->output_files);
    if (current->append_modes)
        free(current->append_modes);
    current->output_files = new_output_files;
    current->append_modes = new_append_modes;
    current->output_count++;
}

static void handle_redirections(t_data *data,t_command *current,int *i)
{
    if(data->token[*i] == TOKEN_REDIRECT_IN)
    {
        (*i)++;  // ✅ Önce artır
        if (data->word_array[*i])  // ✅ Sonra kontrol et
            add_input_to_command(current,data->word_array[*i]);
    }
    else if(data->token[*i] == TOKEN_REDIRECT_OUT)
    {
        (*i)++;  // ✅ Önce artır
        if (data->word_array[*i])  // ✅ Sonra kontrol et
            add_output_to_command(current,data->word_array[*i],0);
    }
    else if(data->token[*i] == TOKEN_APPEND)
    {
        (*i)++;  // ✅ Önce artır
        if (data->word_array[*i])  // ✅ Sonra kontrol et
            add_output_to_command(current,data->word_array[*i],1);
    }
    else if(data->token[*i] == TOKEN_HEREDOC)
    {
        (*i)++;  // ✅ Önce artır
        if (data->word_array[*i])  // ✅ Sonra kontrol et
        {
            current->heredoc_delimiter = ft_strdup(data->word_array[*i]);
            handle_heredoc(data,current);
        }
    }
}

t_command *parser(t_data *data)
{
    t_command *head;
    t_command *current;

    head = create_list();
    current = head;
    int i = 0;
    while (data->word_array[i])
    {
        if (data->token[i] == TOKEN_WORD)
            add_argument_to_command(current,data->word_array[i]);
        else if (data->token[i] == TOKEN_PIPE)
        {
            current->next = create_list();
            current = current->next;
        }
        else
            handle_redirections(data,current,&i);
        i++;
    }
    return head;
}
