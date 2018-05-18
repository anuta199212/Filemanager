#include<ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "main.h"
//---------------------------------------------------------------- 
char** directory_read(WINDOW *win, unsigned *items_counter, char** input_items, int c)
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
