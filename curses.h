#ifndef CURSES_H
#define CURSES_H

#define KEY_UP    72
#define KEY_DOWN  80
#define KEY_LEFT  75
#define KEY_RIGHT 77
#define KEY_ENTER 13
#define KEY_ESC   27

typedef struct{
	int beg_r, beg_c;
	int r_len, c_len;
}WINDOW;

extern int LINES;
extern int COLS;

WINDOW *newwin(int begr, int begc, int rlen, int clen);
void delwin(WINDOW *win);
void box(WINDOW *win, char rc, char cc);
void clrbox(WINDOW *win);
void move(int y, int x);
void wmove(WINDOW *win, int y, int x);
void mvwprintw(WINDOW *win, int r, int c, char *str);
void initwin(int y, int x);
void cursor(int flag);

#endif
