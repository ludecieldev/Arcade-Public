##
## EPITECH PROJECT, 2025
## Arcade
## File description:
## Main Makefile
##

all: core games graphicals

core:
	$(MAKE) -C src

games:
	$(MAKE) -C lib_games

graphicals:
	$(MAKE) -C lib_graphics

clean:
	$(MAKE) -C src clean
	$(MAKE) -C lib_games clean
	$(MAKE) -C lib_graphics clean
	rm -f vgcore.*

fclean:
	$(MAKE) -C src fclean
	$(MAKE) -C lib_games fclean
	$(MAKE) -C lib_graphics fclean
	$(RM) arcade

re: fclean all

.PHONY: all core games graphicals clean fclean re
