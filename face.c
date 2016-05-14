#include <stdio.h>
#include <conio.h>
#include "curses.h"
#include "face.h"
#include "color.h"

int playrow = 30;
int playcol = 50;

void draw_menu(WINDOW *win_ptr, char *menu[], int select_row)
{
	char **menu_prt = menu;
	int current_row = 1;
	int beg_col = 1;
	while(*menu_prt){
		wmove(win_ptr, current_row, beg_col);
		if(current_row == select_row + 1){
			WHITE_BLACK;
			printf("%s", *menu_prt);
		}
		else{
			BLACK_DWHITE;
			printf("%s", *menu_prt);
		}
		current_row++;
		menu_prt++;
	}
}

int get_select(char *menu[])
{
	WINDOW *sub_win;
	int sub_row = 0;
	int sub_col = 0;
	int sub_beg_row = 0;
	int sub_beg_col = 0;
	int select_row = 0;
	int selected = 0;
	int max_row = 0;
	int key = -1;
	char **menu_prt = menu;
	int sub_len = strlen(*menu_prt);
	while(*menu_prt){
		max_row++;
		menu_prt++;
	}
	sub_row = max_row + 2;
	sub_col = sub_len + 2;
	sub_beg_row = (LINES - sub_row) / 2;
	sub_beg_col = (COLS - sub_col) / 2;
	sub_win = newwin(sub_beg_row, sub_beg_col, sub_row, sub_col);
	box(sub_win, '|', '*');
	while(key != 13 && key != '\n' && key != 'q'){
		if(KEY_UP == key){
			select_row--;
			if(select_row < 0)
				select_row = max_row - 1;
		}
		if(KEY_DOWN == key){
			select_row++;
			if(select_row >= max_row)
				select_row = 0;
		}
		selected = select_row;
		draw_menu(sub_win, menu, select_row);
		key = getch();
	}
	delwin(sub_win);
	if(key == 'q')
		selected = key;
	return selected;
}
