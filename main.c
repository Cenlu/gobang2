#include <stdio.h>
#include <conio.h>
#include "curses.h"
#include "face.h"
#include "fun.h"
#include "color.h"

char *Menu[] = {
	" Play ",
	" Help ",
	" Quit ",
	NULL
};

int main()
{
	WINDOW *win;
	int winr = 38;
	int winc = 80;
	int begr = (winr - playrow) / 2;
	int begc = (winc - playcol) / 2;
	int select;
	BLACK_DWHITE;
	initwin(winr, winc); /* 初始化窗口大小 */
	cursor(0);
	win = newwin(begr, begc, playrow, playcol);
	box(win, '|', '*'); /* 绘制游戏窗口 */

	do{
		select = get_select(Menu);
		switch(select){
			case 0 : go_play(); break;
			case 1 : go_help(); break;
		}
	}while(select != 2 && select != 'q');

	delwin(win);
	BLACK_DWHITE;
	system("cls");
	cursor(1);
	return 0;
}
