#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "fun.h"
#include "face.h"
#include "curses.h"
#include "color.h"
#define JUDGE_EDGE(y, x) \
	((y < 0 || y > 14 || x < 0 || x > 14) ? 0 : 1)

/* cpoint[i][j][2] 表示电脑得分 */
/* cpoint[i][j][3] 表示玩家得分 */
/* cpoint[i][j][0] 表示电脑绝杀棋情况 */
/* cpoint[i][j][1] 表示玩家绝杀棋情况 */

int cpoint[20][20][4];
const int G5 = 20000;
const int LIVE4 = 2000;
const int C4 = 300;
const int LIVE3 = 450;
const int RLIVE3 = 300;
const int LIVE2 = 100;
const int RLIVE2 = 70;
const int M3 = 50;
const int DEAD4 = -10;
const int DEAD3 = -10;
const int DEAD2 = -10;
const int inf = 9000000;
const int DEPTH = 6;
int counter;

typedef struct{
	int player;
	int y, x;
}Coor;

typedef struct{
	int g5;
	int l4, l3, rl3, l2, rl2, d4, d3, d2;
	int c4, m3;
}Points;

typedef struct{
	int y, x;
	int chose;
	int pointb;
	int pointw;
	int killb, killw;
}Subpoints;

int state[20][20];
int comy, comx;

void init_time()
{
	static int flag = 0;
	if(!flag){
		srand((unsigned int)time(NULL));
		flag = 1;
	}
}

int compare(const void* _a, const void* _b)
{
	Subpoints *a = (Subpoints *)_a;
	Subpoints *b = (Subpoints *)_b;
	return ((b->pointb + b->pointw) - (a->pointb + a->pointw));
}

/* 计算某点在一个方向上的棋形  */

void cal_chess(Points *po, Coor *co, int d1, int d2)
{
	int player = co->player;
	int y = co->y;
	int x = co->x;
	int i, j, mid = 0;
	int lchess[2], rchess[2];
	int lempty[2], rempty[2];
	lchess[0] = lchess[1] = lempty[0] = lempty[1] = 0;
	rchess[0] = rchess[1] = rempty[0] = rempty[1] = 0;
	i = y - d1;
	j = x - d2;
	while(JUDGE_EDGE(i, j) && state[i][j] == player) { lchess[0]++; i -= d1; j -= d2; }
	while(JUDGE_EDGE(i, j) && state[i][j] == -1) { lempty[0]++; i -= d1; j -= d2; }
	while(JUDGE_EDGE(i, j) && state[i][j] == player) { lchess[1]++; i -= d1; j -= d2; }
	while(JUDGE_EDGE(i, j) && state[i][j] == -1) { lempty[1]++; i -= d1; j -= d2; }
	i = y + d1;
	j = x + d2;
	while(JUDGE_EDGE(i, j) && state[i][j] == player) { rchess[0]++; i += d1; j += d2; }
	while(JUDGE_EDGE(i, j) && state[i][j] == -1) { rempty[0]++; i += d1; j += d2; }
	while(JUDGE_EDGE(i, j) && state[i][j] == player) { rchess[1]++; i += d1; j += d2; }
	while(JUDGE_EDGE(i, j) && state[i][j] == -1) { rempty[1]++; i += d1; j += d2; }
	mid = lchess[0] + rchess[0] + 1;
	if(mid >= 5) po->g5++; /* ooooo */
	else if(mid == 4){
		if(lempty[0] >= 1 && rempty[0] >= 1) po->l4++; /* xoooox */
		else if(lempty[0] + rempty[0]) po->c4++;
		else po->d4++;
	}
	else if(mid == 3){
		int ok = 0; /* 同一个方向上如果可形成活三和冲四，舍弃活三  */
		if((lempty[0] == 1 && lchess[1] >= 1) || (rempty[0] == 1 && rchess[1] >= 1)){
			po->c4++;
			ok = 1;
		}
		if(!ok && lempty[0] + rempty[0] >= 3 && lempty[0] >= 1 && rempty[0] >= 1){
			po->l3++;
			ok = 1;
		}
		else if(lempty[0] + rempty[0])
			po->m3++;
		else
			po->d3++;
	}
	else if(mid == 2){
		int ok = 0;
		if((lempty[0] == 1 && lchess[1] >= 2) || (rempty[0] == 1 && rchess[1] >= 2)){
			po->c4++;
			ok = 1;
		}
		if(!ok && ((lempty[0] == 1 && lchess[1] == 1 && rempty[0] >= 1 && lempty[1] >= 1)
				|| (rempty[0] == 1 && rchess[1] == 1 && lempty[0] >= 1 && rempty[1] >= 1))){
			po->rl3++;
		}
		else if((lempty[0] == 1 && lchess[1] == 1 && rempty[0] + lempty[1] >= 1)
				|| (rempty[0] == 1 && rchess[1] == 1 && lempty[0] + rempty[1] >= 1))
			po->m3++;
		if(lempty[0] + rempty[0] >= 4 && lempty[0] >= 1 && rempty[0] >= 1)
			po->l2++;
		else if(lempty[0] + rempty[0] == 0)
			po->d2++;
	}
	else if(mid == 1){
		int ok = 0;
		if((lempty[0] == 1 && lchess[1] >= 3) || (rempty[0] == 1 && rchess[1] >= 3)){
			po->c4++;
			ok = 1;
		}
		if(!ok && ((lempty[0] == 1 && lchess[1] == 2 && rempty[0] >= 1 && lempty[1] >= 1)
				|| (rempty[0] == 1 && rchess[1] == 2 && lempty[0] >= 1 && rempty[1] >= 1))){
			po->rl3++;
		}
		else if((lempty[0] == 1 && lchess[1] == 2 && rempty[0] + lempty[1] >= 1)
				|| (rempty[0] == 1 && rchess[1] == 2 && lempty[0] + rempty[1] >= 1))
			po->m3++;
		if((lempty[0] == 1 && lchess[1] == 1 && rempty[0] + lempty[1] >= 3 && rempty[0] >= 1 && lempty[1] >= 1)
				|| (rempty[0] == 1 && rchess[1] == 1 && rempty[1] + lempty[0] >= 3 && lempty[0] >= 1 && rempty[1] >= 1)){
			po->rl2++;
		}
		if((lempty[0] == 2 && lchess[1] == 1 && rempty[0] >= 1 && lempty[1] >= 1)
				|| (rempty[0] == 2 && rchess[1] == 1 && lempty[0] >= 1 && rempty[1] >= 1)){
			po->rl2++;
		}
	}
}

/* 计算某玩家在某个点的分数  */
int get_points(Coor *co, int *kill)
{
	Points po;
	Coor tco;
	int player = co->player;
	int y = co->y;
	int x = co->x;
	int ans = 0;
	*kill = 0;
	po.g5 = po.l4 = po.l3 = po.rl3 = po.l2 = po.rl2 = po.c4 = po.m3 = po.d4 = po.d3 = po.d2 = 0;
	tco.player = player;
	tco.y = y;
	tco.x = x;

	cal_chess(&po, &tco, 1, 0);
	cal_chess(&po, &tco, 0, 1);
	cal_chess(&po, &tco, 1, 1);
	cal_chess(&po, &tco, -1, 1);

	if(po.g5 >= 1) { /* 成五 */
		*kill = 3;
		ans = G5;
	}
	else if(po.l4 >= 1 || po.c4 >= 2 || (po.c4 && (po.l3 || po.rl3))){ /* 绝杀 */
		*kill = 2;
		ans = LIVE4;
	}
	else{
		if(po.l3 + po.rl3 >= 2) { /* 双活三 */
			*kill = 1;
		}
		ans = po.l3*LIVE3 + po.rl3*RLIVE3 + po.l2*LIVE2 + po.rl2*RLIVE2 + po.c4*C4 + po.m3*M3 + po.d4*DEAD4 + po.d3*DEAD3 + po.d2*DEAD2;
	}
	return ans;
}

/* 计算某个点的分数  */
void cal_point(int y, int x)
{
	Coor tco;
	tco.y = y;
	tco.x = x;
	if(state[y][x] == -1){
		tco.player = 0;
		cpoint[y][x][2] = get_points(&tco, &cpoint[y][x][0]); /* 攻击力 */
		tco.player = 1;
		cpoint[y][x][3] = get_points(&tco, &cpoint[y][x][1]); /* 防御力 */
	}
}

/* 计算某玩家整个局面的分数  */
int cal_all_points(int player)
{
	int i, j;
	int ans = 0;
	for(i = 0; i < MAP_SIZE; i++){
		for(j = 0; j < MAP_SIZE; j++){
			if(state[i][j] == -1){
				if(player)
					ans += cpoint[i][j][3];
				else
					ans += cpoint[i][j][2];
			}
		}
	}
	return ans;
}

/* 改变点 (y, x) 4 个方向下可落子点的分数  */
void change_cpoint(int y, int x)
{
	int i, j;
	for(i = 0; i < MAP_SIZE; i++){
		if(state[y][i] == -1){
			cal_point(y, i);
		}
		if(state[i][x] == -1){
			cal_point(i, x);
		}
	}
	for(i = 0; i < MAP_SIZE; i++){
		j = i - (y - x);
		if(0 < j && j < MAP_SIZE && state[i][j] == -1){
			cal_point(i, j);
		}
		j = (y + x) - i;
		if(0 < j && j < MAP_SIZE && state[i][j] == -1){
			cal_point(i, j);
		}
	}
}

int set_order(Subpoints *od, int player)
{
	int i, j;
	int n = 0;
	for(i = 0; i < MAP_SIZE; i++){
		for(j = 0; j < MAP_SIZE; j++){
			if(state[i][j] == -1){
				od[n].y = i;
				od[n].x = j;
				od[n].pointb = cpoint[i][j][2];
				od[n].pointw = cpoint[i][j][3];
#if 0
				if(player){
					if(od[n].pointw >= od[n].pointb)
						od[n].chose = cpoint[i][j][3];
					else
						od[n].chose = cpoint[i][j][2];
				}
				else{
					if(od[n].pointb >= od[n].pointw)
						od[n].chose = cpoint[i][j][2];
					else
						od[n].chose = cpoint[i][j][3];
				}
#endif
				od[n].killb = cpoint[i][j][0];
				od[n].killw = cpoint[i][j][1];
				n++;
			}
		}
	}
#if 1
	qsort(od, n, sizeof(Subpoints), compare);
#endif
	return n;
}

int alpha_beta(int player, int depth, int y, int x, int alpha, int beta)
{
	int i;
	counter++; /* 记录搜索节点数 */
	if(judge_end(player^1, y, x) != EMPTY_POINT){ /* 某方成五，结束返回 */
		return player?inf:-inf;
	}
	if(depth >= DEPTH){ /* 达到搜索深度限制，返回估分 */
		int s1 = cal_all_points(0);
		int s2 = cal_all_points(1);
		return s1 - s2;
	}
	
	if(!player){ /* 电脑 */
		Subpoints sp[250];
		int n = set_order(sp, player); /* 对候选点按高分到低分排序 */
		int oppkill = 0;
		if(!depth){
			comy = sp[0].y;
			comx = sp[0].x;
		}
		for(i = 0; i < MAP_SIZE && i < n; i++){ /* 最多选择 MAP_SIZE 个候选点 */
			int val;
			if(sp[i].killw > oppkill)
				oppkill = sp[i].killw; /* 在遍历可行解的时候记录对方威胁性 */
#if 1
			/* 己方可一步成五 || 对方能一步成五 || 己方可一步成绝杀棋 ||  己方可一步成双活三而对方不能一步成绝杀棋*/
			if(sp[i].killb == 3 || oppkill >= 3 || sp[i].killb == 2 || (sp[i].killb == 1 && oppkill < 2)){
				if(!depth){
					comy = sp[i].y;
					comx = sp[i].x;
				}
				alpha = inf;
				break;
			}
#endif
			state[sp[i].y][sp[i].x] = player; /* 在 (y, x) 落子 */
			change_cpoint(sp[i].y, sp[i].x); /* (y, x) 四个方向上的得分受到影响，需要改变  */
			val = alpha_beta(player^1, depth+1, sp[i].y, sp[i].x, alpha, beta);
			state[sp[i].y][sp[i].x] = -1;
			change_cpoint(sp[i].y, sp[i].x);

			if(val > alpha){
				alpha = val;
				if(!depth){
					comy = sp[i].y;
					comx = sp[i].x;
				}
			}
			if(alpha >= beta){ /* 千万不能把等号去掉！！！ */
				return alpha;
			}
		}
		return alpha;
	}
	else{ /* 玩家 */
		Subpoints sp[250];
		int n = set_order(sp, player);
		int oppkill = 0;
		for(i = 0; i < MAP_SIZE && i < n; i++){
			int val;
			if(sp[i].killb > oppkill)
				oppkill = sp[i].killb; /* 在遍历可行解的时候记录对方威胁性 */
#if 1
			if(sp[i].killw == 3 || oppkill >= 3 || sp[i].killw == 2 || (sp[i].killw == 1 && oppkill < 2)){
				beta = -inf;
				break;
			}
#endif
			state[sp[i].y][sp[i].x] = player;
			change_cpoint(sp[i].y, sp[i].x);
			val = alpha_beta(player^1, depth+1, sp[i].y, sp[i].x, alpha, beta);
			state[sp[i].y][sp[i].x] = -1;
			change_cpoint(sp[i].y, sp[i].x);
			if(val < beta){
				beta = val;
			}
			if(alpha >= beta){
				return beta;
			}
		}
		return beta;
	}
}

int test(int player)
{
	Subpoints sp[300];
	Coor co;
	int n = 0;
	int i, j;
	int yes = 0;
	memset(cpoint, 0, sizeof(cpoint));
	for(i = 0; i < MAP_SIZE; i++){
		for(j = 0; j < MAP_SIZE; j++){
			if(map[i][j] == EMPTY_POINT){
				yes++;
				state[i][j] = -1;
			}
			else if(map[i][j] == BLACK_POINT){
				state[i][j] = 0;
			}
			else{
				state[i][j] = 1;
			}
		}
	}
	if(!yes) return 0;
	for(i = 0; i < MAP_SIZE; i++){
		for(j = 0; j < MAP_SIZE; j++){
			if(state[i][j] == -1){
				cal_point(i, j);
			}
		}
	}
#if 0
	for(i = 0; i < MAP_SIZE; i++){
		move(i, 0);
		for(j = 0; j < MAP_SIZE; j++){
			int t;
			if(cpoint[i][j][2] > cpoint[i][j][3])
				t = cpoint[i][j][2];
			else
				t = cpoint[i][j][3];
			printf("%5d", t);
		}
	}
#endif
	for(i = 0; i < MAP_SIZE; i++){
		for(j = 0; j < MAP_SIZE; j++){
			int t = state[i][j];
			if(t == -1){
				sp[n].y = i;
				sp[n].x = j;
				co.y = i;
				co.x = j;
				co.player = 0;
				sp[n].pointb = get_points(&co, &sp[n].killb);
				co.player = 1;
				sp[n].pointw = get_points(&co, &sp[n].killw);
#if 0
				if(sp[n].pointb >= sp[n].pointw)
					sp[n].chose = sp[n].pointb;
				else
					sp[n].chose = sp[n].pointw;
#endif
				n++;
			}
		}
	}
	qsort(sp, n, sizeof(Subpoints), compare);
	comy = sp[0].y;
	comx = sp[0].x;
	if(map[comy][comx] == EMPTY_POINT){
		draw_out_coor(j_cr, j_cc*2);
		j_cr = comy;
		j_cc = comx;
		set_chose(player);
		draw_coor(j_cr, j_cc*2);
	}
	return 1;
}

int computer_go(int player)
{
	int alpha = -inf;
	int beta = inf;
	int i, j;
	int yes = 0;
	clock_t beg_t;
	clock_t end_t;
	double use_t; /* 记录搜索时间 */
	memset(cpoint, 0, sizeof(cpoint));
	for(i = 0; i < MAP_SIZE; i++){
		for(j = 0; j < MAP_SIZE; j++){
			if(map[i][j] == EMPTY_POINT){
				yes++;
				state[i][j] = -1;
			}
			else if(map[i][j] == BLACK_POINT){
				state[i][j] = 0;
			}
			else{
				state[i][j] = 1;
			}
		}
	}
	for(i = 0; i < MAP_SIZE; i++){
		for(j = 0; j < MAP_SIZE; j++){
			if(state[i][j] == -1){
				cal_point(i, j); /* 计算每个可落子点的分数 */
			}
		}
	}
	if(!yes) return 0; /* 没棋可落，和棋 */
	comy = comx = 0;
	counter = 0;
	move(0, 0);
	printf("                     ");
	move(1, 0);
	printf("                     ");
	move(2, 0);
	printf("                     ");

	beg_t = clock();
	alpha_beta(player, 0, j_cr, j_cc, alpha, beta); /* 搜索 */
	end_t = clock();

	use_t = (double)(end_t - beg_t) /  CLOCKS_PER_SEC;
	BLACK_DWHITE;
	move(0, 0);
	printf("state: %d", counter); /* 总搜索节点 */
	move(1, 0);
	printf("time:  %.2lf", use_t); /* 总搜索时间 */
	move(2, 0);
	printf("pers:  %.2lf", counter / use_t); /* 平均速度: 状态数/每秒 */
	if(map[comy][comx] == EMPTY_POINT){
		draw_out_coor(j_cr, j_cc*2);
		j_cr = comy;
		j_cc = comx;
		set_chose(player); /* 落子 */
		draw_coor(j_cr, j_cc*2);
	}
	return 1;
}

