#include "minishell.h"






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
        syntax_check(input); //kontrol ekle
        data.env = copy_env(envp);
        data.word_array = split_by_quote(input);
        expander(&data);
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