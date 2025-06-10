#include "minishell.h"

int to_do_list(t_data *data, char **envp)
{
    data->env = Malloc(sizeof(t_env));
    data->env->env_dictionary = make_env_dictionary(envp);
    data->last_exit_status = 0;  // Initialize exit status to 0
    return 0;
}
char *trim_quotes(char *arg, int free_old)
{
    if (!arg)
        return NULL;

    int len = ft_strlen(arg);
    if (len < 2)
        return arg;

    int i = 1;
    if (len > 2 && arg[1] == '$')
        i++;

    int new_len = len - i - 1;
    if (new_len < 0)
        new_len = 0;

    char *new_arg = malloc(new_len + 1);
    if (!new_arg)
        return NULL;

    int j = 0;
    while (i < len - 1)
        new_arg[j++] = arg[i++];
    new_arg[j] = '\0';

    if (free_old)
        free(arg);
    return new_arg;
}

char * take_env(char * key ,char * value,char * arg)// şu anda echo "$HOME" tıranaklı geldiğinden bu durumda çalışmıyor 
{
    //tüm metni bölüyor ve aranılan env_variable var ise onun yerine onu koyup birleştirip return ediyor
    char **splitted = ft_split(arg,' ');
    char *total = ft_strdup("");
    int i = 0;
    char * new_key = ft_strdup("$");
    new_key = ft_strjoin(new_key,key);

    while (splitted[i]) 
    {
        if (!ft_strcmp(splitted[i],new_key))
        {
            splitted[i] = ft_strdup(value);
        }
        total = ft_strjoin(total , splitted[i]);
        if (splitted[i + 1]) //son boşluk olamasın 
            total = ft_strjoin(total , " ");
        i++;
    }
    printf("total -> %s\n",total);
    return total;
}

char *in_env(t_data *data, char *key, char *arg)
{   
    // For variable names like $HOME, just skip the $ character
    char *ret_value;
    char *var_name = key;
    if (key[0] == '$')
        var_name = key + 1;
    
    // Special case for $?
    if (!ft_strcmp(var_name, "?"))
    {
        char exit_status_str[12]; // Buffer for exit status as string
        snprintf(exit_status_str, sizeof(exit_status_str), "%d", data->last_exit_status);
        return take_env(key, exit_status_str, arg);
    }
        
    char *value = get_value_by_key(data->env->env_dictionary, var_name);
    printf("Looking up: %s -> %s\n", var_name, value ? value : "not found");
    if (!value) // Remove the semicolon!
    {
        return key; // Return original if not found
    }

    return take_env(key, value, arg);
}

char ** set_variables(t_data *data, char **args)// ' ile yazılanlar hariç içini envden veri koyuyor"
{
    int i =0;
    int bosluk_index = 0;
    
    while (args[i])
    {
        int j = 0;
        int in_dquote = 0;
        int in_squote = 0;
        while (args[i][j])
        {
            if (args[i][j] == ' ')
                bosluk_index = j;
            // Track quote state
            if (args[i][j] == '"' && !in_squote)
                in_dquote = !in_dquote;
            else if (args[i][j] == '\'' && !in_dquote)
                in_squote = !in_squote;
            
            // Process environment variables (when not in single quotes)
            if (args[i][j] == '$' && !in_squote && args[i][j+1] && args[i][j+1] != ' ') //  selam  $USR ben amcı  
            {
                int var_start = j + 1; // Start right after the $
                int var_end = var_start;
                
                // Find the end of the variable name (space, quote, or end of string)
                while (args[i][var_end] && args[i][var_end] != ' ' && 
                    args[i][var_end] != '"' && args[i][var_end] != '\'' &&
                    args[i][var_end] != '$')
                    var_end++;
                int var_len = var_end - var_start;
                char *var_name = Malloc(var_len + 1);
                strncpy(var_name, args[i] + var_start, var_len);
                var_name[var_len] = '\0';
                args[i] = in_env(data, var_name,args[i]); //buradan başka "$HOME SAFS" bu durumda düzgün çalışması lazım
            }
            j++;
        }
        i++;
    }
    return args;
}

int main(int argc, char **argv, char **envp)
{   
    char *input;
    t_data data;
    to_do_list(&data,envp);

    while (1)
    {
        input = readline("--> ");
        data.args = lexer(input);
        //sadece  doğru girdilerin geçebilmesi için kontorl
        
        data.args = set_variables(&data,data.args);
        print_tokens(data.args);
    }

    return 0;
}