#ifndef FUN_H
#define FUN_H

#define HASHSIZE (1<<25)
#ifndef MAP_SIZE
#define MAP_SIZE 15
#endif

typedef long long LL;

typedef struct{
	int player;
	int y, x;
}Coor;

typedef struct{
	LL check;
	enum {HASHALPHA, HASHBETA, HASHEXACT} Type;
	int val;
	int depth;
}HashElem;

typedef struct{
	int g5;
	int l4, l3, rc4, rl3, l2, rl2, d4, d3, d2;
	int c4, m3;
}Points;

typedef struct{
	int y, x;
	int chose;
	int pointb; /* 该点黑棋得分 */
	int pointw;
	int killb; /* 该点黑棋情形，3 为成五，2 为绝杀，1 为双活三 */
	int killw;
}Subpoints;


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
char judge_end(int player, int y, int x);
int set_chose(int player);
int computer_go(int player);
int test(int player);

#endif
