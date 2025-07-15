CC = cc
NAME = minishell
SRC = minishell.c utils.c syntax_check.c lexer.c parser.c expander.c
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
