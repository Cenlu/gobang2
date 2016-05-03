#include "face.h"
#include <windows.h>

int lines = 42;
int cols = 79;
int playrow = 34;
int playcol = 67;

void drawBox(Box *box)  
{  
	int i, len;  
	char str[201];
	char s[201];
    move(box->start_row, box->start_col);  
	choseColor(3);
	len = box->end_col - box->start_col;
	for(i = 0; i < len; i++){
		str[i] = '*';
		s[i] = ' ';
	}
	s[0] = '|';
	s[len-1] = '|';
	s[len] = str[len] = '\0';
	printf("%s", str);
    for(i = box->start_row+1; i < box->end_row; i++){  
		move(i, box->start_col);
		printf("%s", s);
    }  
	move(i-1, box->start_col);
	printf("%s", str);
} 

void clearBox(Box *box)  
{  
	int i, len;  
	char str[201];
    move(box->start_row, box->start_col);  
	choseColor(3);
	len = box->end_col - box->start_col;
	for(i = 0; i < len; i++){
		str[i] = ' ';
	}
	str[len] = '\0';
    for(i = box->start_row; i < box->end_row; i++){  
		move(i, box->start_col);
		printf("%s", str);
    }  
} 

void drawChose(Box *box, int selected, int beg, int end)
{
	int cur_r = box->start_row + 1;
	int cur_c = box->start_col + 1;
	int p = beg;
	while(p <= end){
		move(cur_r, cur_c);
		if(p == selected)
			choseColor(9);
		else
			choseColor(8);
		printf(" %-2d ", p);
		cur_c += 4;
		if(cur_c >= box->end_col - 1){
			cur_r += 1;
			cur_c = box->start_col + 1;
		}
		p++;
	}
}

/* len 为每行选项个数; beg 为起始选项， end 为最终选项 */
int get_chose_one_by_one(int len, int beg, int end)
{
	Box chosebox;
	int each = 4; /* 每个选项 4 个字符 */
	int selected = beg; /* 当前选项 */
	int select_cur = beg; /* 最终选项 */
	int key = -1;
	int cal_r = end - beg + 1;
	int box_col = len * each + 2; /* 选项宽度 + 2 个边界字符 */
	int box_row;
	if(cal_r % len) /* 可组成 cal_r 行选项 */
		cal_r = cal_r / len + 1;
	else cal_r = cal_r / len;
	box_row = cal_r + 2; /* 选项高度 + 2 个边界字符 */
	if(beg < 0 || end > 99) /* 限制大于 0 的两位数 */
		return -1;
	chosebox.start_row = (lines - box_row) / 2;
	chosebox.start_col = (cols - box_col) / 2;
	chosebox.end_row = chosebox.start_row + box_row;
	chosebox.end_col = chosebox.start_col + box_col;
	drawBox(&chosebox);
	while(key != KEY_ENTER && key != '\n'){
		if(KEY_LEFT == key){
			select_cur--;
			if(select_cur < beg)
				select_cur = end;
		}
		if(KEY_RIGHT == key){
			select_cur++;
			if(select_cur > end)
				select_cur = beg;
		}
		selected = select_cur;
		drawChose(&chosebox, select_cur, beg, end);
		key = getch();
	}
	clearBox(&chosebox);
	return selected;
}



void choseColor(int x)
{
	switch(x){
		case 1 : BLACK_RED; break;
		case 2 : BLACK_CYAN; break;
		case 3 : WHITE_BLACK; break;
		case 4 : BLACK_BLUE; break;
		case 5 : BLACK_YELLOW; break;
		case 6 : BLACK_WHITE; break;
		case 7 : BLACK_WHITE; break;
		case 8 : YELLOW_DRED; break;
		case 9 : GREEN_DBLUE; break;
		case 10 : BLACK_PURPLE; break;
		case 11 : BLUE_RED; break;
	}
}

void initWin(int r, int c)
{
	char wsize[100];
	Box playbox;
	lines = r;
	cols = c;
	sprintf(wsize, "mode con cols=%d lines=%d", cols, lines);
	system(wsize);
	system("cls");
#if 0
	Box initbox;
	initbox.start_row = 0;
	initbox.start_col = 0;
	initbox.end_row = lines - 2;
	initbox.end_col = cols;
	drawBox(&initbox);
#endif
	playbox.start_row = (lines - playrow - 2) / 2;
	playbox.start_col = (cols - playcol - 2) / 2;
	playbox.end_row = playbox.start_row + playrow + 1;
	playbox.end_col = playbox.start_col + playcol + 1;
	drawBox(&playbox);
}

void move(int y, int x)
{     
    COORD c;  
    c.X=x; 
    c.Y=y;   
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);   
}  

int getSelect(char *menu[])
{
	Box menubox;
	int select_row = 0;
	int selected = 0;
	int max_row = 0;
	int key = -1;
	char **menu_prt = menu;
	int sub_len = strlen(*menu_prt);
	sub_len += 2;
	while(*menu_prt){
		max_row++;
		menu_prt++;
	}
	menubox.start_row = (lines - (max_row + 2)) / 2;
	menubox.start_col = (cols - sub_len) / 2;
	menubox.end_row = menubox.start_row + (max_row + 2);
	menubox.end_col = menubox.start_col + sub_len;
	drawBox(&menubox);
	while(key != 13 && key != 'q' && key != '\n'){
		if(KEY_UP == key){
			select_row--;
			if(select_row < 0)
				select_row = max_row - 1;
		}
		if(KEY_DOWN == key){
			select_row++;
			if(select_row >= max_row)
				select_row = 0;
		}
		selected = select_row;
		drawMenu(menu, select_row, &menubox);
		key = getch();
	}
	clearBox(&menubox);
	if(key == 'q')
		selected = key;
	return selected;
}

void drawMenu(char *menu[], int select_row, Box *box)
{
	char **menu_prt = menu;
	int current_row = box->start_row + 1;
	while(*menu_prt){
		move(current_row, box->start_col + 1);
		if(current_row == box->start_row + 1 + select_row){
			choseColor(7);
			printf("%s\n", *menu_prt);
		}
		else{
			choseColor(3);
			printf("%s\n", *menu_prt);
		}
		current_row++;
		menu_prt++;
	}
}

void hide_cursor()
{
	CONSOLE_CURSOR_INFO cursor_info = {1, 0}; 
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}
