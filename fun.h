#ifndef FUN_H
#define FUN_H

typedef struct{
	int R, C;
	int r, c;
	int player;
}Stack;

extern int set_r;
extern int set_c;
extern int j_cr;
extern int j_cc;
extern int cur_cr;
extern int cur_cc;
extern int base_r;
extern int base_c;
extern const int TRA;
extern const int VER;
extern Stack stack[];
extern int top;
extern int map[][20];
extern const char chess[][10];

extern void go_play();
extern void go_help();
extern int set_chose(int player);
extern void draw_coor(int y, int x);
extern void draw_out_coor(int y, int x);
extern int judge_end(int player, int y, int x);
#endif
