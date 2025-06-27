#include "minishell.h"

// Helper function to check if character is a redirection operator
int is_redirection(char c)
{
    return (c == '<' || c == '>');
}

// Helper function to check if character is a special character (pipe or redirection)
int is_special_char(char c)
{
    return (c == '|' || c == '<' || c == '>');
}

char **lexer(char *input)
{
    char **parts;
    int i = 0, j = 0, word = 0;
    int in_dquote = 0;  // Double quote flag
    int in_squote = 0;  // Single quote flag
    
    // Count approximate number of tokens to allocate appropriate size
    int estimated_tokens = 0;
    int temp_i = 0;
    while (input[temp_i])
    {
        if (input[temp_i] == ' ' || is_special_char(input[temp_i]))
            estimated_tokens++;
        temp_i++;
    }
    estimated_tokens += 10; // Add some buffer
    
    // Allocate memory for parts array
    parts = (char **)malloc(sizeof(char *) * estimated_tokens);
    if (!parts)
        return NULL;
    
    // Skip leading spaces
    while (input[i] && input[i] == ' ')
        i++;
    j = i;
    
    while (input[i] && input[i] != '\n')  // Stop at newline
    {
        // Handle quotes
        if (input[i] == '"' && !in_squote)
            in_dquote = !in_dquote;
        else if (input[i] == '\'' && !in_dquote)
            in_squote = !in_squote;
        
        // Handle pipes and redirections when not in quotes
        if (is_special_char(input[i]) && !in_dquote && !in_squote)
        {
            // Add current token if any
            if (i > j)
            {
                parts[word] = ft_substr(input, j, i - j);
                word++;
            }
            
            // Special handling for redirection operators
            if (is_redirection(input[i]))
            {
                int start = i;
                
                // Check for >> (append) or << (heredoc)
                if (input[i+1] && input[i] == input[i+1])
                {
                    i++;  // Skip second character of >> or <<
                    parts[word] = ft_substr(input, start, 2);  // Extract << or >>
                }
                else
                {
                    parts[word] = ft_substr(input, start, 1);  // Extract < or >
                }
                word++;
                i++;  // Move past the operator
            }
            else  // Handle pipe
            {
                parts[word] = ft_substr(input, i, 1);
                word++;
                i++;
            }
            
            // Skip spaces after operators
            while (input[i] && input[i] == ' ')
                i++;
            j = i;  // Update start position for next token
            continue;
        }
        
        // Split on spaces when not inside quotes
        if (input[i] == ' ' && !in_dquote && !in_squote)
        {
            if (i > j)  // Only create token if length > 0
            {
                parts[word] = ft_substr(input, j, i - j);
                word++;
            }
            // Skip multiple spaces
            while (input[i] && input[i] == ' ')
                i++;
            j = i;  // Move start pointer past the space(s)
            continue;
        }
        i++;
    }
    
    // Add the last token if any
    if (i > j)
    {
        parts[word] = ft_substr(input, j, i - j);
        word++;
    }
    
    parts[word] = NULL;  // Null-terminate the array
    
    // Resize to exact size to avoid wasted memory
    parts = realloc(parts, sizeof(char *) * (word + 1));
    
    return parts;
}