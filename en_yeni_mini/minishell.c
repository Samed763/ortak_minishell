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

        if (syntax_check(input)) // TODO: if koy returna göre işlem yap
            printf("%s doğru;\n",input);
        else
            printf("%s yanlış;\n",input);

        free(input); // Memory leak önleme
    }

    return 0;
}
