#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "face.h"
#include "fun.h"
#include "computer.h"

Stack stack[5000];
int top;
const char chess[][10] = {
	"¡ñ","¡ð"
};
const char left_up[] = "©³";
const char right_up[] = "©·";
const char left_down[] = "©»";
const char right_down[] = "©¿";
const char edge1[] = "©Ä";
const char edge2[] = "©Ì";
const char edge3[] = "©Ó";
const char edge4[] = "©Û";
const char edge5[] = "©à";
const char edge6[] = "©§";
const char edge7[] = "©¦";
const char edge8[] = "©¥";
const char edge9[] = "¡ª";
const int TRA = 4;
const int VER = 2;
int base_r;
int base_c;
int cur_cr;
int cur_cc;
int j_cr;
int j_cc;
int set_r;
int set_c;
int map[20][20];

const char f1[80] = {
	"©à¡ª©à¡ª©à¡ª©à¡ª©à¡ª©à¡ª©à¡ª©à¡ª©à¡ª©à¡ª©à¡ª©à¡ª©à¡ª©à¡ª©à¡ª©à¡ª©à"
};

const char f2[80] = {
	"©¦  ©¦  ©¦  ©¦  ©¦  ©¦  ©¦  ©¦  ©¦  ©¦  ©¦  ©¦  ©¦  ©¦  ©¦  ©¦  ©¦",
};

const char coor[][10] = {
	"©³©¦©·",
	"¡ª©à¡ª",
	"©»©¦©¿",
};

const char out_coor[][10] = {
	"  ©¦  ",
	"¡ª©à¡ª",
	"  ©¦  ",
};

void draw_coor(int y, int x)
{
	move(y, x);
	printf("%s\n", coor[0]);
	move(y+1, x);
	printf("%s\n", coor[1]);
	move(y+2, x);
	printf("%s\n", coor[2]);
	if(map[j_cr][j_cc] != -1){
		move(y+1, x+2);
		printf("%s", chess[map[j_cr][j_cc]]);
	}
}

void draw_out_coor(int y, int x)
{
	move(y, x);
	printf("%s\n", out_coor[0]);
	move(y+1, x);
	printf("%s\n", out_coor[1]);
	move(y+2, x);
	printf("%s\n", out_coor[2]);
	if(map[j_cr][j_cc] != -1){
		move(y+1, x+2);
		printf("%s", chess[map[j_cr][j_cc]]);
	}
}

void draw_grid(int by, int bx, int r, int c)
{
	int i;
	int cur_r = by;
	int cur_c = bx;
	for(i = 0; i < r; i++){
		move(cur_r+i*2, cur_c);
		printf("%s", f1);
		if(i){
			move(cur_r+i*2-1, cur_c);
			printf("%s", f2);
		}
	}
}

void go_move(int player, int d1, int d2)
{
	draw_out_coor(cur_cr, cur_cc);
	j_cr += d1;
	j_cc += d2;
	cur_cr += d1*VER;
	cur_cc += d2*TRA;
	if(j_cr < 0){
		j_cr = 14;
		cur_cr = base_r + 15 * VER - 1;
	}
	if(j_cr > 14){
		j_cr = 0;
		cur_cr = base_r + 1;
	}
	if(j_cc < 0){
		j_cc = 14;
		cur_cc = base_c + 15 * TRA - 2;
	}
	if(j_cc > 14){
		j_cc = 0;
		cur_cc = base_c + 2;
	}
	draw_coor(cur_cr, cur_cc);
}

int set_chose(int player)
{
	if(map[j_cr][j_cc] == -1){
		move(cur_cr+1, cur_cc+2);
		printf("%s", chess[player]);
		move(1, (cols-6)/2);
		printf("          ");
		move(1, (cols-6)/2);
		printf("[%d, %d]", j_cr+1, j_cc+1);
		map[j_cr][j_cc] = player;
		stack[top].R = cur_cr + 1;
		stack[top].C = cur_cc + 2;
		stack[top].r = j_cr;
		stack[top].c = j_cc;
		stack[top].player = player;
		top++;
#if 0
		move(0, 0);
		for(i = 0; i < 15; i++){
			for(j = 0; j < 15; j++){
				printf("%3d", map[i][j]);
			}
			printf("\n");
		}
#endif
		return 1;
	}
	return 0;
}

void go_back()
{
	if(!top)
		return;
	top--;
	move(stack[top].R, stack[top].C);
	printf("©à");
	map[stack[top].r][stack[top].c] = -1;

	if(!top)
		return;
	top--;
	move(stack[top].R, stack[top].C);
	printf("©à");
	map[stack[top].r][stack[top].c] = -1;

	draw_out_coor(cur_cr, cur_cc);
	cur_cr = stack[top].R - 1;
	cur_cc = stack[top].C - 2;
	j_cr = stack[top].r;
	j_cc = stack[top].c;
	draw_coor(cur_cr, cur_cc);
}

int go_chose(int player)
{
	int ok = 1;
	if(player){
#if 0
		move(lines/2 + 18, cols/2 - 10);
		printf("computer is thinking...");
		ok = test(player);
		move(lines/2 + 18, cols/2 - 10);
		printf("                       ");
#endif
#if 1
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
			else if(key == KEY_ESC)
				go_back();
		}
#endif
	}
	else{
		move(lines/2 + 18, cols/2 - 10);
		printf("computer is thinking...");
		ok = computer_go(player);
#if 0
		ok = test(player);
#endif
		move(lines/2 + 18, cols/2 - 10);
		printf("                       ");
	}
	return ok;
}

void draw_map(Box *map_box)
{
	base_r = map_box->start_row = (lines - playrow) / 2;
	base_c = map_box->start_col = (cols - playcol) / 2;
	map_box->end_row = map_box->start_row + playrow - 1;
	map_box->end_col = map_box->start_col + playcol - 1;
	draw_grid(base_r, base_c, 17, 17);
}

int judge_end(int player, int y, int x)
{
	int i, j;
	int count = 0;
	for(i = x-1; i >= 0; i--) if(map[y][i] == player) count++; else break;
	for(i = x+1; i < 15; i++) if(map[y][i] == player) count++; else break;
	if(count >= 4) return player;
	count = 0;
	for(i = y-1; i >= 0; i--) if(map[i][x] == player) count++; else break;
	for(i = y+1; i < 15; i++) if(map[i][x] == player) count++; else break;
	if(count >= 4) return player;
	count = 0;
	i = y-1; j = x-1;
	while(i >= 0 && j >= 0){
		if(map[i][j] == player) count++;
		else break;
		i--; j--;
	}
	i = y+1; j = x+1;
	while(i < 15 && j < 15){
		if(map[i][j] == player) count++;
		else break;
		i++; j++;
	}
	if(count >= 4) return player;
	count = 0;
	i = y-1; j = x+1;
	while(i >= 0 && j < 15){
		if(map[i][j] == player) count++;
		else break;
		i--; j++;
	}
	i = y+1; j = x-1;
	while(i < 15 && j >= 0){
		if(map[i][j] == player) count++;
		else break;
		i++; j--;
	}
	if(count >= 4) return player;
	return -1;
}

void init_data()
{
	memset(map, -1, sizeof(map));
	j_cr = j_cc = 0;
	top = 0;
}

void go_first(int player)
{
	cur_cr = base_r+8*VER - 1;
	cur_cc = base_c+8*TRA - 2;
	draw_coor(cur_cr, cur_cc);
	move(cur_cr+1, cur_cc+2);
	printf("%s", chess[player]);
	map[7][7] = player;
	j_cr = j_cc = 7;
}

void go_play()
{
	Box map_box;
	int winner = -1;
	int player = 0;
	int key = -1;
	draw_map(&map_box);
	init_data();
#if 1
	go_first(player);
	player ^= 1;
#endif

	while(winner == -1){
		if(!go_chose(player))
			break;
		winner = judge_end(player, j_cr, j_cc);
		player ^= 1;
	}

	move(lines/2 + 18, cols/2 - 7);
	if(winner != -1) printf(" winner: %s ", chess[winner]);
	else printf("- draw -");

	while(key != KEY_ENTER && key != 'q' && key != '\n')
		key = getch();

	clearBox(&map_box);
	move(lines/2 + 18, cols/2 - 7);
	printf("            ");
	move(1, (cols-6)/2);
	printf("          ");
	move(0, 0);
	printf("              ");
	move(1, 0);
	printf("              ");
	move(2, 0);
	printf("                  ");
}

void go_help()
{
	int key = -1;
	Box helpbox;
	int r = 10;
	int c = 22;
	int sr, sc, baser, basec;
	sr = helpbox.start_row = (lines - r) / 2;
	sc = helpbox.start_col = (cols - c) / 2;
	helpbox.end_row = sr + r;
	helpbox.end_col = sc + c;
	drawBox(&helpbox);
	sr++; sc++;
	baser = 1;
	basec = 1;
	move(sr+baser, sc+basec);
	printf("up:      %s", "¡ü");
	move(sr+baser+1, sc+basec);
	printf("down:    %s", "¡ý");
	move(sr+baser+2, sc+basec);
	printf("left:    %s", "¡û");
	move(sr+baser+3, sc+basec);
	printf("right:   %s", "¡ú");
	move(sr+baser+4, sc+basec);
	printf("confirm: [ENTER]");
	move(sr+baser+5, sc+basec);
	printf("back:    [ESC]");
	while(key != 13 && key != '\n' && key != 'q'){
		key = getch();
	}
	clearBox(&helpbox);
}
