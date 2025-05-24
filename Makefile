# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/08/05 01:22:18 by samusanc          #+#    #+#              #
#    Updated: 2025/05/08 14:56:33 by smsanchez        ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= ft_nm
COMPOSE := $(shell command -v docker-compose || echo "docker compose")

#=============================== INCLUDES ===============================#

INC			= -I./includes/ 
INC		   += -I./libft/ 

#============================== LIBRARIES ===============================#

SUBMODLIB	= ./libft/libft.a

#================================= GCC ==================================#

# GCC WITH LIBS AND INCLUDES
CFLAGS		=# -fsanitize=address
CFLAGS	   +=# -Wall -Wextra -Werror
CC			= gcc $(CFLAGS) $(INC)

#================================= SCRS =================================#

SRCS		= \
				./src/main.c \
				./src/x64/x64_utils.c \
				./src/x86/x86_utlis.c \
				./src/utils/flags.c \
				./src/utils/print_utils.c \
				./src/utils/utils.c \

#================================= OBJS =================================#

O_DIR		= ./objects/
OBJS		= $(addprefix $(O_DIR)/, $(SRCS:.c=.o))

#========================================================================#

$(O_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(INC) -g -c $< -o $(O_DIR)/$(<:.c=.o)

all: submodules $(NAME)

$(NAME): submodules $(OBJS) .mandatory
	@echo "mandatory done..."

#============================= SUBMODULES =============================#

submodules: .make_submodules
	@echo "All submodules loaded..."

.make_submodules: 
	@make -sC ./libft/ all
	@touch .make_submodules

#======================= MANDATORY AND BONUS =========================#

.mandatory:
	$(CC) -o $(NAME) $(OBJS) $(SUBMODLIB)
	@touch .mandatory

re: fclean all

fclean: clean
	@echo "cleaning binaries..."
	@rm -f $(NAME)
	@rm -rf .mandatory
	@rm -rf .make_submodules
	@rm -rf .clean

submodule_fclean:
	@make -sC ./libft/ fclean

clean: .clean
	@echo "objects removed!"

.clean: .submodule_clean
	@echo "cleaning objects..."
	@rm -f $(OBJS)
	@rm -rf $(O_DIR)
	@touch .clean

.submodule_clean:
	@make -sC ./libft/ clean

.PHONY: all clean fclean re title submodules submodule_fclean
