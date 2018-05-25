#include<ncurses.h>
#include <stdlib.h>
#include <menu.h>
#include "main.h"

//---------------------------------------------------------------- 
WINDOW *create_new_window(int startx, int starty, int width, int height, int color, int box_state)
{
	WINDOW *local_win;

	local_win = newwin(height,width,starty,startx);
	if (box_state)
		box(local_win, 0, 0);

	wrefresh(local_win);
	wbkgd(local_win, COLOR_PAIR(color));

	scrollok(local_win, TRUE);
	idlok(local_win, TRUE);
	wsetscrreg(local_win, starty, height);
	keypad(local_win, TRUE);
	wrefresh(local_win);
	
	return(local_win);
}
//---------------------------------------------------------------- 
void screen_init(void)
{
	initscr();	
	start_color();
	init_pair(WHITE, COLOR_WHITE, COLOR_BLUE);
	init_pair(WHITE_BLACK, COLOR_WHITE, COLOR_BLACK);
	init_pair(CYAN, COLOR_CYAN, COLOR_BLUE);
	init_pair(RED, COLOR_RED, COLOR_BLUE);
	init_pair(MAGENTA, COLOR_MAGENTA, COLOR_BLUE);
	init_pair(YELLOW, COLOR_YELLOW, COLOR_BLUE);
	init_pair(GREEN, COLOR_GREEN, COLOR_BLUE);
	init_pair(BLACK, COLOR_BLACK, COLOR_BLUE);
	init_pair(GREEN, COLOR_GREEN, COLOR_BLUE);
	init_pair(MENU_COLOR, COLOR_BLACK, COLOR_CYAN);
	cbreak();
	keypad(stdscr, TRUE);
	scrollok(stdscr, TRUE);
	noecho();
	curs_set(FALSE);
	wbkgd(stdscr, COLOR_PAIR(WHITE));
	refresh();
}
//---------------------------------------------------------------- 



