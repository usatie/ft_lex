NAME = ft_lex
CC = cc
CFLAGS = -Wall -Wextra -Werror
SRCS = src/main.c
OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME):
	$(CC) -o $(NAME) $(SRCS) $(CFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
