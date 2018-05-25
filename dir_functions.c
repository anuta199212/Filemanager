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
int type_select(char** input_file, int c)
{
	struct stat info;
	int ret = 0;

	ret = stat(input_file[c], &info);
	if (ret)
	{
		return(0);
	}
	switch(info.st_mode & S_IFMT)
	{
		case S_IFDIR:
			return(1);
			break;
		default:
			return(2);
			break;
	}
}
//---------------------------------------------------------------- 

FILE* file_editor_open(WINDOW *win, char *editor_buffer, char** input_file, int c, size_t *block_size, size_t *block_count)
{
	FILE *stream;
	size_t real_size = 0;
	int ret = 0;
	char key = 0;
	if (!((stream) = fopen(input_file[c], "r+")))
		return(0);
	real_size = fread((editor_buffer), *block_size, *block_count, stream);
//	if (!(real_size = fread((editor_buffer), *block_size, *block_count, stream)))
//		return(0);
	wrefresh(win);
	keypad(win, TRUE);
	mvwprintw(win, 1,1, " %s", (editor_buffer));
	return(stream);
}

//---------------------------------------------------------------- 
void file_size_determine(WINDOW *win, char** input_file, int c, size_t *block_size, size_t *block_count)
{
	struct stat info;
	int ret = 0;

	ret = stat(input_file[c], &info);
	if (ret)
	{
		return;
	}
	(*block_size) = info.st_blksize;
	(*block_count) = info.st_blocks;
	
}
//---------------------------------------------------------------- 
void file_processing(WINDOW *win, char *editor_buffer, FILE *stream, size_t *block_size, size_t *block_count)
{
	int editor_key = 0;
	int position_x = 1;
	int position_y = 1;
	size_t position_in_buffer = 0;
	while((editor_key = wgetch(win)) != KEY_F(10))	//F10 - quit
	{
		if(!((editor_key >= 32)&&(editor_key <= 126)))
		{	switch(editor_key)
			{
				case KEY_F(1):	//save file
					break;
				case KEY_RIGHT:
	//				if (position_x < (COLS-1))
					if (editor_buffer[position_in_buffer + 1] != '\r')
					{
						position_x++;
						printf(ESC "[%d;%dH", position_y, position_x);
						position_in_buffer++;
					}
					else 
					{
						position_x = 1;
						position_y++;
						printf(ESC "[%d;%dH", position_y, position_x);
						position_in_buffer += 3;
					}
					break;
				case KEY_LEFT:
					if (position_x > 1)
					{
						position_x--;
						printf(ESC "[%d;%dH", position_y, position_x);
						position_in_buffer--;
					}
					else if ((position_x == 1)&&(position_y > 1))
					{
						int a = 0;
						int b = 0;
						int count = 0;
						position_in_buffer -= 3;
						for(int i = position_in_buffer; i>1; i--)
						{
							if ((editor_buffer[i] == '\r')&&(editor_buffer[i+1] == '\n'))
							{	count++;
								if (count == 2)
									b = i+2;
									position_x = b-a;
									break;
								a = i-1;	
							}
						}
						position_y--;
						printf(ESC "[%d;%dH", position_y, position_x);
					}
					break;
				case KEY_UP:
					if (position_y > 1)
					{
						position_y--;
						printf(ESC "[%d;%dH", position_y, position_x);
					}
					break;
				case KEY_DOWN:
					position_y++;
					printf(ESC "[%d;%dH", position_y, position_x);
					break;
					
			}	
			continue;
		}
		
			mvwprintw(win, position_y, position_x, "%c",editor_key);
			position_x++;
			position_in_buffer++;
	}	
	return;
}
//---------------------------------------------------------------- 
void file_editor_close(FILE *stream)
{
	fclose(stream);
}
//---------------------------------------------------------------- 
