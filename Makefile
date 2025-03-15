# minilibx_test
NAME		=	minilibx_test
SRCS		=	main.c
OBJS 		=	$(SRCS:%.c=%.o)
# Compiler
CC			=	gcc
LIBX_FLAGS	=	mlx_linux/libmlx.a -lXext -lX11
CFLAGS		=	-Wall -Wextra -Werror

# Rules
all:		$(NAME)

$(NAME):	$(OBJS)
			$(CC) $(OBJS) submodules/minilibx-linux/libmlx.a -Imlx_linux -lXext -lX11 -lm -o $(NAME)

%.o: %.c
		 $(CC) $(CCFLAGS) -Imlx_linux -c $< -o $@

minilibx:
			git submodule update --init

clean:
			$(RM) $(OBJS)

fclean:		clean
			$(RM) $(NAME)

re:			fclean all

.PHONY:		all clean fclean re
