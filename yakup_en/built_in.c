#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "minishell.h"

// ----------- echo -----------
int builtin_echo(char **args)
{
    int i = 1;
    int newline = 1;

    if (args[1] && strcmp(args[1], "-n") == 0) {
        newline = 0;
        i = 2;
    }
    while (args[i]) {
        printf("%s", args[i]);
        if (args[i + 1])
            printf(" ");
        i++;
    }
    if (newline)
        printf("\n");
    return 0;
}

// ----------- cd -----------
int builtin_cd(char **args)
{
    if (!args[1]) {
        fprintf(stderr, "minishell: cd: missing operand\n");
        return 1;
    }
    if (chdir(args[1]) != 0) {
        perror("minishell: cd");
        return 1;
    }
    return 0;
}

// ----------- pwd -----------
int builtin_pwd(void)
{
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)))
        printf("%s\n", cwd);
    else
        perror("pwd");
    return 0;
}













// ----------- export -----------





// Variable name kontrolü
static int is_valid_var(const char *str) {
    if (!str || !(isalpha(str[0]) || str[0] == '_'))
        return 0;
    for (int i = 1; str[i] && str[i] != '='; i++)
        if (!(isalnum(str[i]) || str[i] == '_'))
            return 0;
    return 1;
}

static void print_env_sorted(char **env) {
    // 1. Ortam değişkenlerini kopyala
    int count = 0;
    while (env[count]) count++;
    char **copy = malloc((count + 1) * sizeof(char *));
    for (int i = 0; i < count; i++) copy[i] = strdup(env[i]);
    copy[count] = NULL;

    // 2. Sırala
    for (int i = 0; i < count - 1; i++)
        for (int j = i + 1; j < count; j++)
            if (strcmp(copy[i], copy[j]) > 0) {
                char *tmp = copy[i];
                copy[i] = copy[j];
                copy[j] = tmp;
            }

    // 3. declare -x formatında yaz
    for (int i = 0; i < count; i++) {
        char *eq = strchr(copy[i], '=');
        if (eq) {
            printf("declare -x %.*s=\"%s\"\n", (int)(eq - copy[i]), copy[i], eq + 1);
        } else {
            printf("declare -x %s\n", copy[i]);
        }
        free(copy[i]);
    }
    free(copy);
}

// ENV'de var mı bak, varsa indexini döndür
static int find_env(const char *name, char **env) {
    int nlen = strchr(name, '=') ? strchr(name, '=') - name : strlen(name);
    for (int i = 0; env[i]; i++)
        if (!strncmp(env[i], name, nlen) && env[i][nlen] == '=')
            return i;
    return -1;
}

int builtin_export(char **args, char ***env) {
    if (!args[1]) {
        print_env_sorted(*env);
        return 0;
    }
    for (int i = 1; args[i]; i++) {
        if (!is_valid_var(args[i])) {
            fprintf(stderr, "export: `%s`: not a valid identifier\n", args[i]);
            continue;
        }
        int idx = find_env(args[i], *env);
        if (strchr(args[i], '=')) {
            // Yeni veya güncelle
            if (idx >= 0) {
                free((*env)[idx]);
                (*env)[idx] = strdup(args[i]);
            } else {
                // env'yi büyüt
                int count = 0; while ((*env)[count]) count++;
                char **new_env = malloc((count + 2) * sizeof(char *));
                for (int j = 0; j < count; j++) new_env[j] = (*env)[j];
                new_env[count] = strdup(args[i]);
                new_env[count+1] = NULL;
                free(*env);
                *env = new_env;
            }
        } // Sadece isim gelirse bir şey yapmayabilirsin, ya da ekleyebilirsin.
    }
    return 0;
}














// ----------- unset -----------
int builtin_unset(char **args, char ***env)
{
    if (!args[1])
        return 0;
    int i = 0, j = 0;
    size_t key_len = strlen(args[1]);
    while ((*env)[i]) {
        if (strncmp((*env)[i], args[1], key_len) == 0 && (*env)[i][key_len] == '=') {
            free((*env)[i]);
            // Sondaki ile değiştirip diziyi 1 küçült
            int k = i;
            while ((*env)[k + 1]) {
                (*env)[k] = (*env)[k + 1];
                k++;
            }
            (*env)[k] = NULL;
            continue;
        }
        i++;
    }
    return 0;
}

// ----------- env -----------
int builtin_env(char **env)
{
    for (int i = 0; env[i]; i++)
        printf("%s\n", env[i]);
    return 0;
}

// ----------- exit -----------
int builtin_exit(char **args)
{
    int status = 0;
    if (args[1])
        status = atoi(args[1]);
    printf("exit\n");
    exit(status);
    return 0; // Unreachable, sadece derleyici için
}

// ----------- Built-in Kontrol -----------
int try_builtin(char **args, char **env)
{
    if (!args || !args[0])
        return 0;
    if (strcmp(args[0], "echo") == 0)
        return builtin_echo(args), 1;
    if (strcmp(args[0], "cd") == 0)
        return builtin_cd(args), 1;
    if (strcmp(args[0], "pwd") == 0)
        return builtin_pwd(), 1;
    if (strcmp(args[0], "export") == 0)
        return builtin_export(args, &env), 1;/*try_builtin(cmd->args, &env);
    if (strcmp(args[0], "unset") == 0)
        return builtin_unset(args, env), 1;
    */ 
   if (strcmp(args[0], "env") == 0)
        return builtin_env(env), 1;
    if (strcmp(args[0], "exit") == 0) // ilk başta kontrol et sonradan  burda  leak sorunu 
        return builtin_exit(args), 1;
    return 0;
}