#include "minishell.h"

char *find_value_by_key(t_data *data,char *key)
{
    int i = 0;
    char *search;
    search = ft_strjoin(key, "="); // "USER" -> "USER=" şeklinde aransın
    if (!search)
        return NULL;
    while (data->env[i])
    {
        if (!strncmp(data->env[i],search,ft_strlen(search)))
            return ft_strdup(data->env[i] + ft_strlen(search));
        i++;
    }
    return NULL;
}


void pipe_execute(t_data *data,char **splitted_path)
{
    //is_builtin
}

void single_execute(t_data *data,char **splitted_path)
{
    //is_builtin

    
}

void execute_command(t_data* data)
{
    char ** splitted_path;
    splitted_path = ft_split(find_value_by_key(data,"PATH"),':');
    for (size_t i = 0; splitted_path[i]; i++)
    {
        printf("[%ld]: %s",i,splitted_path[i]);
    }
    if (!data->cmd->next)
        pipe_execute(data,splitted_path);
    else    
        single_execute(data,splitted_path);
}

int main(int argc, char **argv,char **envp)
{
    (void)argc;
    (void)argv;
    char	*input;
    t_data	data;

    while (1)
    {
        input = readline("-->");
        if (!input)
            break;
        if (!strcmp(input, "exit"))
        {
            free(input);
            break;
        }

        //------------> test maini <------------
        add_history(input);
        syntax_check(input);
        data.env = copy_env(envp);
        data.word_array = split_by_quote(input);
        check_start_var(&data);
        for (size_t i = 0; data.word_array[i]; i++)
        {
            printf("%s\n",data.word_array[i]);
        }
        //printf("\n%s\n",find_value_by_key(&data,"HOME"));
        data.token = tokenize_words(data.word_array);
        
        /*for (size_t i = 0; data.word_array[i]; i++)
            printf("[%ld] :%s \n", i, data.word_array[i]);
        for (size_t i = 0; data.env[i]; i++)
            printf("[%ld] :%s \n", i, data.env[i]);
        data.env = add_to_env(data.env,input);
        for (size_t i = 0; data.env[i]; i++)
            printf("[%ld] :%s \n", i, data.env[i]);
        print_tokens(data.word_array, data.token);*/

        data.cmd = parse_commands(data.word_array,data.token);
        //print_parsed_commands(data.cmd);
        execute_command(&data);
        free(data.token);
        free(input);
    }
    return (0);
}
