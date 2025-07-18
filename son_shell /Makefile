CC = cc
NAME = minishell
SRC = minishell.c utils.c syntax_check.c \
	lexer.c parser.c expander.c heredoc.c \
	split.c apply_input.c execute_command.c \
	apply_output.c pipe_execute.c \
	blt_in.c blt_cd.c blt_echo.c blt_env.c blt_exit.c \
	blt_export.c blt_unset.c blt_pwd.c blt_exp_utils.c \
	blt_exp_utils2.c blt_exp_utils3.c blt_exp_utils4.c

OBJ = $(SRC:.c=.o)

all: $(NAME) clean     #cleanı unutma -------------------------#

$(NAME): $(OBJ) 
	@$(CC) -Wall -Wextra -Werror -g -o $(NAME) $(OBJ) -lreadline
	@echo "\033[0;32mCompilation complete. Executable: $(NAME)\033[0m"

clean:
	@rm -f $(OBJ)

fclean: clean
	@rm -f $(NAME)

re: fclean all 

.PHONY: all clean fclean re
