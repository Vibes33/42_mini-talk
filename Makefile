CC = cc
CFLAGS = -Wall -Wextra -Werror -g

NAME_SERVER = server
NAME_CLIENT = client

SRC_SERVER = server.c utilis.c
SRC_CLIENT = client.c

OBJ_SERVER = $(SRC_SERVER:.c=.o)
OBJ_CLIENT = $(SRC_CLIENT:.c=.o)

# Libft and Printf
LIBFT_DIR = Libft
PRINTF_DIR = Printf
LIBFT = $(LIBFT_DIR)/libft.a
LIBPRINTF = $(PRINTF_DIR)/libftprintf.a

all: $(LIBFT) $(LIBPRINTF) $(NAME_SERVER) $(NAME_CLIENT)

$(LIBFT):
	make -C $(LIBFT_DIR)

$(LIBPRINTF):
	make -C $(PRINTF_DIR)

$(NAME_SERVER): $(OBJ_SERVER) $(LIBFT) $(LIBPRINTF)
	$(CC) $(CFLAGS) $(OBJ_SERVER) -o $(NAME_SERVER) -L$(LIBFT_DIR) -lft -L$(PRINTF_DIR) -lftprintf

$(NAME_CLIENT): $(OBJ_CLIENT) $(LIBFT) $(LIBPRINTF)
	$(CC) $(CFLAGS) $(OBJ_CLIENT) -o $(NAME_CLIENT) -L$(LIBFT_DIR) -lft -L$(PRINTF_DIR) -lftprintf

%.o: %.c
	$(CC) $(CFLAGS) -I$(LIBFT_DIR) -I$(PRINTF_DIR) -c $< -o $@

clean:
	rm -f $(OBJ_SERVER) $(OBJ_CLIENT)
	make -C $(LIBFT_DIR) clean
	make -C $(PRINTF_DIR) clean

fclean: clean
	rm -f $(NAME_SERVER) $(NAME_CLIENT)
	make -C $(LIBFT_DIR) fclean
	make -C $(PRINTF_DIR) fclean

re: fclean all

.PHONY: all clean fclean re
