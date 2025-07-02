#include "minishell.h"

int find_strchr(char *str,int c)
{
    int i = 0;
    int find = 0;
    while (str[i])
    {
        if (str[i] == c)
            find = 1;
        i++;
    }
    if (find == 1)
        return i;
    return 0;
    
}

void check_start_var(t_data *data)  // tam bitmedi
{   
    int count = 0;
    int equals_index;
    while (data->word_array[count]) //hem kaç değişken olduğunu sayıyor hemde doğrumu çdiye kontrol ediyor
    {
        equals_index = find_strchr(data->word_array[count],'=');
        if (equals_index > 0)
        {
            //is_valid_var_name; =den öncesi değiken ismi olabilirmi ?
            if ( is_valid_after_pipe(data->word_array[count][equals_index - 1]))
                printf("valid name");
            else  //debuglar değişmeli
                 printf("sıçış");
        }   
        else
            break;
        count++;
    }
    printf("count : %d\n",count);
    
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
