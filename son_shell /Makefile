CC = cc
NAME = minishell

SRC = minishell.c \
	Tools/expander.c Tools/split.c Tools/heredoc.c Tools/utils.c Tools/leak_manager.c \
	Executer/execute_command.c Executer/apply_input.c Executer/apply_output.c Executer/pipe_execute.c \
	analyzer/lexer.c analyzer/parser.c analyzer/syntax_check.c analyzer/count_words.c analyzer/parser_utils.c analyzer/parser_utils2.c \
	Built_ins/blt_cd.c Built_ins/blt_echo.c Built_ins/blt_env.c Built_ins/blt_exit.c \
	Built_ins/blt_exp_utils.c Built_ins/blt_exp_utils2.c Built_ins/blt_exp_utils3.c Built_ins/blt_exp_utils4.c \
	Built_ins/blt_export.c Built_ins/blt_in.c Built_ins/blt_pwd.c Built_ins/blt_unset.c

OBJ = $(SRC:.c=.o)

CFLAGS = -Wall -Wextra -Werror -g

all: $(NAME) clean

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ) -lreadline
	@echo "\033[0;32mCompilation complete. Executable: $(NAME)\033[0m"

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re