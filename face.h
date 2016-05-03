
/* face.h */
#ifndef FACE_H
#define FACE_H
#include "color.h"
#include <conio.h>

#define KEY_UP    72
#define KEY_DOWN  80
#define KEY_LEFT  75
#define KEY_RIGHT 77
#define KEY_ENTER 13
#define KEY_ESC   27

typedef struct{
	int start_row;
	int start_col;
	int end_row;
	int end_col;
}Box;

extern int cols;
extern int lines;
extern int playrow;
extern int playcol;
extern int get_chose_one_by_one(int len, int beg, int end);
extern void drawChose(Box *box, int selected, int beg, int end);
extern void drawBox(Box *box);
extern void clearBox(Box *box);
extern void choseColor(int x);
extern void move(int y, int x);
extern int getSelect(char *menu[]);
extern void drawMenu(char *menu[], int select_row, Box *box);
extern void hide_cursor();
extern void initWin(int lines, int cols);

#endif
