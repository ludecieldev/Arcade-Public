##
## EPITECH PROJECT, 2025
## Multi-Tests Project
## File description:
## Makefile
##

# Core sources
CORE_SRC = src/Main.cpp \
           src/utils/DLLoader.cpp \
           src/utils/Error.cpp \
           src/Core.cpp \
           src/managers/LibraryManager.cpp \
           src/managers/ScoreManager.cpp
CORE_OBJ = $(CORE_SRC:.cpp=.o)
CORE_NAME = arcade

# Game sources
GAME_SRC =  # Add more game sources here
GAME_LIBS = $(GAME_SRC:.cpp=.so)

# Graphics sources
GRAPHICS_SRC = # Add more graphics sources here
GRAPHICS_LIBS = $(GRAPHICS_SRC:.cpp=.so)

CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++20 -I./include -fPIC
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lncurses -lSDL2

# Colors for output
CYAN = \033[0;36m
GREEN = \033[0;32m
BOLD_GREEN = \033[1;32m
RED = \033[1;91m
YELLOW = \033[1;33m
NC = \033[0m

.SILENT:

all: core games graphics

core: $(CORE_NAME)

$(CORE_NAME): $(CORE_OBJ)
	@$(CXX) -o $(CORE_NAME) $(CORE_OBJ) $(LDFLAGS)
	@echo -e "${YELLOW}LUD'S MAKEFILE | ${BOLD_GREEN}Core program built: $(CORE_NAME)!${NC}"

games: $(GAME_LIBS)

%.so: %.cpp
	@$(CXX) $(CXXFLAGS) -shared -o $@ $<
	@echo -e "${GREEN}Game library built: $@${NC}"

graphics: $(GRAPHICS_LIBS)

%.so: %.cpp
	@$(CXX) $(CXXFLAGS) -shared -o $@ $<
	@echo -e "${GREEN}Graphics library built: $@${NC}"

clean:
	@rm -f $(CORE_OBJ) *.gcda *.gcno
	@echo -e "${YELLOW}LUD'S MAKEFILE | ${RED}Cleaning object and coverage files...${NC}"

fclean: clean
	@rm -f $(CORE_NAME) $(GAME_LIBS) $(GRAPHICS_LIBS)
	@echo -e "${YELLOW}LUD'S MAKEFILE | ${RED}Full cleanup complete. All executables removed.${NC}"

re: fclean all

.PHONY: all clean fclean re core games graphics