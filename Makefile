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
           src/managers/GameManager.cpp \
           src/managers/PlayerManager.cpp
CORE_OBJ = $(CORE_SRC:.cpp=.o)
CORE_NAME = arcade

# Game sources
GAME_SRC = src/games/SnakeGame.cpp \
		   src/games/Minesweeper.cpp

GAME_LIBS = lib/arcade_snake.so lib/arcade_minesweeper.so

# Graphicals sources
GRAPHICALS_SRC = src/graphics/NcursesGraphics.cpp
GRAPHICALS_SRC += src/graphics/SDL2Graphics.cpp
GRAPHICALS_SRC += src/graphics/Allegro5Graphics.cpp
GRAPHICALS_LIBS = lib/arcade_ncurses.so lib/arcade_sdl2.so lib/arcade_allegro5.so

CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++20 -I./include -fPIC -fno-gnu-unique
LDFLAGS = -lallegro -lallegro_font -lallegro_ttf -lallegro_primitives -lncurses -lSDL2 -lSDL2_ttf

# Debug flags
DEBUG_FLAGS = -g3S

# Colors for output
CYAN = \033[0;36m
GREEN = \033[0;32m
BOLD_GREEN = \033[1;32m
RED = \033[1;91m
YELLOW = \033[1;33m
NC = \033[0m

.SILENT:

all: core games graphicals

core: $(CORE_NAME)

$(CORE_NAME): $(CORE_OBJ)
	@$(CXX) -o $(CORE_NAME) $(CORE_OBJ) $(LDFLAGS)
	@echo -e "${YELLOW}LUD'S MAKEFILE | ${BOLD_GREEN}Core program built: $(CORE_NAME)!${NC}"

games: $(GAME_LIBS)

lib/arcade_snake.so: src/games/SnakeGame.cpp
	@mkdir -p lib
	@$(CXX) $(CXXFLAGS) -shared -o $@ $< $(LDFLAGS)
	@echo -e "${GREEN}Game library built: $@${NC}"

lib/arcade_minesweeper.so: src/games/Minesweeper.cpp
	@mkdir -p lib
	@$(CXX) $(CXXFLAGS) -shared -o $@ $< $(LDFLAGS)
	@echo -e "${GREEN}Game library built: $@${NC}"

graphicals: $(GRAPHICALS_LIBS)

lib/arcade_ncurses.so: src/graphics/NcursesGraphics.cpp
	@mkdir -p lib
	@$(CXX) $(CXXFLAGS) -shared -o $@ $< -lncurses
	@echo -e "${GREEN}Graphicals library built: $@${NC}"

lib/arcade_sdl2.so: src/graphics/SDL2Graphics.cpp
	@mkdir -p lib
	@$(CXX) $(CXXFLAGS) -shared -o $@ $< -lSDL2 -lSDL2_ttf
	@echo -e "${GREEN}Graphicals library built: $@${NC}"

lib/arcade_allegro5.so: src/graphics/Allegro5Graphics.cpp
	@mkdir -p lib
	@$(CXX) $(CXXFLAGS) -shared -o $@ $< -lallegro -lallegro_font -lallegro_ttf -lallegro_primitives
	@echo -e "${GREEN}Graphicals library built: $@${NC}"

debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: fclean all
	@echo -e "${YELLOW}LUD'S MAKEFILE | ${BOLD_GREEN}Debug version built with $(DEBUG_FLAGS)${NC}"

valgrind: debug
	@echo -e "${YELLOW}LUD'S MAKEFILE | ${BOLD_GREEN}Running valgrind...${NC}"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./$(CORE_NAME) ./lib/arcade_NcursesGraphics.so

clean:
	@rm -f $(CORE_OBJ) *.gcda *.gcno scores.dat
	@echo -e "${YELLOW}LUD'S MAKEFILE | ${RED}Cleaning object and coverage files...${NC}"

fclean: clean
	@rm -f $(CORE_NAME) $(GAME_LIBS) $(GRAPHICALS_LIBS)
	@echo -e "${YELLOW}LUD'S MAKEFILE | ${RED}Full cleanup complete. All executables removed.${NC}"

re: fclean all

.PHONY: all clean fclean re core games graphicals debug valgrind