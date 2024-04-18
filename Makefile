CC=gcc
FLAGS= -Wall -Werror -Wextra
NAME=ft_nm
SRC= 	srcs/ft_nm.c \

LIBFT=  libft/ft_strlen.c libft/ft_putstr_fd.c libft/ft_putchar_fd.c libft/ft_memcpy.c libft/ft_strncmp.c libft/ft_strdup.c libft/ft_isalpha.c libft/ft_tolower.c  libft/ft_strjoin.c \

OBJ = $(SRC:.c=.o) $(LIBFT:.c=.o)
all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o $(NAME)

%.o: %.c
	$(CC) $(FLAGS) -c $< -o $@

clean:
	make -C libft/ clean
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all