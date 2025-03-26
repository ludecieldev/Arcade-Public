##
## EPITECH PROJECT, 2025
## Arcade
## File description:
## Makefile for SFML graphics library
##

NAME = ../../lib/arcade_sfml.so

SRC = src/SFMLLib.cpp

OBJ = $(SRC:.cpp=.o)

CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -fPIC -fno-gnu-unique
CPPFLAGS = -I./include -I../../include/core_interfaces -I../../src/utils
LDFLAGS = -shared -lsfml-graphics -lsfml-window -lsfml-system

all: $(NAME)

$(NAME): $(OBJ)
	@mkdir -p ../../lib
	$(CXX) -o $(NAME) $(OBJ) $(LDFLAGS)
	@echo "SFML library compiled successfully"

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
