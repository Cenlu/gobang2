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

/* cpoint[i][j][2] ��ʾ���Ե÷� */
/* cpoint[i][j][3] ��ʾ��ҵ÷� */
/* cpoint[i][j][0] ��ʾ���Ծ�ɱ����� */
/* cpoint[i][j][1] ��ʾ��Ҿ�ɱ����� */

int cpoint[20][20][4];
const int G5 = 20000;
const int LIVE4 = 2000;
const int C4 = 300; /* ���� */
const int RC4 = 250; /* ������ */
const int LIVE3 = 450;
const int RLIVE3 = 300;
const int LIVE2 = 100;
const int RLIVE2 = 70;
const int M3 = 50;
const int DEAD4 = -10;
const int DEAD3 = -10;
const int DEAD2 = -10;
const int inf = 9000000;
const int unknow = 9900000;
const int DEPTH = 6;
int counter;

HashElem hashtable[HASHSIZE];
LL zobrist[3][20][20];
int state[20][20];
int comy, comx;

LL rand14()
{
	double val = rand()/(RAND_MAX+1.0);
	return (LL)(val * 10e14);
}

void init_zobrist()
{
	int i, j, k;
	for(i = 0; i < 3; i++){
		for(j = 0; j < 20; j++){
			for(k = 0; k < 20; k++){
				zobrist[i][j][k] = rand14();
			}
		}
	}
}

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

/* ����ĳ����һ�������ϵ�����  */

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
		int ok = 0; /* ͬһ��������������γɻ����ͳ��ģ���������  */
		if((lempty[0] == 1 && lchess[1] >= 1) || (rempty[0] == 1 && rchess[1] >= 1)){
			po->rc4++;
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
			po->rc4++;
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
			po->rc4++;
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

/* ����ĳ�����ĳ����ķ���  */
int get_points(Coor *co, int *kill)
{
	Points po;
	Coor tco;
	int player = co->player;
	int y = co->y;
	int x = co->x;
	int ans = 0;
	int nc4, nl3;
	*kill = 0;
	po.g5 = po.rc4 = po.l4 = po.l3 = po.rl3 = po.l2 = po.rl2 = po.c4 = po.m3 = po.d4 = po.d3 = po.d2 = 0;
	tco.player = player;
	tco.y = y;
	tco.x = x;

	cal_chess(&po, &tco, 1, 0);
	cal_chess(&po, &tco, 0, 1);
	cal_chess(&po, &tco, 1, 1);
	cal_chess(&po, &tco, -1, 1);

	nc4 = po.c4 + po.rc4;
	nl3 = po.l3 + po.rl3;
	if(po.g5 >= 1) { /* ���� */
		*kill = 3;
		ans = G5;
	}
	else if(po.l4 >= 1 || nc4 >= 2 || (nc4 && nl3)){ /* ��ɱ */
		*kill = 2;
		ans = LIVE4;
	}
	else{
		if(nl3 >= 2) { /* ˫���� */
			*kill = 1;
		}
		ans = po.l3*LIVE3 + po.rl3*RLIVE3 + po.l2*LIVE2 + po.rl2*RLIVE2 + po.c4*C4 + po.rc4*RC4 + po.m3*M3 + po.d4*DEAD4 + po.d3*DEAD3 + po.d2*DEAD2;
	}
	return ans;
}

/* ����ĳ����ķ���  */
void cal_point(int y, int x)
{
	Coor tco;
	tco.y = y;
	tco.x = x;
	if(state[y][x] == -1){
		tco.player = 0;
		cpoint[y][x][2] = get_points(&tco, &cpoint[y][x][0]); /* ������ */
		tco.player = 1;
		cpoint[y][x][3] = get_points(&tco, &cpoint[y][x][1]); /* ������ */
	}
}

/* ����ĳ�����������ķ���  */
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

/* �ı�� (y, x) 4 �������¿����ӵ�ķ���  */
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

int cal_zobrist()
{
	int z = 0;
	int i, j;
	for(i = 0; i < MAP_SIZE; i++){
		for(j = 0; j < MAP_SIZE; j++){
			if(state[i][j] == -1)
				z ^= zobrist[2][i][j];
			else if(state[i][j] == 0)
				z ^= zobrist[0][i][j];
			else if(state[i][j] == 1)
				z ^= zobrist[1][i][j];
		}
	}
	return z;
}

void init_hashtable()
{
	int i;
	for(i = 0; i < HASHSIZE; i++)
		hashtable[i].val = unknow;
}

int find_in_hash(int depth, int alpha, int beta, LL st)
{
	int p = (st&(HASHSIZE-1));
	int val = hashtable[p].val;
	if(val == unknow)
		return val;
	if(hashtable[p].check == st){
		if(hashtable[p].depth >= depth){
			if(hashtable[p].type == HASHEXACT){
				return val;
			}
			if(hashtable[p].type == HASHALPHA && val <= alpha){
				return alpha;
			}
			if(hashtable[p].type == HASHBETA && val >= beta){
				return beta;
			}
		}
	}
	return unknow;
}

void record_hash(int depth, int val, LL st, Type type)
{
	int p = (st&(HASHSIZE-1));
	hashtable[p].check = st;
	hashtable[p].val = val;
	hashtable[p].depth = depth;
	hashtable[p].type = type;
}

int alpha_beta(int player, int depth, int alpha, int beta, LL st)
{
	int i;
	int val;
	counter++; /* ��¼�����ڵ��� */

#if 0
	if((val = find_in_hash(depth, alpha, beta, st)) != unknow){
		return val;
	}
#endif

	if(depth == 0){ /* �ﵽ����������ƣ����ع��� */
		int s1 = cal_all_points(0);
		int s2 = cal_all_points(1);
		record_hash(depth, s1-s2, st, HASHEXACT);
		return s1 - s2;
	}
	
	if(!player){ /* ���� */
		Subpoints sp[250];
		int n = set_order(sp, player); /* �Ժ�ѡ�㰴�߷ֵ��ͷ����� */
		int oppkill = 0;
		int y, x;
		int hashf = HASHALPHA;
		if(!depth){
			comy = sp[0].y;
			comx = sp[0].x;
		}
		for(i = 0; i < 20 && i < n; i++){ /* ���ѡ�� MAP_SIZE ����ѡ�� */
			y = sp[i].y;
			x = sp[i].x;
			if(sp[i].killw > oppkill)
				oppkill = sp[i].killw; /* �ڱ������н��ʱ���¼�Է���в�� */
#if 1
			/* ������һ������ || (�Է�����һ������ && ������һ���ɾ�ɱ��) ||  ������һ����˫�������Է�����һ���ɾ�ɱ��*/
			if(sp[i].killb == 3 || (oppkill < 3 && sp[i].killb == 2) || (sp[i].killb == 1 && oppkill < 2)){
				if(!depth){
					comy = y;
					comx = x;
				}
				alpha = inf;
				break;
			}
#endif
			state[y][x] = player; /* �� (y, x) ���� */
			st ^= zobrist[0][y][x];
			change_cpoint(y, x); /* (y, x) �ĸ������ϵĵ÷��ܵ�Ӱ�죬��Ҫ�ı�  */
			val = alpha_beta(player^1, depth-1, alpha, beta, st);
			state[y][x] = -1;
			st ^= zobrist[0][y][x];
			change_cpoint(y, x);

			if(val > alpha){
				if(!depth){
					comy = y;
					comx = x;
				}
				hashf = HASHEXACT;
				alpha = val;
			}
			if(alpha >= beta){ /* ǧ���ܰѵȺ�ȥ�������� */
				record_hash(depth, beta, st, HASHBETA);
				return beta;
			}
		}
		record_hash(depth, alpha, st, HASHEXACT);
		return alpha;
	}
	else{ /* ��� */
		Subpoints sp[250];
		int n = set_order(sp, player);
		int oppkill = 0;
		int y, x;
		int hashf = HASHBETA;
		for(i = 0; i < 20 && i < n; i++){
			y = sp[i].y;
			x = sp[i].x;
			if(sp[i].killb > oppkill)
				oppkill = sp[i].killb; /* �ڱ������н��ʱ���¼�Է���в�� */
#if 1
			if(sp[i].killw == 3 || (oppkill < 3 && sp[i].killw == 2) || (sp[i].killw == 1 && oppkill < 2)){
				beta = -inf;
				break;
			}
#endif
			state[y][x] = player;
			st ^= zobrist[1][y][x];
			change_cpoint(y, x);
			val = alpha_beta(player^1, depth-1, alpha, beta, st);
			state[y][x] = -1;
			st ^= zobrist[1][y][x];
			change_cpoint(y, x);


			if(val < beta){
				hashf = HASHEXACT;
				beta = val;
			}

			if(alpha >= beta){
				record_hash(depth, alpha, st, HASHALPHA);
				return alpha;
			}
		}
		record_hash(depth, beta, st, HASHEXACT);
		return beta;
	}
}

int copy_and_cal_points()
{
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
	for(i = 0; i < MAP_SIZE; i++){
		for(j = 0; j < MAP_SIZE; j++){
			if(state[i][j] == -1){
				cal_point(i, j); /* ����ÿ�������ӵ�ķ��� */
			}
		}
	}
	if(!yes) return 0; /* û����䣬���� */
	else return 1;
}


int test(int player)
{
	Subpoints sp[300];
	Coor co, kb, kw;
	int n = 0;
	int i, j;
	int yes;
	int killb, killw;
	yes = copy_and_cal_points();
	if(!yes)
		return 0;
	for(i = 0; i < MAP_SIZE; i++){
		for(j = 0; j < MAP_SIZE; j++){
			int t = state[i][j];
			if(t == -1){
				sp[n].y = i;
				sp[n].x = j;
				co.y = i;
				sp[n].pointb = cpoint[i][j][2];
				sp[n].pointw = cpoint[i][j][3];
				sp[n].killb = cpoint[i][j][0];
				sp[n].killw = cpoint[i][j][1];
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
	killb = killw = 0;
	for(i = 0 ; i < n; i++){
		if(sp[i].killb > killb){
			killb = sp[i].killb;
			kb.y = sp[i].y;
			kb.x = sp[i].x;
		}
		if(sp[i].killw > killw){
			killw = sp[i].killw;
			kw.y = sp[i].y;
			kw.x = sp[i].x;
		}
	}

	if(killb == 3) {comy = kb.y; comx = kb.x;}
	else if(killw == 3) {comy = kw.y; comx = kw.x;}
	else if(killb == 2) {comy = kb.y; comx = kb.x;}
	else if(killw == 2) {comy = kw.y; comx = kw.x;}
	else if(killb == 1) {comy = kb.y; comx = kb.x;}
	else if(killw == 1) {comy = kw.y; comx = kw.x;}
	else {comy = sp[0].y; comx = sp[0].x;}

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
	int yes;
	int st;
	clock_t beg_t;
	clock_t end_t;
	double use_t; /* ��¼����ʱ�� */

	yes = copy_and_cal_points();
	if(!yes)  return 0;

	comy = comx = 0;
	counter = 0;
	move(0, 0);
	printf("                     ");
	move(1, 0);
	printf("                     ");
	move(2, 0);
	printf("                     ");

	init_time();
	init_zobrist();
	init_hashtable();
	st = cal_zobrist();

	beg_t = clock();
	alpha_beta(player, DEPTH, alpha, beta, st); /* ���� */
	end_t = clock();

	use_t = (double)(end_t - beg_t) /  CLOCKS_PER_SEC;
	BLACK_DWHITE;
	move(0, 0);
	printf("state: %d", counter); /* �������ڵ� */
	move(1, 0);
	printf("time:  %.2lf", use_t); /* ������ʱ�� */
	move(2, 0);
	printf("pers:  %.2lf", counter / use_t); /* ƽ���ٶ�: ״̬��/ÿ�� */
	if(map[comy][comx] == EMPTY_POINT){
		draw_out_coor(j_cr, j_cc*2);
		j_cr = comy;
		j_cc = comx;
		set_chose(player); /* ���� */
		draw_coor(j_cr, j_cc*2);
	}
	return 1;
}

