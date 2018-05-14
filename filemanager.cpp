#include<ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <menu.h>
//---------------------------------------------------------------- 
#define WHITE	1
#define CYAN	3
#define RED	4
#define MAGENTA	5
#define YELLOW	6
#define BLACK	7
#define GREEN	8
#define WHITE_BLACK	2
#define MENU_COLOR	9
#define box_on	1
#define box_off	0

WINDOW *create_new_window(int x, int y, int w, int h, int color, int box_state);
char** directory_read2(WINDOW *win, unsigned *items_counter, char** input_items, int c);
char * items_array[] = {"Left panel", "Rigth_panel",  (char *)NULL };
ITEM ** my_items;
//---------------------------------------------------------------- 
int main (void)
{
	WINDOW *window_rigth;
	WINDOW *window_left;
	WINDOW *current_window;
	WINDOW *key_window;
	MENU *my_menu;
	WINDOW *menu_win;
	int n_menuitems;
	int key = 0;
	int position_left = 0;
	int position_right = 0;
	int menu_key = 0;

	char** items_left;
	unsigned items_counter_left = 0;

	char** items_rigth;
	unsigned items_counter_rigth = 0;

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

	n_menuitems = (sizeof(items_array)/sizeof(items_array[0]));
	my_items = (ITEM **) calloc(n_menuitems +1, sizeof(ITEM *));
	for(int i = 0; i < n_menuitems; i++)
		my_items[i] = new_item(items_array[i], " ");
	my_menu = new_menu((ITEM **)my_items);
	menu_win = create_new_window(0, 0, 12, n_menuitems, MENU_COLOR, box_off);
	set_menu_back(my_menu, COLOR_PAIR(MENU_COLOR));
	set_menu_win(my_menu, menu_win);
	set_menu_sub(my_menu, derwin(menu_win, 6 ,12, 3,1));
	set_menu_mark(my_menu, "*");
	wattron(menu_win, COLOR_PAIR(MENU_COLOR));
	mvwprintw(menu_win, 0, 2, "Panel");
	wattroff(menu_win, COLOR_PAIR(MENU_COLOR));

	refresh();
	wrefresh(menu_win);
	
	key_window = create_new_window(0, LINES-1, COLS, 1, MENU_COLOR, box_off);
	window_left = create_new_window(0, 1, COLS/2-1, LINES-2, WHITE, box_on);
	window_rigth = create_new_window(COLS/2, 1, COLS/2-1, LINES-2, WHITE, box_on);
	scrollok(window_left, TRUE);
	scrollok(window_rigth, TRUE);
	current_window = window_left;

	items_left = directory_read2(window_left, &items_counter_left, items_left, 0);
	items_rigth = directory_read2(window_rigth, &items_counter_rigth, items_rigth, 0);

	mvwprintw(key_window, 0, 0, "F3 - open directory	F9 - Panel	F10 - Quit");
	wrefresh(key_window);
	wrefresh(window_left);
	wrefresh(window_rigth);

	while(1)
	{
		key = wgetch(current_window);

		wattroff(current_window, A_STANDOUT);
		if (current_window == window_left)
		{
			mvwprintw(current_window, position_left+1,2,"%s", *(items_left+(position_left)));
		}
		else if (current_window == window_rigth)
		{
			mvwprintw(current_window, position_right+1,2,"%s", *(items_rigth+(position_right)));
		}
		wrefresh(current_window);
		switch(key)
		{
			case KEY_UP:
				if (current_window == window_left)
				{
					position_left--;
					if (position_left < 0)
						position_left = 0;
				}
				else if (current_window == window_rigth)
				{
					position_right--;
					if (position_right < 0)
						position_right = 0;
				}
				break;
			case KEY_DOWN:
				if (current_window == window_left)
				{
					position_left++;
					if (position_left >= items_counter_left)
						position_left = 0;
				}
				else if (current_window == window_rigth)
				{
					position_right++;
					if (position_right >= items_counter_rigth)
						position_right = 0;
				}
				break;
			case KEY_F(3):
				if (current_window == window_left)
				{
					items_left = directory_read2(current_window, &items_counter_left, items_left, position_left);
				position_left = 0;
				}
				else if (current_window == window_rigth)
				{
					items_rigth = directory_read2(current_window, &items_counter_rigth, items_rigth, position_right);
				position_right = 0;
				}
				break;
			case KEY_F(10):
				unpost_menu(my_menu);
				free_menu(my_menu);
				for(int i = 0; i < n_menuitems; i++)
				{
					free_item(my_items[i]);
				}
				for(int i = 0; i<items_counter_left;i++)
					free(items_left[i]);
				free(items_left);
				for(int i = 0; i<items_counter_rigth;i++)
					free(items_rigth[i]);
				free(items_rigth);
				wclear(window_left);
				wclear(window_rigth);
				delwin(window_left);
				delwin(window_rigth);
				clear();
				wbkgd(stdscr, COLOR_PAIR(WHITE_BLACK));
				refresh();
				endwin();
				return(0);
				break;
			case KEY_F(9):
				post_menu(my_menu);
				wclear(key_window);
				mvwprintw(key_window, 0, 0, "F3 - Select item	F10 - Quit");
				wrefresh(key_window);
				while ((menu_key = wgetch(menu_win)) != KEY_F(10))
				{
					switch(menu_key)
					{
						case KEY_F(3):
							if (current_item(my_menu) == my_items[0])
								current_window = window_left;
							else if (current_item(my_menu) == my_items[1])
								current_window = window_rigth;
							break;
						case KEY_DOWN:
							menu_driver(my_menu, REQ_DOWN_ITEM);
							break;
						case KEY_UP:
							menu_driver(my_menu, REQ_UP_ITEM);
							break;
					}
					wrefresh(menu_win);
				}
				unpost_menu(my_menu);
				wattron(menu_win, COLOR_PAIR(MENU_COLOR));
				mvwprintw(menu_win, 0, 2, "Panel");
				wattroff(menu_win, COLOR_PAIR(MENU_COLOR));
				box(window_rigth, 0, 0);
				box(window_left, 0, 0);

				mvwprintw(key_window, 0, 0, "F3 - open directory	F9 - Panel	F10 - Quit");
				wrefresh(key_window);
				refresh();
				wrefresh(menu_win);
				wrefresh(window_rigth);
				wrefresh(window_left);
				break;
		}
	
		wattron(current_window, A_STANDOUT);
		
		if (current_window == window_left)
		{
			mvwprintw(current_window, position_left+1,2,"%s", *(items_left+(position_left)));
		}
		else if (current_window == window_rigth)
		{
			mvwprintw(current_window, position_right+1,2,"%s", *(items_rigth+(position_right)));
		}
		wrefresh(current_window);
		wattroff(current_window, A_STANDOUT);
		wrefresh(current_window);
	}
}
//------------------------------------------------
char** directory_read2(WINDOW *win, unsigned *items_counter, char** input_items, int c)
{
	DIR *d;
	struct dirent *dir;
	struct stat info;
	int count_dir = 0;
	char* start_dir = "."; 
	char* tmp = ".."; 
	int ret = 0;
	int color = 0;
	int attr = 0;
	unsigned item_length = 0;
	unsigned size = 1;

	if (!(*items_counter))
	{
		d = opendir(start_dir);

		input_items = (char**) malloc(size*sizeof(char*));
		input_items[(*items_counter)] = (char*) malloc(sizeof(tmp));
		strcpy(input_items[(*items_counter)], start_dir);
	}
	else 
	{
		d = opendir(input_items[c]);
	}
	if (!d)
	{
		return(input_items);
	}
	ret = chdir(input_items[c]);
	if (ret)
	{
		return(input_items);
	}
	
	for(int i = 1; i<(*items_counter);i++)
		free(input_items[i]);
	(*items_counter) = 0;
	size = 1;
	c = 0;
	wclear(win);	
	box(win, 0, 0);
	strcpy(input_items[*items_counter], tmp);
	wattron(win, A_STANDOUT);
	mvwprintw(win, count_dir + 1,1, " %s",input_items[(*items_counter)]);
	wattroff(win, A_STANDOUT);
	(*items_counter)++;
	wrefresh(win);

	while ((dir = readdir(d)) != NULL){
		if ((!strcmp(dir->d_name, ".")) || (!strcmp(dir->d_name, "..")))
			continue;

		ret = stat(dir->d_name, &info);
		if (ret)
		{
			mvwprintw(win, count_dir, 1, "ERROR in stat!");
			wrefresh(win);
			return(input_items);
		}
		
		switch(info.st_mode & S_IFMT)
		{
			case S_IFBLK:
				color = GREEN;
				attr = A_NORMAL;
				break;
			case S_IFCHR:
				color = RED;
				attr = A_NORMAL;
				break;
			case S_IFDIR:
				color = WHITE;
				attr = A_BOLD;
				break;
			case S_IFIFO:
				color = MAGENTA;
				attr = A_NORMAL;
				break;
			case S_IFLNK:
				color = YELLOW;
				attr = A_NORMAL;
				break;
			case S_IFREG:
				color = CYAN;
				attr = A_NORMAL;
				break;
			case S_IFSOCK:
				color = BLACK;
				attr = A_NORMAL;
				break;
			default:
				color = BLACK;
				attr = A_NORMAL;
				break;
		}
		item_length = sizeof(dir->d_name);
		if ((*items_counter) >= size)
		{
			size++;
			input_items = (char**) realloc(input_items, size*sizeof(char*));
		}
		input_items[(*items_counter)] = (char*) malloc(item_length+1);
		strcpy(input_items[(*items_counter)], dir->d_name);
		(*items_counter)++;
		wattron(win, COLOR_PAIR(color) | attr);
		mvwprintw(win, count_dir + 2,1, " %s", dir->d_name);
		wattroff(win, COLOR_PAIR(color) | attr);
		wrefresh(win);
		count_dir++;
	}
	closedir(d);
	return(input_items);
}
//---------------------------------------------------------------- 
WINDOW *create_new_window(int x, int y, int w, int h, int color, int box_state)
{
	WINDOW *local_win;

	local_win = newwin(h,w,y,x);
	if (box_state)
		box(local_win, 0, 0);

	wrefresh(local_win);
	wbkgd(local_win, COLOR_PAIR(color));

	wrefresh(local_win);
	keypad(local_win, TRUE);
	
	return(local_win);
}



