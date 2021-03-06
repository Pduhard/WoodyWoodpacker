# **************************************************************************** #
#                                                           LE - /             #
#                                                               /              #
#    Makefile                                         .::    .:/ .      .::    #
#                                                  +:+:+   +:    +:  +:+:+     #
#    By: pduhard- <marvin@le-101.fr>                +:+   +:    +:    +:+      #
#                                                  #+#   #+    #+    #+#       #
#    Created: 2018/12/10 21:06:37 by pduhard-     #+#   ##    ##    #+#        #
#    Updated: 2019/03/04 11:32:24 by pduhard-    ###    #+. /#+    ###.fr      #
#                                                          /                   #
#                                                         /                    #
# **************************************************************************** #

##__________CONFIG__________#

NAME		=	woody_woodpacker

CC			=	clang
NASM		= 	nasm
CFLAGS	= -g -Wall -Werror -Wextra -O3 -ffast-math -march=native -flto -fPIC
SFLAGS	=	-f elf64 -O3
LIB_FLAGS	=	-L$(LIB_PATH) $(LIB_FLAG)
INC_DIR	=	./includes/
INCLUDES	=	woody_woodpacker.h
# OTOOL_INC_DIR	=	otool/includes/
# OTOOL_INCLUDES	=	ft_otool.h
# COMMON_INC_DIR	=	common/includes/
# COMMON_INCLUDES	=	common.h

SRC_PATH	=	./srcs/
# OTOOL_SRC_PATH	=	./otool/srcs/
# COMMON_SRC_PATH	=	./common/srcs/

BIN_PATH	=	./bins/
# OTOOL_BIN_PATH	=	./otool/bins/
# COMMON_BIN_PATH	=	./common/bins/
LIB_PATH	=	./libft/

SRC		=		main.c		\
					error.c		\
					segment.c	\
					section.c	\
					write.c		

ASM_SRC	= unpack.s	\

BIN			=	$(SRC:.c=.o)
ASM_BIN = $(ASM_SRC:.s=.o)

LIB_FLAG			=	-lft
LIB						=	libft.a

BINS				=	$(addprefix $(BIN_PATH), $(BIN))
ASM_BINS			= $(addprefix $(BIN_PATH), $(ASM_BIN))
LIBS				=	$(addprefix $(LIB_PATH), $(LIB))
INCS				= $(addprefix $(INC_DIR), $(INCLUDES))

.PHONY: all clean fclean re lib libft

#__________COLORS__________#

R			=	\033[0;31m
G			=	\033[32;7m
B			=	\033[0;34m
N			=	\33[0m

#__________RULES__________#

all: make_libft $(NAME)

libft: $(LIBS)

$(NAME): $(LIBS) $(BINS) $(ASM_BINS)
	@$(CC) $(CFLAGS) -o $@ $^ $(LIB_FLAGS) -I $(INC_DIR)
	@echo "\n\n$(B)[EXECUTABLE \"$(NAME)\" READY]\n"

make_libft:

		@make -C $(LIB_PATH)

$(LIBS):

	@make -C $(LIB_PATH)

$(BIN_PATH)%.o: $(SRC_PATH)%.c $(INCS)

	@mkdir -p $(BIN_PATH) || true
	@$(CC) $(CFLAGS) -I $(INC_DIR) -o $@ -c $< && echo "${G} \c"

$(BIN_PATH)%.o: $(SRC_PATH)%.s $(INCS)

	@mkdir -p $(BIN_PATH) || true
	@$(NASM) $(SFLAGS) -I $(INC_DIR) -o $@ $< && echo "${G} \c"

clean:

	@make clean -C $(LIB_PATH)
	@echo "${R}[CLEANING $(NAME) BINARIES]\n"
	@rm -Rf $(BIN_PATH)

fclean: clean

	@echo "${R}[REMOVING $(LIB)]"
	@rm -rf $(LIBS)
	@echo "\n${R}[REMOVING \"$(NAME)\"]"
	@rm -f $(NAME)

re: fclean all
