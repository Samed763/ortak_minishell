#include "minishell.h"

char * set_var(char * arg,t_data *data)
{
    int i = 0;
    int j = 0;
    char *key;
    while (arg[i])
    {
        if(arg[i] == '$')
        {
            j = 0;
            while (arg[i + j] != '/' && arg[i + j] != '\0')
                j++;
            printf("i : %d, j : %d",i,j);
            key = ft_substr(arg,i+1,i-j);
            printf("%s",key);
            printf("%s",find_value_by_key(data,key));
        }
        i++;
    }
    
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

