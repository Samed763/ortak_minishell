#include "minishell.h"

char *set_var(char *arg, t_data *data)
{
    int		i;
    int		j;
    char	*key;
    char	*value;
    char	*start;
    char	*end;
    char	*temp;
    char	*ret_val;

    i = 0;
    ret_val = NULL;
    while (arg[i])
    {
        if (arg[i] == '$')
        {
            j = 1;
            while (arg[i + j] && arg[i + j] != '/' && 
                   arg[i + j] != ' ' && arg[i + j] != '\t')
                j++;
            key = ft_substr(arg, i + 1, j - 1);
            if (!key)
                return (ft_strdup(arg));
            value = find_value_by_key(data, key);
            if (!value)  // ✅ NULL kontrolü eklendi
            {
                free(key);
                return (ft_strdup(arg));  // Değişken yoksa orijinali döndür
            }
            start = ft_substr(arg, 0, i);
            end = ft_substr(arg, i + j, ft_strlen(arg) - (i + j));
            if (!start || !end)
            {
                free(key);
                free(value);
                free(start);
                free(end);
                return (ft_strdup(arg));
            }
            temp = ft_strjoin(value, end);
            ret_val = ft_strjoin(start, temp);
            
            // Memory temizleme
            free(key);
            free(value);
            free(start);
            free(end);
            free(temp);
            
            return (ret_val);
        }
        i++;
    }
    return (ft_strdup(arg));  // $ yoksa orijinali döndür
}


//her data.word_array da $HOME arıcak ve eğer sonu / yada \0 ise onun yerine env den değer atıcak
void expander(t_data *data)
{
    int i ;
    int j ;

    i =0;
    while (data->word_array[i])
    {
        j=0;
        //$ varsa peşindekini / a kadar env de arıyacak
        while (data->word_array[i][j])
        {
            
            if (data->word_array[i][j] == '$')
            {
                printf("$  bulundu\n");
                data->word_array[i] =ft_strdup(set_var(data->word_array[i],data));
                printf("%s",data->word_array[i]);
                break;
            }
            
            j++;
        }
        
        i++;
    }
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
        syntax_check(input);
        data.env = copy_env(envp);
        data.word_array = split_by_quote(input);
        expander(&data);
        //check_start_var(&data);
        data.token = tokenize_words(data.word_array);
        data.cmd = parse_commands(data.word_array, data.token);
        execute_command(&data);
        free(data.token);
        free_word_array(data.word_array);
        free_word_array(data.env);
        free(input);
    }

    return (0);
}

