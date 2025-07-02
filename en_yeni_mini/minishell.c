#include "minishell.h"


int main(int argc, char **argv)
{
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
        add_history(input);
        syntax_check(input);
        data.word_array = split_by_quote(input);
        data.token = tokenize_words(data.word_array);
        
        for (size_t i = 0; data.word_array[i]; i++)
            printf("[%ld] :%s \n", i, data.word_array[i]);
        
        print_tokens(data.word_array, data.token);
        data.cmd = parse_commands(data.word_array,data.token);
        print_parsed_commands(data.cmd);
        free(data.token);
        free(input);
    }
    return (0);
}
