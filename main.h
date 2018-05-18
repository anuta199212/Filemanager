#include <menu.h>
//---------------------------------------------------------------- 
#define WHITE		1
#define CYAN		3
#define RED		4
#define MAGENTA		5
#define YELLOW		6
#define BLACK		7
#define GREEN		8
#define WHITE_BLACK	2
#define MENU_COLOR	9
#define BOX_ON		1
#define BOX_OFF		0
//---------------------------------------------------------------- 
WINDOW *create_new_window(int startx, int starty, int width, int height, int color, int box_state);
char** directory_read(WINDOW *win, unsigned *items_counter, char** input_items, int c);
void screen_init(void);
//---------------------------------------------------------------- 



