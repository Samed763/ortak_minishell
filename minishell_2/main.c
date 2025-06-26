#include "minishell.h"

int to_do_list(t_data *data, char **envp)
{
    data->env = Malloc(sizeof(t_env));
    data->env->env_dictionary = make_env_dictionary(envp);
    data->last_exit_status = 0;  // Initialize exit status to 0
    return 0;
}
char *trim_quotes(char *arg, int free_old, int take_dollar)// 1-> $a" tirank sonda olunca silmiyor
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

char * take_env(char * key ,char * value,char * arg)
{
    //tüm metni bölüyor ve aranılan env_variable var ise onun yerine onu koyup birleştirip return ediyor
    char **splitted = ft_split(arg,' ');
    char *total = ft_strdup("");
    int i = 0;
    char * new_key = ft_strdup("$");
    new_key = ft_strjoin(new_key,key);

    while (splitted[i]) 
    {
        splitted[i] = ft_strdup(trim_quotes(splitted[i],1,0)); //Token 1:  "$HOME" durumunu düzeltiyor
        printf("%d-> %s \n",i,splitted[i]);
        if (!ft_strcmp(splitted[i],new_key))
        {
            splitted[i] = ft_strdup(value);
        }
        total = ft_strjoin(total , splitted[i]);
        if (splitted[i + 1]) //son boşluk olamasın 
            total = ft_strjoin(total , " ");
        i++;
    }
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
        value = ft_strdup("");
    }
    printf("->>%s<<-\n",value);

    return take_env(key, value, arg);
}

char ** set_variables(t_data *data, char **args)// ' ile yazılanlar hariç içini envden veri koyuyor"
{
    int i = 0;
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

// Function to process environment variables in the parsed command structure
void set_variables_parsed(t_data *data)
{
    int cmd_idx = 0;
    
    if (!data || !data->parsed || !data->parsed->args)
        return;
        
    // Process each command segment
    while (data->parsed->args[cmd_idx])
    {
        // Apply variable substitution to each argument in the command
        data->parsed->args[cmd_idx] = set_variables(data, data->parsed->args[cmd_idx]);
        cmd_idx++;
    }
    
    // Process redirection filenames for variables
    if (data->parsed->input_file)
    {
        char **processed_arr;
        char *temp_arr[2];
        temp_arr[0] = data->parsed->input_file;
        temp_arr[1] = NULL;
        processed_arr = set_variables(data, temp_arr);
        // Free the old input file and update with processed value
        free(data->parsed->input_file);
        data->parsed->input_file = ft_strdup(processed_arr[0]);
        free(processed_arr[0]);
        free(processed_arr);
    }
    
    if (data->parsed->output_file)
    {
        char **processed_arr;
        char *temp_arr[2];
        temp_arr[0] = data->parsed->output_file;
        temp_arr[1] = NULL;
        processed_arr = set_variables(data, temp_arr);
        // Free the old output file and update with processed value
        free(data->parsed->output_file);
        data->parsed->output_file = ft_strdup(processed_arr[0]);
        free(processed_arr[0]);
        free(processed_arr);
    }
    
    if (data->parsed->heredoc_limiter)
    {
        char **processed_arr;
        char *temp_arr[2];
        temp_arr[0] = data->parsed->heredoc_limiter;
        temp_arr[1] = NULL;
        processed_arr = set_variables(data, temp_arr);
        // Free the old heredoc limiter and update with processed value
        free(data->parsed->heredoc_limiter);
        data->parsed->heredoc_limiter = ft_strdup(processed_arr[0]);
        free(processed_arr[0]);
        free(processed_arr);
    }
}

// Function to check if all quotes are properly closed
int check_quotes_closed(char **args)
{
    int i = 0;
    
    while (args[i])
    {
        int j = 0;
        int dquote_count = 0;
        int squote_count = 0;
        
        while (args[i][j])
        {
            if (args[i][j] == '"')
                dquote_count++;
            else if (args[i][j] == '\'')
                squote_count++;
            j++;
        }
        
        // Odd number of quotes means unclosed quotes
        if (dquote_count % 2 != 0 || squote_count % 2 != 0)
            return 0;
        i++;
    }
    
    return 1;
}

// Function to validate the arguments
int validate_args(t_data *data)
{
    char **args = data->args;
    int i = 0;
    
    if (!args || !args[0])
        return 0;  // No arguments
    
    // Check for unclosed quotes
    if (!check_quotes_closed(args))
    {
        printf("Error: Unclosed quotes\n");
        data->last_exit_status = 1;
        return 0;
    }
    
    // Check for invalid redirection operators (>>, >, <, <<)
    i = 0;
    while (args[i])
    {
        // Check for invalid redirection formats (>>>, >>>>, etc)
        if (args[i][0] == '>' || args[i][0] == '<')
        {
            int len = ft_strlen(args[i]);
            // Valid: >, >>, <, <<
            // Invalid: >>>+, <<<+
            if ((args[i][0] == '>' && len > 2) || 
                (args[i][0] == '<' && len > 2))
            {
                printf("Error: syntax error near unexpected token `%c'\n", args[i][0]);
                data->last_exit_status = 1;
                return 0;
            }
        }
        i++;
    }
    
    // Check for pipe at the beginning
    if (ft_strcmp(args[0], "|") == 0)
    {
        printf("Error: Syntax error near unexpected token '|'\n");
        data->last_exit_status = 1;
        return 0;
    }
    
    // Check for consecutive pipes and pipe at the end
    i = 0;
    while (args[i])
    {
        // Check for consecutive pipes
        if (ft_strcmp(args[i], "|") == 0 && args[i+1] && ft_strcmp(args[i+1], "|") == 0)
        {
            printf("Error: Syntax error near unexpected token '|'\n");
            data->last_exit_status = 1;
            return 0;
        }
        
        // Check for pipe at the end
        if (ft_strcmp(args[i], "|") == 0 && !args[i+1])
        {
            printf("Error: Syntax error near unexpected token '|'\n");
            data->last_exit_status = 1;
            return 0;
        }
        
        // Check for redirection without file
        if ((ft_strcmp(args[i], "<") == 0 || 
             ft_strcmp(args[i], ">") == 0 ||
             ft_strcmp(args[i], ">>") == 0 ||
             ft_strcmp(args[i], "<<") == 0) && !args[i+1])
        {
            printf("Error: Syntax error near unexpected token '%s'\n", args[i]);
            data->last_exit_status = 1;
            return 0;
        }
        
        // Check for multiple redirections in a row
        if ((ft_strcmp(args[i], "<") == 0 || 
             ft_strcmp(args[i], ">") == 0 ||
             ft_strcmp(args[i], ">>") == 0 ||
             ft_strcmp(args[i], "<<") == 0) && args[i+1])
        {
            if (ft_strcmp(args[i+1], "<") == 0 || 
                ft_strcmp(args[i+1], ">") == 0 ||
                ft_strcmp(args[i+1], ">>") == 0 ||
                ft_strcmp(args[i+1], "<<") == 0)
            {
                printf("Error: Syntax error near unexpected token '%s'\n", args[i+1]);
                data->last_exit_status = 1;
                return 0;
            }
        }
        
        i++;
    }
    
    return 1;  // Arguments are valid
}

void input_file(t_data *data ,t_parsed parsed)
{

}

void parser(t_data *data)
{
    int i;
    int pipe_count = 0;
    
    // Count pipes to determine number of command segments
    for (i = 0; data->args[i]; i++) {
        if (ft_strcmp(data->args[i], "|") == 0)
            pipe_count++;
    }
    
    // Number of commands is pipe_count + 1
    int cmd_count = pipe_count + 1;
    
    // Allocate memory for parsed structure and command segments
    data->parsed = Malloc(sizeof(t_parsed));
    data->parsed->args = Malloc(sizeof(char **) * (cmd_count + 1));
    data->parsed->args[cmd_count] = NULL;
    
    // Initialize other parsed fields
    data->parsed->input_file = NULL;
    data->parsed->output_file = NULL;
    data->parsed->append = 0;
    data->parsed->heredoc = 0;
    data->parsed->heredoc_limiter = NULL;
    
    // First pass: Count normal arguments (excluding redirections) for each command
    int cmd_idx = 0;
    int *arg_counts = Malloc(sizeof(int) * cmd_count);
    for (i = 0; i < cmd_count; i++)
        arg_counts[i] = 0;
    
    for (i = 0; data->args[i]; i++) {
        // Check for pipe to switch to next command
        if (ft_strcmp(data->args[i], "|") == 0) {
            cmd_idx++;
            continue;
        }
        
        // Skip redirection operators and their arguments
        if ((ft_strcmp(data->args[i], "<") == 0) ||
            (ft_strcmp(data->args[i], ">") == 0) ||
            (ft_strcmp(data->args[i], ">>") == 0) ||
            (ft_strcmp(data->args[i], "<<") == 0)) {
            i++; // Skip the file/limiter argument
            continue;
        }
        
        // Count normal arguments
        arg_counts[cmd_idx]++;
    }
    
    // Allocate memory for each command's arguments
    for (i = 0; i < cmd_count; i++) {
        data->parsed->args[i] = Malloc(sizeof(char *) * (arg_counts[i] + 1));
        data->parsed->args[i][arg_counts[i]] = NULL;
    }
    
    // Second pass: Fill the parsed structure
    cmd_idx = 0;
    int *arg_indices = Malloc(sizeof(int) * cmd_count);
    for (i = 0; i < cmd_count; i++)
        arg_indices[i] = 0;
    
    for (i = 0; data->args[i]; i++) {
        // Handle pipe
        if (ft_strcmp(data->args[i], "|") == 0) {
            cmd_idx++;
            continue;
        }
        
        // Handle input redirection
        if (ft_strcmp(data->args[i], "<") == 0 && data->args[i+1]) {
            if (data->parsed->input_file)
                free(data->parsed->input_file);
            data->parsed->input_file = ft_strdup(data->args[i+1]);
            i++; // Skip the filename
            continue;
        }
        
        // Handle heredoc
        if (ft_strcmp(data->args[i], "<<") == 0 && data->args[i+1]) {
            data->parsed->heredoc = 1;
            if (data->parsed->heredoc_limiter)
                free(data->parsed->heredoc_limiter);
            data->parsed->heredoc_limiter = ft_strdup(data->args[i+1]);
            i++; // Skip the limiter
            continue;
        }
        
        // Handle output redirection
        if (ft_strcmp(data->args[i], ">") == 0 && data->args[i+1]) {
            data->parsed->append = 0;
            if (data->parsed->output_file)
                free(data->parsed->output_file);
            data->parsed->output_file = ft_strdup(data->args[i+1]);
            i++; // Skip the filename
            continue;
        }
        
        // Handle append output redirection
        if (ft_strcmp(data->args[i], ">>") == 0 && data->args[i+1]) {
            data->parsed->append = 1;
            if (data->parsed->output_file)
                free(data->parsed->output_file);
            data->parsed->output_file = ft_strdup(data->args[i+1]);
            i++; // Skip the filename
            continue;
        }
        
        // Add normal argument to current command
        data->parsed->args[cmd_idx][arg_indices[cmd_idx]] = ft_strdup(data->args[i]);
        arg_indices[cmd_idx]++;
    }
    
    // Free temporary arrays
    free(arg_counts);
    free(arg_indices);
}


int main(int argc, char **argv, char **envp)
{   
    char *input;
    t_data data;
    to_do_list(&data,envp);
    while (1)
    {
        input = readline("--> ");//satır okuma 
        data.args = lexer(input);// tokenlara ayırma 
        
        // Validate arguments
        if (data.args && validate_args(&data))
        {
            parser(&data);
            // Apply environment variable substitution to the parsed structure
            set_variables_parsed(&data);
            print_parsed_commands(&data); // Print the parsed commands
            print_tokens(data.args);//geçici token yazdırma
        }
    }

    return 0;
}