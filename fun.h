#ifndef FUN_H
#define FUN_H

#define HASHSIZE (1<<21)
#ifndef MAP_SIZE
#define MAP_SIZE 15
#endif
#define ABS(x) \
	((x)<(0)?(-x):(x))

typedef long long LL;
typedef enum {HASHALPHA, HASHBETA, HASHEXACT} Type;

typedef struct{
	int player;
	int y, x;
}Coor;

typedef struct{
	LL check;
	Type type;
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
	int point[2]; /* �õ�������÷� */
	int kill[2]; /* �õ����Σ�3 Ϊ���壬2 Ϊ��ɱ��1 Ϊ˫���� */
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
