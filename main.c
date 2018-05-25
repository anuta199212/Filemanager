#include<ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <menu.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "main.h"
//---------------------------------------------------------------- 
char* items_array[] = {"Left panel", "Rigth panel", (char *)NULL};
ITEM** my_items;
//---------------------------------------------------------------- 
int main (void)
{
	WINDOW *window_rigth;		//right panel
	WINDOW *window_left;		//left panel
	WINDOW *current_window;		
	WINDOW *editor_window;		
	WINDOW *key_window;		//window with a description of the keys
	MENU *my_menu;			//menu for selecting current panel
	WINDOW *menu_win;		//menu windowi
	FILE *stream;
	int n_menuitems;		//number of menu items
	int key = 0;			//pressed key
	int editor_key = 0;			//pressed key
	int position_left = 0;		//the current cursor position in the left panel
	int position_right = 0;		//the current cursor position in the right panel
	int menu_key = 0;		//pressed key in menu window
	char** items_left;		//array of directories in the left panel
	unsigned items_counter_left = 0;
	char** items_rigth;		//array of directories in the right panel
	unsigned items_counter_rigth = 0;
	int type = 0;
	char main_keys[] = "F3 - open directory\tF9 - Panel\tF10 - Quit";
	char panel_keys[] = "F3 - Select item\tF10 - Quit";
	char editor_keys[] = "F1 - Save file\tF10 - Quit";
	size_t block_size = 0;
	size_t block_count = 0;
//	char* editor_buffer;

	/*screen initialisation*/
	screen_init();

	/*creating a menu for selecting the panel*/
	n_menuitems = (sizeof(items_array)/sizeof(items_array[0]));
	my_items = (ITEM **) calloc(n_menuitems +1, sizeof(ITEM *));
	for(int i = 0; i < n_menuitems; i++)
		my_items[i] = new_item(items_array[i], " ");
	my_menu = new_menu((ITEM **)my_items);
	menu_win = create_new_window(0, 0, 12, n_menuitems, MENU_COLOR, BOX_OFF);
	set_menu_back(my_menu, COLOR_PAIR(MENU_COLOR));
	set_menu_win(my_menu, menu_win);
	set_menu_sub(my_menu, derwin(menu_win, 6 ,12, 3,1));
	set_menu_mark(my_menu, "*");
	wattron(menu_win, COLOR_PAIR(MENU_COLOR));
	mvwprintw(menu_win, 0, 2, "Panel");
	wattroff(menu_win, COLOR_PAIR(MENU_COLOR));
	refresh();
	wrefresh(menu_win);

	/*create a window with a description of the keys */
	key_window = create_new_window(0, LINES-1, COLS, 1, MENU_COLOR, BOX_OFF);
	window_left = create_new_window(0, 1, COLS/2-1, LINES-2, WHITE, BOX_ON);
	window_rigth = create_new_window(COLS/2, 1, COLS/2-1, LINES-2, WHITE, BOX_ON);
	current_window = window_left;
	
	/*read the current directory and print the list */ 
	items_left = directory_read(window_left, &items_counter_left, items_left, 0);
	items_rigth = directory_read(window_rigth, &items_counter_rigth, items_rigth, 0);

	mvwprintw(key_window, 0, 0, main_keys);
	wrefresh(key_window);
	wrefresh(window_left);
	wrefresh(window_rigth);

	while(1)
	{	/*reading the pressed key*/
		key = wgetch(current_window);
		/*highlighting the first line of the list*/
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
		/*processing of the pressed key*/
		switch(key)
		{
			case KEY_UP:	//move up one item in the directory list
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
			case KEY_DOWN:	//move down one item in the directory list
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

			
			case KEY_F(3):	//open directory
				if (current_window == window_left)
				{
					type = type_select(items_left, position_left);
					switch(type)
					{
						case 1:
							items_left = directory_read(current_window, &items_counter_left, items_left, position_left);
							break;
						case 2:
							wclear(key_window);
							mvwprintw(key_window, 0, 0, editor_keys);
							wrefresh(key_window);
							editor_window = create_new_window(0, 0, COLS-1, LINES-1, WHITE, BOX_OFF);
							curs_set(TRUE);
							file_size_determine(editor_window, items_left, position_left, &block_size, &block_count);
							char editor_buffer[block_size*block_count];
							if (!(stream = file_editor_open(editor_window, editor_buffer, items_left, position_left, &block_size, &block_count)))
								break;

							file_processing(editor_window, editor_buffer, stream, &block_size, &block_count);

							file_editor_close(stream);
							clear();
							wclear(editor_window);
							delwin(editor_window);
							box(window_rigth, 0, 0);
							box(window_left, 0, 0);
							wclear(key_window);
							mvwprintw(key_window, 0, 0, main_keys);
							refresh();
							wrefresh(key_window);
							wrefresh(window_left);
							wrefresh(window_rigth);
							wrefresh(menu_win);
							curs_set(FALSE);
							break;
					}
				position_left = 0;
				}
				else if (current_window == window_rigth)
				{
					items_rigth = directory_read(current_window, &items_counter_rigth, items_rigth, position_right);
				position_right = 0;
				}

				mvwprintw(key_window, 0, 0, main_keys);
				break;
			case KEY_F(10):	//quit the program
				unpost_menu(my_menu);
				free_menu(my_menu);
				for(int i = 0; i < n_menuitems; i++)
					free_item(my_items[i]);
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
				delwin(editor_window);
				clear();
				wbkgd(stdscr, COLOR_PAIR(WHITE_BLACK));
				refresh();
				endwin();
				printf(ESC "[H");
				return(0);
				break;
			case KEY_F(9):	//display the menu
				post_menu(my_menu);
				wclear(key_window);
				mvwprintw(key_window, 0, 0, "F3 - Select item\tF10 - Quit");
				wrefresh(key_window);
				while ((menu_key = wgetch(menu_win)) != KEY_F(10))
				{
					switch(menu_key)
					{
						case KEY_F(3):	//selecting the current panel
							if (current_item(my_menu) == my_items[0])
								current_window = window_left;
							else if (current_item(my_menu) == my_items[1])
								current_window = window_rigth;
							break;
						case KEY_DOWN:	//move down one item in the menu
							menu_driver(my_menu, REQ_DOWN_ITEM);
							break;
						case KEY_UP://move up one item in the menu
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

				mvwprintw(key_window, 0, 0, "F3 - open directory\tF9 - Panel\tF10 - Quit");
				wrefresh(key_window);
				refresh();
				wrefresh(menu_win);
				wrefresh(window_rigth);
				wrefresh(window_left);
				break;
		}
		
		/*highlighting the current line of the list*/
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


