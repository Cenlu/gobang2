#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "curses.h"
int LINES = 25;
int COLS = 80;

void cursor(int flag)
{
	CONSOLE_CURSOR_INFO cursor_info = {1, flag}; 
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

void initwin(int r, int c)
{
	char wsize[100];
	LINES = r;
	COLS = c;
	sprintf(wsize, "mode con cols=%d lines=%d", COLS, LINES);
	system(wsize);
	system("cls");
}

void move(int y, int x)
{     
    COORD c;  
    c.X = x; 
    c.Y = y;   
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);   
}  

void wmove(WINDOW *win, int y, int x)
{     
    COORD c;  
    c.X = win->beg_c + x; 
    c.Y = win->beg_r + y;   
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);   
}  

WINDOW *newwin(int begr, int begc, int rlen, int clen)
{
	WINDOW *win = (WINDOW *)malloc(sizeof(WINDOW));
	win->beg_r = begr;
	win->beg_c = begc;
	win->r_len = rlen;
	win->c_len = clen;
	return win;
}

void box(WINDOW *win, char rc, char cc)
{
	char buf1[200];
	char buf2[200];
	int begr = win->beg_r;
	int begc = win->beg_c;
	int rlen = win->r_len;
	int clen = win->c_len;
	int i;
	char fill = ' ';

	for(i = 0; i < clen; i++){
		buf1[i] = cc;
		buf2[i] = fill;
	}
	buf2[0] = rc;
	buf2[clen-1] = rc;
	buf1[clen] = buf2[clen] = '\0';

	move(begr, begc);
	printf("%s", buf1);
	for(i = 1; i < rlen-1; i++){
		move(begr+i, begc);
		printf("%s", buf2);
	}
	move(begr+i, begc);
	printf("%s", buf1);
	move(begr, begc);
}

void clrbox(WINDOW *win)
{
	char buf[200];
	int i;
	int begr = win->beg_r;
	int begc = win->beg_c;
	int rlen = win->r_len;
	int clen = win->c_len;
	char fill = ' ';

	for(i = 0; i < clen; i++)
		buf[i] = fill;
	buf[clen] = '\0';

	for(i = 0; i < rlen; i++){
		move(begr+i, begc);
		printf("%s", buf);
	}
}

void delwin(WINDOW *win)
{
	clrbox(win);
	free(win);
}












