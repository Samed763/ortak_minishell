#include "minishell.h"

//[" salg ' "]
//[" 'salg ' "]
//[" salg]
//[salg ' "]


char *trim_quotes_2(char *arg, int free_old, int take_dollar)// 1-> $a" tirank sonda olunca silmiyor
{
    if (!arg)
        return NULL;
    int len = ft_strlen(arg);
    char * new_arg = Malloc(len + 1);
    int i = 0;
    int j = 0;

    while (arg[i])
    {
        if (take_dollar && len > 2 && arg[i] == '$')
            i++;
        if (arg[i] != '\'' && arg[i] != '\"')
            new_arg[j++] = arg[i];
        i++;
    }
    new_arg[j] = '\0';

    if (free_old)
        free(arg);
    return new_arg;
}


int main(int argc, char const *argv[])
{
    // Basic test cases
    char *test1 = "\"hello world\"";
    char *test2 = "'single quotes'";
    char *test3 = "\"mixed'quotes\"here'";
    char *test4 = "no quotes here";
    char *test5 = "\"";  // Empty quotes
    char *test6 = "\"hello";  // Unmatched quote
    char *test7 = "hello'";   // Quote at end
    char *test8 = "'$HOME'";  // Environment variable in quotes
    
    // Additional test cases
    char *test9 = "\"\"\"\"";  // Multiple quotes
    char *test10 = "a'b'c'd'e";  // Quotes in middle
    char *test11 = "quote \"inside\" text";  // Quoted substring
    char *test12 = "\"'nested'\"";  // Nested quotes
    char *test13 = "\"$HOME\"";  // ENV variable in double quotes
    char *test14 = "echo \"$USER is $HOME\"";  // Multiple ENV vars
    char *test15 = "\"   spaces    \"";  // Spaces in quotes
    char *test16 = "'\"double in single\"'";  // Double quotes inside single quotes

    // Testing with take_dollar=1
    char *test17 = "'$PATH'";  // ENV in single quotes with take_dollar
    char *test18 = "\"$USER $HOME\"";  // Multiple ENV in double quotes
    char *test19 = "$VAR";  // Just ENV variable with no quotes
    char *test20 = "text with $VAR and more";  // ENV in middle
    
    printf("=== Basic Tests ===\n");
    printf("Original: [%s] -> Trimmed: [%s]\n", test1, trim_quotes_2(test1, 0, 0));
    printf("Original: [%s] -> Trimmed: [%s]\n", test2, trim_quotes_2(test2, 0, 0));
    printf("Original: [%s] -> Trimmed: [%s]\n", test3, trim_quotes_2(test3, 0, 0));
    printf("Original: [%s] -> Trimmed: [%s]\n", test4, trim_quotes_2(test4, 0, 0));
    printf("Original: [%s] -> Trimmed: [%s]\n", test5, trim_quotes_2(test5, 0, 0));
    printf("Original: [%s] -> Trimmed: [%s]\n", test6, trim_quotes_2(test6, 0, 0));
    printf("Original: [%s] -> Trimmed: [%s]\n", test7, trim_quotes_2(test7, 0, 0));
    printf("Original: [%s] -> Trimmed: [%s]\n", test8, trim_quotes_2(test8, 0, 0));
    
    printf("\n=== Advanced Quote Tests ===\n");
    printf("Original: [%s] -> Trimmed: [%s]\n", test9, trim_quotes_2(test9, 0, 0));
    printf("Original: [%s] -> Trimmed: [%s]\n", test10, trim_quotes_2(test10, 0, 0));
    printf("Original: [%s] -> Trimmed: [%s]\n", test11, trim_quotes_2(test11, 0, 0));
    printf("Original: [%s] -> Trimmed: [%s]\n", test12, trim_quotes_2(test12, 0, 0));
    printf("Original: [%s] -> Trimmed: [%s]\n", test15, trim_quotes_2(test15, 0, 0));
    printf("Original: [%s] -> Trimmed: [%s]\n", test16, trim_quotes_2(test16, 0, 0));
    
    printf("\n=== Dollar Sign Tests (take_dollar=0) ===\n");
    printf("Original: [%s] -> Trimmed: [%s]\n", test13, trim_quotes_2(test13, 0, 0));
    printf("Original: [%s] -> Trimmed: [%s]\n", test14, trim_quotes_2(test14, 0, 0));
    
    printf("\n=== Dollar Sign Tests (take_dollar=1) ===\n");
    printf("Original: [%s] -> Trimmed: [%s]\n", test17, trim_quotes_2(test17, 0, 1));
    printf("Original: [%s] -> Trimmed: [%s]\n", test18, trim_quotes_2(test18, 0, 1));
    printf("Original: [%s] -> Trimmed: [%s]\n", test19, trim_quotes_2(test19, 0, 1));
    printf("Original: [%s] -> Trimmed: [%s]\n", test20, trim_quotes_2(test20, 0, 1));

    return 0;
}
