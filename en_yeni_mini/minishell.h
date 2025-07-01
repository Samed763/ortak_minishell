#ifndef MINISHELL_H
#define MINISHELL_H

#include <readline/readline.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct s_data
{

}t_data;

int	is_valid_after_pipe(int c);
int is_valid_filename_char(int c);

#endif
