filemanager: dir_functions.o window_config.o main.o 
	gcc dir_functions.o window_config.o main.o -lmenu -lncurses -o filemanager -Wall -Wextra -pedantic
dir_functions.o: dir_functions.c main.h
	gcc -c dir_functions.c -lmenu -lncurses
window_config.o: window_config.c main.h
	gcc -c window_config.c -lmenu -lncurses
main.o: main.c main.h
	gcc -c main.c -lmenu -lncurses
clean: 
	rm -f filemanager dir_functions.o window_config.o main.o 

