#include "../minishell.h"

// Process export values with complex quoting
char *process_export_value(const char *raw_value)
{
    int i = 0, j = 0;
    int len = strlen(raw_value);
    char *result = malloc(len + 1); // Allocate enough space
    if (!result)
        return NULL;
    
    while (i < len) {
        // Skip pairs of quotes (both " and ')
        if ((raw_value[i] == '"' || raw_value[i] == '\'') && 
            i + 1 < len) {
            char quote = raw_value[i];
            i++; // Skip opening quote
            
            // Copy characters until matching quote
            while (i < len && raw_value[i] != quote)
                result[j++] = raw_value[i++];
                
            if (i < len) i++; // Skip closing quote
        } else {
            // Copy regular characters
            result[j++] = raw_value[i++];
        }
    }
    
    result[j] = '\0'; // Null-terminate
    return result;
}

void fill_export(char **token, t_export *st_export)
{
    int equals_pos = -1;
    int i = 0;
    
    // Check if token[1] exists
    if (!token[1]) {
        st_export->key = NULL;
        st_export->value = NULL;
        return;
    }
    
    // Find the '=' character
    while (token[1][i] != '\0') {
        if (token[1][i] == '=') {
            equals_pos = i;
            break;
        }
        i++;
    }
    
    // Handle cases based on whether '=' was found
    if (equals_pos == -1) {
        // No '=' found, only a variable name to export
        st_export->key = strdup(token[1]);
        st_export->value = strdup("");  // Empty value
    } else {
        // '=' found, extract key and value
        st_export->key = malloc(equals_pos + 1);
        if (st_export->key) {
            strncpy(st_export->key, token[1], equals_pos);
            st_export->key[equals_pos] = '\0';
        }
        
        // Use our new function to process the value
        st_export->value = process_export_value(token[1] + equals_pos + 1);
    }
}



char ***sort_env(char ***env_dic)
{
    int i, j, count = 0;
    char **temp;
    
    // Count environment variables
    while (env_dic[count] != NULL)
        count++;
    
    // Create a copy of the array to sort
    char ***sorted = malloc(sizeof(char **) * (count + 1));
    if (!sorted)
        return NULL;
    
    // Copy pointers to sorted array
    for (i = 0; i < count; i++)
        sorted[i] = env_dic[i];
    
    // Null terminate
    sorted[count] = NULL;
    
    // Simple bubble sort by key name
    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - i - 1; j++) {
            // Compare keys and swap if needed
            if (strcmp(sorted[j][0], sorted[j + 1][0]) > 0) {
                temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }
    
    return sorted;
}


int ft_export(char **token, t_data *data)
{
    int i;
    int env_count = 0;
    char ***display_array;
    
    // If no arguments, print all environment variables in declared format
    if (!token[1]) {
        display_array = sort_env(data->env->env_dictionary);
        if (display_array) {
            print_export(display_array);
            free(display_array); // Free the sorted array (but not its contents)
        }
    } else {
        // Process each argument (token[1], token[2], etc.)
        for (i = 1; token[i] != NULL; i++) {
            // Allocate memory for st_export
            t_export *st_export = malloc(sizeof(t_export));
            if (!st_export)
                return 1;
            
            // Initialize to NULL to prevent issues if fill_export fails
            st_export->key = NULL;
            st_export->value = NULL;
            
            // Create a temporary token array with just this argument
            char *temp_tokens[3];  // [command, arg, NULL]
            temp_tokens[0] = token[0];
            temp_tokens[1] = token[i];
            temp_tokens[2] = NULL;
            
            // Fill export values for this argument
            fill_export(temp_tokens, st_export);
            
            // Add to environment dictionary
            data->env->env_dictionary = add_env_variable(data->env->env_dictionary, st_export);
            
            // Free the temporary structure (but not its contents which are now part of env_dict)
            free(st_export);
        }
    }
    
    return 0;
}