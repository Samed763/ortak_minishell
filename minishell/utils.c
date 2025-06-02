#include "minishell.h"

void print_env_dictionary(char ***env)
{
    int i = 0;
    while (env[i] != NULL)
    {
        //[i][0] == KEY
        //[i][1] == Value
        if (env[i][0] && env[i][1])                   // key ve value varsa
            printf("%s=%s \n", env[i][0], env[i][1]); // = sonradan ekleniyor
        i += 1;
    }
}

void print_export(char ***env)
{
    int i = 0;
    while (env[i] != NULL)
    {
        if (env[i][0]) // Key exists
        {
            printf("declare -x %s", env[i][0]);
            
            // Check if value exists and is not empty
            if (env[i][1] && env[i][1][0] != '\0') 
            {
                // Print with a single set of quotes
                printf("=\"%s\"", env[i][1]);
            }
            
            printf("\n");
        }
        i++;
    }
}
char *get_value_by_key(char ***env_dict, const char *key)
{
    int i = 0;
    while (env_dict[i] != NULL)
    {
        if (env_dict[i][0] && env_dict[i][1])
        {
            // strcmp ile karşılaştır
            int j = 0;
            while (key[j] && env_dict[i][0][j] && key[j] == env_dict[i][0][j])
                j++;
            if (key[j] == '\0' && env_dict[i][0][j] == '\0')
                return env_dict[i][1]; // key bulundu, value döndür
        }
        i++;
    }
    return NULL; // bulunamadı
}

char **copy_env(char **envp) // geçici
{
    int i;
    char **new_env;

    i = 0;
    while (envp[i])
        i++;

    new_env = malloc(sizeof(char *) * (i + 1));
    if (!new_env)
        return (NULL);

    i = 0;
    while (envp[i])
    {
        new_env[i] = strdup(envp[i]); // ft_strdup kullanabilirsin
        i++;
    }
    new_env[i] = NULL;
    return (new_env);
}

int is_builtin(char *input)
{
    if (!strcmp("echo", input))
        return 1;
    else if (!strcmp("cd", input))
        return 2;
    else if (!strcmp("pwd", input))
        return 3;
    else if (!strcmp("export", input))
        return 4;
    else if (!strcmp("unset", input))
        return 5;
    else if (!strcmp("env", input))
        return 6;
    else if (!strcmp("exit", input))
        return 7;
    return 0;
}

char ***make_env_dictionary(char **envp)
{
    int i, count = 0;

    while (envp[count])
        count++;

    char ***env_ret = malloc(sizeof(char **) * (count + 1));
    if (!env_ret)
        return NULL;

    for (i = 0; i < count; i++)
    {
        /* '=' işaretini bul */
        int pos = 0;
        while (envp[i][pos] && envp[i][pos] != '=')
            pos++;

        /* anahtar ve değer uzunlukları */
        int key_len = pos;
        int value_len = 0;
        if (envp[i][pos] == '=') /* '=' bulunduysa */
            while (envp[i][pos + 1 + value_len])
                value_len++;

        /* 2 string + NULL saklayacak alan */
        env_ret[i] = malloc(sizeof(char *) * 3);

        /* --- key kopyala --- */
        env_ret[i][0] = malloc(key_len + 1);
        for (int k = 0; k < key_len; k++)
            env_ret[i][0][k] = envp[i][k];
        env_ret[i][0][key_len] = '\0';

        /* --- value kopyala (varsa) --- */
        env_ret[i][1] = malloc(value_len + 1);
        for (int v = 0; v < value_len; v++)
            env_ret[i][1][v] = envp[i][pos + 1 + v];
        env_ret[i][1][value_len] = '\0';

        env_ret[i][2] = NULL; /* satır sonu */
    }
    env_ret[count] = NULL; /* tablo sonu */
    return env_ret;
}


char *find_path(char **env) // geçici
{
    size_t i;
    size_t j;
    char *needle = "PATH=";

    i = 0;
    while (env[i])
    {
        j = 0;

        while (env[i][j] == needle[j])
        {
            if (needle[j + 1] == '\0')
                return ((char *)&env[i][j + 1]);
            j++;
        }
        i++;
    }
    return (NULL);
}

void update_env_value(char ***env_dict, const char *key, const char *value)
{
    int i = 0;

    // Find the key in the environment
    while (env_dict[i] != NULL)
    {
        if (strcmp(env_dict[i][0], key) == 0)
        {
            // Free old value and set new value
            free(env_dict[i][1]);
            env_dict[i][1] = strdup(value);
            return;
        }
        i++;
    }

    // Key not found (could implement adding it here)
    printf("Warning: Could not update %s, key not found\n", key);
}

char ***add_env_variable(char ***env_dict, t_export *st_export)
{
    int count = 0;
    char ***new_env;
    
    // Count existing entries
    while (env_dict[count] != NULL)
        count++;
    
    // Create new array with space for new entry
    new_env = malloc(sizeof(char **) * (count + 2));
    if (!new_env)
        return env_dict;
    
    // Copy existing entries
    for (int i = 0; i < count; i++)
        new_env[i] = env_dict[i];
    
    // Add new entry
    new_env[count] = malloc(sizeof(char *) * 3);
    if (!new_env[count])
        return env_dict;
    
    // Copy key and value WITHOUT adding any quotes
    new_env[count][0] = strdup(st_export->key);
    new_env[count][1] = strdup(st_export->value); // No quotes added here
    new_env[count][2] = NULL;
    
    // NULL terminate array
    new_env[count + 1] = NULL;
    
    // Free old array (not its contents)
    free(env_dict);
    
    return new_env;
}

char *ft_strjoin(char const *s1, char const *s2)
{
    int i;
    int j;
    char *mem;

    i = 0;
    j = 0;
    mem = (char *)malloc(ft_strlen(s1) + ft_strlen(s2) + 1);
    if (!mem)
        return (NULL);
    while (s1[i])
    {
        mem[i] = s1[i];
        i++;
    }
    while (s2[j])
    {
        mem[i + j] = s2[j];
        j++;
    }
    mem[i + j] = '\0';
    return (mem);
}