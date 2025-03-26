##
## EPITECH PROJECT, 2025
## Arcade
## File description:
## Makefile for Snake game library
##

NAME = ../../lib/arcade_snake.so

SRC = src/SnakeGame.cpp

OBJ = $(SRC:.cpp=.o)

CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -fPIC -fno-gnu-unique
CPPFLAGS = -I./include -I../../include/core_interfaces -I../../src/utils

LDFLAGS = -shared

all: $(NAME)

$(NAME): $(OBJ)
	@mkdir -p ../../lib
	$(CXX) -o $(NAME) $(OBJ) $(LDFLAGS)
	@echo "Snake game library compiled successfully"

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
