#ifndef FUN_H
#define FUN_H

#ifndef MAP_SIZE
#define MAP_SIZE 15
#endif

typedef struct{
	int R, C;
	int r, c;
	int player;
}Stack;

extern int j_cr;
extern int j_cc;
extern char EMPTY_POINT;
extern char BLACK_POINT;
extern char WHITE_POINT;
extern char map[][MAP_SIZE+5];

void draw_coor(int y, int x);
void draw_out_coor(int y, int x);
void go_play();
void go_help();
int set_chose(int player);
int judge_end(int player, int y, int x);
int computer_go(int player);
int test(int player);

#endif
