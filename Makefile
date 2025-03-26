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
           src/managers/ScoreManager.cpp \
           src/managers/GameManager.cpp
CORE_OBJ = $(CORE_SRC:.cpp=.o)
CORE_NAME = arcade

# Game sources
GAME_SRC = # Add game sources here
GAME_LIBS = $(patsubst src/games/%.cpp,lib/arcade_%.so,$(GAME_SRC))

# Graphics sources
GRAPHICS_SRC = src/graphics/NcursesGraphics.cpp
GRAPHICS_LIBS = $(patsubst src/graphics/%.cpp,lib/arcade_%.so,$(GRAPHICS_SRC))

CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++20 -I./include -fPIC -fno-gnu-unique
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

lib/arcade_%.so: src/games/%.cpp
	@mkdir -p lib
	@$(CXX) $(CXXFLAGS) -shared -o $@ $< $(LDFLAGS)
	@echo -e "${GREEN}Game library built: $@${NC}"

graphics: $(GRAPHICS_LIBS)

lib/arcade_%.so: src/graphics/%.cpp
	@mkdir -p lib
	@$(CXX) $(CXXFLAGS) -shared -o $@ $< $(LDFLAGS)
	@echo -e "${GREEN}Graphics library built: $@${NC}"

clean:
	@rm -f $(CORE_OBJ) *.gcda *.gcno
	@echo -e "${YELLOW}LUD'S MAKEFILE | ${RED}Cleaning object and coverage files...${NC}"

fclean: clean
	@rm -f $(CORE_NAME) $(GAME_LIBS) $(GRAPHICS_LIBS)
	@echo -e "${YELLOW}LUD'S MAKEFILE | ${RED}Full cleanup complete. All executables removed.${NC}"

re: fclean all

.PHONY: all clean fclean re core games graphics