#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "fun.h"
#include "face.h"

const int G5 = 100000;
const int LIVE4 = 20000;
const int DOUBLE_C4 = 20000;
const int C4_LIVE3 = 20000;
const int DOUBLE_LIVE3 = 5000;
const int LIVE3_M3 = 1000;
const int LIVE3 = 300;
const int RLIVE3 = 150;
const int DOUBLE_LIVE2 = 100;
const int LIVE2 = 50;
const int DEAD4 = -10;
const int DEAD3 = -10;
const int DEAD2 = -10;
const int C4 = 500;
const int M3 = 50;
const int inf = 9000000;
const int DEPTH = 6;
int counter;

typedef struct{
	int g5;
	int l4, l3, rl3, l2, d4, d3, d2;
	int dou_c4, c4_l3, dou_l3, c4, m3;
}Points;

typedef struct{
	int y, x;
	int pointsb;
	int pointsw;
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
	return (b->pointsb+b->pointsw) - (a->pointsb+a->pointsw);
}

int get_f(char str[][20])
{
	int i, j, k, u = 0;
	for(i = 0; i < 15; i++){
		int len = 0;
		for(j = 0; j < 15; j++){
			int t = state[i][j];
			if(t == -1)
				str[u][len++] = 'x';
			else
				str[u][len++] = '0' + t;
		}
		str[u][len] = '\0';
		u++;
	}
	/*-------------------------------------------------------*/
	for(i = 0; i < 15; i++){
		int len = 0;
		for(j = 0; j < 15; j++){
			int t = state[j][i];
			if(t == -1)
				str[u][len++] = 'x';
			else
				str[u][len++] = '0' + t;
		}
		str[u][len] = '\0';
		u++;
	}
	/*-------------------------------------------------------*/
	for(j = 0; j < 11; j++){
		int len = 0;
		for(k = 0; j+k < 15; k++){
			int t = state[k][j+k];
			if(t == -1)
				str[u][len++] = 'x';
			else
				str[u][len++] = '0' + t;
		}
		str[u][len] = '\0';
		u++;
		/*----------------------------------------------*/
		if(j != 10){
			len = 0;
			for(k = 0; j+k+1 < 15; k++){
				int t = state[j+k+1][k];
				if(t == -1)
					str[u][len++] = 'x';
				else
					str[u][len++] = '0' + t;
			}
			str[u][len] = '\0';
			u++;
		}
	}
	/*---------------------------------------------------------*/
	for(j = 14; j >=4; j--){
		int len = 0;
		for(k = 0; k < 15 && j-k >= 0; k++){
			int t = state[k][j-k];
			if(t == -1)
				str[u][len++] = 'x';
			else
				str[u][len++] = '0' + t;
		}
		str[u][len] = '\0';
		u++;
		/*----------------------------------------------*/
		if(j != 4){
			len = 0;
			for(k = 14; 29-j-k < 15; k--){
				int t = state[29-j-k][k];
				if(t == -1)
					str[u][len++] = 'x';
				else
					str[u][len++] = '0' + t;
			}
			str[u][len] = '\0';
			u++;
		}
	}
	return u;
}

int get_lines(char str[][20], int y, int x, int flag)
{
	int i, j;
	int n, len;
	state[y][x] = flag;
	/* --------------------------------------------------- */
	n = 0;
	len = 0;
	for(i = 0; i < 15; i++){
		int t = state[y][i];
		if(t == -1) str[n][len++] = 'x';
		else str[n][len++] = '0' + t;
	}
	str[n][len] = '\0';
	n++;
	/* --------------------------------------------------- */
	len = 0;
	for(i = 0; i < 15; i++){
		int t = state[i][x];
		if(t == -1) str[n][len++] = 'x';
		else str[n][len++] = '0' + t;
	}
	str[n][len] = '\0';
	n++;
	/* --------------------------------------------------- */
	i = y;
	j = x;
	while(j < 14 && i > 0){i--; j++;}
	len = 0;
	for(; i < 15 && j >= 0; i++, j--){
		int t = state[i][j];
		if(t == -1) str[n][len++] = 'x';
		else str[n][len++] = '0' + t;
	}
	str[n][len] = '\0';
	n++;
	/* --------------------------------------------------- */
	i = y;
	j = x;
	while(i > 0 && j > 0){i--; j--;}
	len = 0;
	for(; i < 15 && j < 15; i++, j++){
		int t = state[i][j];
		if(t == -1) str[n][len++] = 'x';
		else str[n][len++] = '0' + t;
	}
	str[n][len] = '\0';
	n++;
	state[y][x] = -1; 
	return n;
}

int get_points(char str[][20], int player, int n, int *kill)
{
	int dir[80];
	Points po;
	int i, j;
	int ans = 0;
	char me = '0'+player;
	char opp = '0'+(player^1);
	int beg, end, count, flag, len;
	*kill = 0;
	po.dou_c4 = po.c4_l3 = po.dou_l3 = po.g5 = po.l4 = po.l3 = po.rl3 = po.l2 = po.c4 = po.m3 = po.d4 = po.d3 = po.d2 = 0;
	memset(dir, 0, sizeof(dir));
	for(i = 0; i < n; i++){
		flag = 0;
		len = strlen(str[i]);
		count = 0;
		if(len < 5)
			continue;
		for(j = 0; j < len; j++){
			char ch = str[i][j];
			if(ch == me){
				count++;
				if(!flag){
					flag = 1;
					beg = end = j;
				}
			}
			if((ch != me && flag == 1) || (j == (len-1) && str[i][j] == me)){
				int l, r, lc, rc;
				if(j == len - 1) end = j;
				else end = j - 1;
				l = beg-1;
				r = end+1;
				lc = 0;
				rc = 0;
				if(count >= 5) po.g5++;
				else if(count == 4){
					while(l >= 0 && str[i][l] == 'x') {lc++; l--;}
					while(r < 15 && str[i][r] == 'x') {rc++; r++;}
					if(lc >= 1 && rc >= 1) po.l4++;
					else if(lc + rc >= 1) po.c4++;
					else po.d4++;
				}
				else if(count == 3){
					while(l >= 0 && str[i][l] == 'x') {lc++; l--;}
					while(r < 15 && str[i][r] == 'x') {rc++; r++;}
					if(lc + rc >= 3 && lc >= 1 && rc >= 1){po.l3++; dir[i] = 1;}
					else if(lc + rc == 2) po.m3++;
					else po.d3++;
				}
				else if(count == 2){
					while(l >= 0 && str[i][l] == 'x') {lc++; l--;}
					while(r < 15 && str[i][r] == 'x') {rc++; r++;}
					if(lc + rc >= 4 && lc >= 1 && rc >= 1) po.l2++;
					if(rc == 1 && lc >= 1 && r < 14 && str[i][r] == me && str[i][r+1] == 'x'){
						po.rl3++;
						dir[i] = 1;
					}
					else if(lc == 1 && rc >= 1 && l > 0 && str[i][l] == me && str[i][l-1] == 'x'){
						po.rl3++;
						dir[i] = 1;
					}
					else if(lc + rc <= 3) po.d2++;
				}
				flag = count = 0;
			}
			
		}
	}
#if 1
	for(i = 0; i < n; i++){
		len = strlen(str[i]);
		for(beg = 0; beg+4 < len; beg++){
			int k;
			end = beg + 4;
			count = 0;
			flag = -1;
			for(k = beg; k <= end; k++){
				if(str[i][k] == opp) break;
				else if(str[i][k] == me) count++;
				else if(str[i][k] == 'x') flag = k;
			}
			if(count == 4 && flag != -1 && flag != beg && flag != end && dir[i] == 0)
				po.c4++;
		}
	}
#endif
	ans = po.g5*G5 + po.l4*LIVE4 + po.l3*LIVE3 + po.l2*LIVE2 + po.c4*C4 + po.m3*M3 + po.d4*DEAD4 + po.d3*DEAD3 + po.d2*DEAD2;
	if(po.g5 >= 1 || po.l4 >= 1) *kill = 1;
	if(po.c4 >= 2){ans += DOUBLE_C4; po.dou_c4++; *kill = 1;}
	if(po.c4 && (po.l3 || po.rl3)){ans += C4_LIVE3; po.c4_l3++; *kill = 1;}
	if(po.l3 + po.rl3 >= 2){ans += DOUBLE_LIVE3; po.dou_l3++; *kill = 1;}
	if(po.m3 && (po.l3 || po.rl3)) ans += LIVE3_M3;
	return ans;
}

int alpha_beta(int player, int depth, int y, int x, int alpha, int beta)
{
	char str1[6][20];
	char str2[6][20];
	Subpoints sp[300];
	int i, j;
	int n = 0;
	counter++;
#if 0
	printf("%d\r", counter);
#endif
	if(judge_end(player^1, y, x) != -1){
		return player?inf:-inf;
	}
	if(depth >= DEPTH){
		char str[80][20];
		int u = get_f(str);
		int killb, killw;
		int s1 = get_points(str, 0, u, &killb);
		int s2 = get_points(str, 1, u, &killw);
		return s1 - s2;
	}
	for(i = 0; i < 15; i++){
		for(j = 0; j < 15; j++){
			if(state[i][j] == -1){
				sp[n].y = i;
				sp[n].x = j;
				get_lines(str1, i, j, 0);
				get_lines(str2, i, j, 1);
				sp[n].pointsb = get_points(str1, 0, 4, &sp[n].killb);
				sp[n].pointsw = get_points(str2, 1, 4, &sp[n].killw);
				n++;
			}
		}
	}
	if(!player){
#if 0
		for(i = 0; i < n; i++)
			sp[i].pointsb *= 1.4;
#endif
		qsort(sp, n, sizeof(Subpoints), compare);
		if(!depth){
			comy = sp[0].y;
			comx = sp[0].x;
		}
		for(i = 0; i < 7 && i < n; i++){
			int val;
#if 1
			if(sp[i].pointsb >= LIVE4 || sp[i].pointsw >= LIVE4 || sp[i].killb){
				if(!depth){
					comy = sp[i].y;
					comx = sp[i].x;
				}
				alpha = inf;
				break;
			}
#endif
			state[sp[i].y][sp[i].x] = player;
			val = alpha_beta(player^1, depth+1, sp[i].y, sp[i].x, alpha, beta);
			state[sp[i].y][sp[i].x] = -1;
			if(val > alpha){
				alpha = val;
				if(!depth){
					comy = sp[i].y;
					comx = sp[i].x;
				}
			}
			if(alpha > beta){
				return alpha;
			}
		}
		return alpha;
	}
	else{
#if 0
		for(i = 0; i < n; i++)
			sp[i].pointsw *= 1.4;
#endif
		qsort(sp, n, sizeof(Subpoints), compare);
		for(i = 0; i < 7 && i < n; i++){
			int val;
#if 1
			if(sp[i].pointsw >= LIVE4 || sp[i].pointsb >= LIVE4 || sp[i].killw){
				if(!depth){
					comy = sp[i].y;
					comx = sp[i].x;
				}
				alpha = inf;
				break;
			}
#endif
			state[sp[i].y][sp[i].x] = player;
			val = alpha_beta(player^1, depth+1, sp[i].y, sp[i].x, alpha, beta);
			state[sp[i].y][sp[i].x] = -1;
			if(val < beta){
				beta = val;
			}
			if(alpha > beta){
				return beta;
			}
		}
		return beta;
	}
}

int test(int player)
{
	char str1[6][20];
	char str2[6][20];
	Subpoints sp[300];
	int n = 0;
	int i, j;
	int yes = 0;
	for(i = 0; i < 15; i++){
		for(j = 0; j < 15; j++){
			state[i][j] = map[i][j];
			if(map[i][j] == -1) yes++;
		}
	}
	if(!yes) return 0;
	for(i = 0; i < 15; i++){
		for(j = 0; j < 15; j++){
			int t = state[i][j];
			if(t == -1){
				sp[n].y = i;
				sp[n].x = j;
				get_lines(str1, i, j, 0);
				get_lines(str2, i, j, 1);
				sp[n].pointsb = get_points(str1, 0, 4, &sp[n].killb);
				sp[n].pointsw = 0.7 * get_points(str2, 1, 4, &sp[n].killw);
				n++;
			}
		}
	}
	qsort(sp, n, sizeof(Subpoints), compare);
	comy = sp[0].y;
	comx = sp[0].x;
	if(map[comy][comx] == -1){
		draw_out_coor(cur_cr, cur_cc);
		j_cr = comy;
		j_cc = comx;
		cur_cr = base_r+(comy+1)*VER-1;
		cur_cc = base_c+(comx+1)*TRA-2;
		set_chose(player);
		draw_coor(cur_cr, cur_cc);
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
	double use_t;
	for(i = 0; i < 15; i++){
		for(j = 0; j < 15; j++){
			state[i][j] = map[i][j];
			if(map[i][j] == -1) yes++;
		}
	}
	if(!yes) return 0;
	comy = comx = 0;
	counter = 0;
	move(0, 0);
	printf("             ");
	move(1, 0);
	printf("             ");
	move(2, 0);
	printf("                 ");

	beg_t = clock();
	alpha_beta(player, 0, j_cr, j_cc, alpha, beta);
	end_t = clock();

	use_t = (double)(end_t - beg_t) /  CLOCKS_PER_SEC;
	move(0, 0);
	printf("state: %d", counter);
	move(1, 0);
	printf("time:  %.2lf", use_t);
	move(2, 0);
	printf("pers:  %.2lf", counter / use_t);
	if(map[comy][comx] == -1){
		draw_out_coor(cur_cr, cur_cc);
		j_cr = comy;
		j_cc = comx;
		cur_cr = base_r+(comy+1)*VER-1;
		cur_cc = base_c+(comx+1)*TRA-2;
		set_chose(player);
		draw_coor(cur_cr, cur_cc);
	}
	else{
		move(1, cols-15);
		printf("[%d, %d], %d", comy+1, comx+1, map[comy][comx]);
	}
	return 1;
}
