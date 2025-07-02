#include "minishell.h"


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
