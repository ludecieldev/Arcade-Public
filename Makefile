##
## EPITECH PROJECT, 2025
## Multi-Tests Project
## File description:
## Makefile
##

SRC     =   src/Main.cpp

OBJ     =   $(SRC:.cpp=.o)
NAME    =   arcade

CXX     =   g++
CXXFLAGS =  -Wall -Wextra -Werror -std=c++20 -I./include
LDFLAGS =   

TEST_BIN =   unit_tests
TEST_SRC =   $(wildcard tests/*.cpp)
CXXFLAGS_TEST = $(CXXFLAGS) --coverage
LDFLAGS_TEST = -lcriterion --coverage

CYAN    =   \033[0;36m
GREEN   =   \033[0;32m
BOLD_GREEN = \033[1;32m
RED     =   \033[1;91m
YELLOW  =   \033[1;33m
NC      =   \033[0m

TOTAL_FILES = $(words $(SRC))
FILES_COMPILED = 0

.SILENT:

all: start_compilation $(NAME) end_compilation

start_compilation:
	@echo -e "${YELLOW}LUD'S MAKEFILE | ${CYAN}Started compilation of ${RED}$(TOTAL_FILES) files${NC}"

$(NAME): $(OBJ)
	@$(CXX) -o $(NAME) $(OBJ) $(LDFLAGS)
	@echo -e "${YELLOW}LUD'S MAKEFILE | ${BOLD_GREEN}Program built: $(NAME)!${NC}"

%.o: %.cpp
	@echo -e -n "${CYAN}Compiling $< $(FILES_COMPILED)/$(TOTAL_FILES)...${NC}\r"
	@$(eval FILES_COMPILED=$(shell echo $$(($(FILES_COMPILED)+1))))
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo -e "${GREEN}Compiled $< $(FILES_COMPILED)/$(TOTAL_FILES)${NC}"

end_compilation:
	@if [ $(FILES_COMPILED) -eq $(TOTAL_FILES) ]; then \
		echo -e "${YELLOW}LUD'S MAKEFILE | ${BOLD_GREEN}All files compiled successfully!${NC}"; \
	else \
		echo -e "${YELLOW}LUD'S MAKEFILE | No new changes to compile.${NC}"; \
	fi

clean:
	@rm -f $(OBJ) *.gcda *.gcno
	@echo -e "${YELLOW}LUD'S MAKEFILE | ${RED}Cleaning object and coverage files...${NC}"

fclean: clean
	@rm -f $(NAME) $(TEST_BIN)
	@echo -e "${YELLOW}LUD'S MAKEFILE | ${RED}Full cleanup complete. All executables removed.${NC}"

re: fclean all

tests_run: $(OBJ)
	@echo "${YELLOW}LUD'S MAKEFILE | ${CYAN}Compiling and running tests...${NC}"
	@$(CXX) -o $(TEST_BIN) $(TEST_SRC) $(CXXFLAGS_TEST) $(LDFLAGS_TEST)
	@./$(TEST_BIN)

coverage:
	@gcovr --exclude tests/
	@echo "${GREEN}Coverage report generated in terminal.${NC}"

coverage_html:
	@gcovr --exclude tests/ --html --html-details -o coverage_report.html
	@echo "${GREEN}Coverage report generated: coverage_report.html${NC}"

help:
	@echo "${BOLD_GREEN}Available targets:${NC}"
	@echo "${BOLD_GREEN}  all: Compiles the program${NC}"
	@echo "${BOLD_GREEN}  clean: Removes object and coverage files${NC}"
	@echo "${BOLD_GREEN}  fclean: Removes object files and executables${NC}"
	@echo "${BOLD_GREEN}  re: Rebuilds the project from scratch${NC}"
	@echo "${BOLD_GREEN}  tests_run: Compiles and runs the unit tests${NC}"
	@echo "${BOLD_GREEN}  coverage: Displays coverage results in terminal${NC}"
	@echo "${BOLD_GREEN}  coverage_html: Generates an HTML coverage report${NC}"
	@echo "${BOLD_GREEN}  help: Displays this help${NC}"

.PHONY: all clean fclean re start_compilation end_compilation help tests_run coverage coverage_html