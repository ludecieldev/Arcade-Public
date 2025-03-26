##
## EPITECH PROJECT, 2025
## Arcade
## File description:
## Makefile for the core component
##

NAME = ../arcade

SRC = Main.cpp                      \
      core/src/ArcadeCore.cpp       \
      core/src/LibraryManager.cpp   \
      core/src/GraphicsManager.cpp  \
      core/src/GameManager.cpp      \
      core/src/Menu.cpp

OBJ = $(SRC:.cpp=.o)

CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -fno-gnu-unique -g3
CPPFLAGS = -I../include/core_interfaces -I./core/include -I./utils

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) -o $(NAME) $(OBJ) -ldl
	@echo "Core compiled successfully"

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
