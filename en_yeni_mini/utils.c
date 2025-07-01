#include "minishell.h"

int	is_valid_after_pipe(int c)
{
	return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || 
			(c >= '0' && c <= '9') || c == '_' || c == '-' || 
			c == '.' || c == '/' || c == '\'' || c == '"' || 
			c == '$' || c == ' ' || c == '\t');
}

int is_valid_filename_char(int c)
{
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || 
            (c >= '0' && c <= '9') || c == '_' || c == '-' || 
            c == '.' || c == '/' || c == '~' || c == '\'' || 
            c == '"' || c == '$');
}