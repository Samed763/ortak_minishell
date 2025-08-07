CC = cc
NAME = minishell

SRC = minishell.c tools/main_utils.c \
	tools/expander.c tools/expander_utils.c tools/split.c tools/heredoc.c tools/heredoc_utils.c tools/heredoc_utils2.c tools/utils.c tools/libft_utils.c tools/libft_utils2.c tools/check_utils.c tools/leak_manager.c tools/itoa.c\
	executer/execute_command.c executer/apply_input.c executer/apply_input_utils.c executer/apply_output.c executer/pipe_execute.c  executer/pipe_utils.c executer/execute_utils.c \
	analyzer/lexer.c analyzer/parser.c analyzer/syntax_check.c analyzer/count_words.c analyzer/parser_utils.c analyzer/parser_utils2.c \
	built_ins/blt_cd.c built_ins/blt_echo.c built_ins/blt_env.c built_ins/blt_exit.c \
	built_ins/blt_exp_utils.c built_ins/blt_exp_utils2.c built_ins/blt_exp_utils3.c built_ins/blt_exp_utils4.c \
	built_ins/blt_export.c built_ins/blt_in.c built_ins/blt_pwd.c built_ins/blt_unset.c

OBJ = $(SRC:.c=.o)

CFLAGS = -Wall -Wextra -Werror

all: $(NAME) clean

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ) -lreadline

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re