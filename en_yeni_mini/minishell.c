#include "minishell.h"

void	free_word_array(char **array)
{
    int	i;

    if (!array)
        return ;
    i = 0;
    while (array[i])
    {
        free(array[i]);
        i++;
    }
    free(array);
}

static void	remove_processed_vars(t_data *data, int processed_count)
{
    int		i;
    int		j;
    char	**new_array;
    int		remaining_count;

    remaining_count = 0;
    while (data->word_array[remaining_count])
        remaining_count++;
    remaining_count -= processed_count;
    if (remaining_count <= 0)
    {
        data->word_array[0] = NULL;
        return ;
    }
    new_array = malloc(sizeof(char *) * (remaining_count + 1));
    if (!new_array)
        return ;
    i = processed_count;
    j = 0;
    while (data->word_array[i])
    {
        new_array[j] = ft_strdup(data->word_array[i]);
        j++;
        i++;
    }
    new_array[j] = NULL;
    free_word_array(data->word_array);
    data->word_array = new_array;
}

int	find_strchr(char *str, int c)
{
    int	i;

    i = 0;
    while (str[i])
    {
        if (str[i] == c)
            return (i + 1);
        i++;
    }
    return (0);
}

char	*take_var_name(char *var_line)
{
    int		i;
    char	*var_name;

    i = 0;
    while (var_line[i] && var_line[i] != '=')
        i++;
    var_name = malloc(i + 1);
    if (!var_name)
        return (NULL);
    i = 0;
    while (var_line[i] && var_line[i] != '=')
    {
        var_name[i] = var_line[i];
        i++;
    }
    var_name[i] = '\0';
    return (var_name);
}

static void	update_export_command(t_data *data, int var_index, int export_index)
{
    char	*temp;

    temp = data->word_array[export_index];
    data->word_array[export_index] = ft_strdup(data->word_array[var_index]);
    free(temp);
}

void	set_var_wa(t_data *data, int var_index)
{
    char	*var_name;
    char	*current_var_name;
    int		i;

    var_name = take_var_name(data->word_array[var_index]);
    if (!var_name)
        return ;
    i = var_index + 1;
    while (data->word_array[i])
    {
        if (!strcmp(data->word_array[i], "export") && data->word_array[i + 1])
        {
            current_var_name = take_var_name(data->word_array[i + 1]);
            if (current_var_name && !strcmp(var_name, current_var_name))
            {
                update_export_command(data, var_index, i + 1);
                free(current_var_name);
                break ;
            }
            free(current_var_name);
        }
        i++;
    }
    free(var_name);
}

int	is_valid_var_name(char *str, int equals_pos)
{
    int	i;

    i = 0;
    if (equals_pos <= 0)
        return (0);
    while (i < equals_pos - 1)
    {
        if (!((str[i] >= 'a' && str[i] <= 'z') || 
            (str[i] >= 'A' && str[i] <= 'Z') || 
            (str[i] >= '0' && str[i] <= '9') || 
            str[i] == '_'))
            return (0);
        i++;
    }
    return (1);
}

void	check_start_var(t_data *data)
{
    int	count;
    int	equals_index;

    count = 0;
    while (data->word_array[count])
    {
        equals_index = find_strchr(data->word_array[count], '=');
        if (equals_index > 0)
        {
            if (is_valid_var_name(data->word_array[count], equals_index))
                set_var_wa(data, count);
            else
                printf("Invalid variable name\n");
        }
        else
            break ;
        count++;
    }
    if (count > 0)
        remove_processed_vars(data, count);
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
        free(data.token);
        free(input);
    }
    return (0);
}
