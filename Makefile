# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: miwasa <miwasa@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/11/26 21:38:04 by <your_name>       #+#    #+#              #
#    Updated: 2025/01/04 15:33:22 by miwasa           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# プロジェクト名
NAME		:= miniRT

# libft 関連
FT_DIR		:= ./libft
LIBFT		:= $(FT_DIR)/libft.a

# miniLibX 関連
MLX_DIR		:= ./minilibx-linux
MLX_LIB		:= $(MLX_DIR)/libmlx.a

# インクルード・ソース・オブジェクトのディレクトリ
INCS_DIR	:= ./incs
SRCS_DIR	:= ./srcs
OBJS_DIR	:= ./objs

# コンパイラとフラグ
CC			:= cc
CFLAGS		:= -Wall -Wextra -Werror -I$(INCS_DIR) -I$(FT_DIR) -I$(MLX_DIR) -g

# miniLibX リンク時のフラグ (Linux想定: -lXext -lX11 など)
MLXFLAGS	:= -L$(MLX_DIR) -lmlx -lm -L/usr/X11/lib -lXext -lX11

# ソースファイル (例: 必要に応じて修正してください)
# ここでは例として main.c や parse.c などを挙げていますが、
# 実際の miniRT ソースに置き換えてください。
SRCS		:= \
	main.c \
	parse.c \
	render.c \
	intersect.c \
	utils.c \
	events.c \

# オブジェクトファイル
OBJS		:= $(addprefix $(OBJS_DIR)/, $(SRCS:.c=.o))

# ルール定義
.PHONY: all clean fclean re

all: $(LIBFT) $(MLX_LIB) $(NAME)

# libft のビルド
$(LIBFT):
	$(MAKE) -C $(FT_DIR)

# miniLibX のビルド
$(MLX_LIB):
	$(MAKE) -C $(MLX_DIR)

# miniRT バイナリ生成
$(NAME): $(OBJS) $(LIBFT) $(MLX_LIB)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LIBFT) $(MLXFLAGS)

# オブジェクト生成
$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r $(OBJS_DIR)
	$(MAKE) clean -C $(FT_DIR)
	$(MAKE) clean -C $(MLX_DIR)

fclean: clean
	$(RM) $(NAME)
	$(MAKE) fclean -C $(FT_DIR)

re: fclean all
