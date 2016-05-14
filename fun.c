#include <stdio.h>
#include <conio.h>
#include "curses.h"
#include "face.h"
#include "fun.h"
#include "color.h"
#define MAP_SIZE 15

Stack stack[5000];
char map[MAP_SIZE+5][MAP_SIZE+5];
WINDOW *win;
int j_cr;
int j_cc;
int top;
char EMPTY_POINT = '+';
char BLACK_POINT = 'o';
char WHITE_POINT = 'x';

char *FirstMenu[] = {
	" He ",
	" Me ",
	NULL
};

void draw_map()
{
	box(win, ' ', ' ');
	int r = 0;
	int c = 0;
	for(r = 0; r < MAP_SIZE; r++)
		mvwprintw(win, r, c, "+ + + + + + + + + + + + + + +");
}

void init_date()
{
	int i, j;
	for(i = 0; i < MAP_SIZE; i++)
		for(j = 0; j < MAP_SIZE; j++)
			map[i][j] = EMPTY_POINT;
	j_cr = j_cc = MAP_SIZE / 2;
	top = 0;
}

void draw_coor(int y, int x)
{
	wmove(win, y, x);
	BLACK_WHITE;
	putchar(map[j_cr][j_cc]);
}

void draw_out_coor(int y, int x)
{
	char ch = map[j_cr][j_cc];
	wmove(win, y, x);
	if(ch == EMPTY_POINT){
		BLACK_DWHITE;
	}
	else if(ch == BLACK_POINT){
		BLACK_GREEN;
	}
	else if(ch == WHITE_POINT){
		BLACK_RED;
	}
	putchar(ch);
}

void go_first(int *player)
{
	int p = *player;
	if(!p){ /* 电脑先走，在正中间落子 */
		wmove(win, j_cr, j_cc*2);
		BLACK_GREEN;
		putchar(BLACK_POINT);
		map[j_cr][j_cc] = BLACK_POINT;
		(*player) ^= 1;
	}
	draw_coor(j_cr, j_cc*2);
}

void go_move(int player, int d1, int d2)
{
	draw_out_coor(j_cr, j_cc*2);
	j_cr += d1;
	j_cc += d2;
	if(j_cr < 0){
		j_cr = MAP_SIZE - 1;
	}
	if(j_cr > MAP_SIZE - 1){
		j_cr = 0;
	}
	if(j_cc < 0){
		j_cc = MAP_SIZE - 1;
	}
	if(j_cc > MAP_SIZE - 1){
		j_cc = 0;
	}
	draw_coor(j_cr, j_cc*2);
}

int set_chose(int player)
{
	if(map[j_cr][j_cc] == EMPTY_POINT){
		wmove(win, j_cr, j_cc*2);
		if(player){
			BLACK_RED;
			putchar(WHITE_POINT);
			map[j_cr][j_cc] = WHITE_POINT;
		}
		else{
			BLACK_GREEN;
			putchar(BLACK_POINT);
			map[j_cr][j_cc] = BLACK_POINT;
		}
		stack[top].r = j_cr;
		stack[top].c = j_cc;
		stack[top].player = player;
		top++;
		return 1;
	}
	return 0;
}

int go_chose(int player)
{
	int ok = 1;
	int key = -1;
	while(1){
		key = getch();
		if(key == KEY_UP)
			go_move(player, -1, 0);
		else if(key == KEY_DOWN)
			go_move(player, 1, 0);
		else if(key == KEY_LEFT)
			go_move(player, 0, -1);
		else if(key == KEY_RIGHT)
			go_move(player, 0, 1);
		else if(key == KEY_ENTER){
			if(set_chose(player))
				break;
		}
	}
	return 1;
}

int judge_end(int flag, int y, int x)
{
	int i, j;
	int count = 0;
	char player;
	if(flag)
		player = WHITE_POINT;
	else
		player = BLACK_POINT;

	for(i = x-1; i >= 0; i--) if(map[y][i] == player) count++; else break;
	for(i = x+1; i < MAP_SIZE; i++) if(map[y][i] == player) count++; else break;
	if(count >= 4) return player;
	count = 0;
	for(i = y-1; i >= 0; i--) if(map[i][x] == player) count++; else break;
	for(i = y+1; i < MAP_SIZE; i++) if(map[i][x] == player) count++; else break;
	if(count >= 4) return player;
	count = 0;
	i = y-1; j = x-1;
	while(i >= 0 && j >= 0){
		if(map[i][j] == player) count++;
		else break;
		i--; j--;
	}
	i = y+1; j = x+1;
	while(i < MAP_SIZE && j < 15){
		if(map[i][j] == player) count++;
		else break;
		i++; j++;
	}
	if(count >= 4) return player;
	count = 0;
	i = y-1; j = x+1;
	while(i >= 0 && j < MAP_SIZE){
		if(map[i][j] == player) count++;
		else break;
		i--; j++;
	}
	i = y+1; j = x-1;
	while(i < MAP_SIZE && j >= 0){
		if(map[i][j] == player) count++;
		else break;
		i++; j--;
	}
	if(count >= 4) return player;
	return -1;
}

void go_play()
{
	int winner = -1;
	int key = -1;
	int player = get_select(FirstMenu);
	int begr = (LINES - MAP_SIZE) / 2;
	int begc = (COLS - MAP_SIZE * 2) / 2;
	win = newwin(begr, begc, MAP_SIZE, 30);
	draw_map();
	init_date();
	go_first(&player);

	while(winner == -1){
		if(!go_chose(player))
			break;
		winner = judge_end(player, j_cr, j_cc);
		player ^= 1;
	}

	key = getch();
	while(key != KEY_ENTER && key != '\n' && key != 'q')
		key = getch();

	delwin(win);
	system("color 07");
}

void go_help()
{
	WINDOW *help_win;
	int key = -1;
	int r = 9;
	int c = 20;
	int tr, tc;
	int beg_r = (LINES - r) / 2;
	int beg_c = (COLS - c) / 2;
	help_win = newwin(beg_r, beg_c, r, c);
	box(help_win, '|', '*');

	tr = 1;
	tc = 2;
	mvwprintw(help_win, tr++, tc, "up:      [UP]");
	mvwprintw(help_win, tr++, tc, "down:    [DOWN]");
	mvwprintw(help_win, tr++, tc, "left:    [LEFT]");
	mvwprintw(help_win, tr++, tc, "right:   [RIGHT]");
	mvwprintw(help_win, tr++, tc, "confirm: [ENTER]");
	mvwprintw(help_win, tr++, tc, "back:    [w]");
	mvwprintw(help_win, tr++, tc, "quit:    [q]");

	key = getch();
	while(key != KEY_ENTER && key != '\n' && key != 'q')
		key = getch();

	delwin(help_win);
}

