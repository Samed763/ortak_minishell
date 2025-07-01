#include "minishell.h"



int main(int argc, char **argv)
{
    char *input;
    t_data data;

    while (1)
    {
        input = readline("-->");

        if (!input) // Ctrl+D kontrolü
            break;

        if (!strcmp(input, "exit")) // boş kontrolu yapıcam
        {
            free(input);
            break;
        }

        add_history(input);
        syntax_check(input);// TODO: if koy returna göre işlem yap
        // Process input
        data.word_array = split_by_quote(input);


        for (size_t i = 0; data.word_array[i]; i++)
            printf("[%ld] :%s \n",i , data.word_array[i]);
        

            
        free(input); // Memory leak önleme
    }

    return 0;
}
