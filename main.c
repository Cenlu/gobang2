#include "face.h"
#include "fun.h"

char *Menu[] = {
	" Play ",
	" Help ",
	" Quit ",
	NULL
};

int main()
{
	system("color F0");
	int select;
	initWin(lines, cols);
	hide_cursor();
	do{
		select = getSelect(Menu);
		switch(select){
			case 0 : go_play(); break;
			case 1 : go_help(); break;
		}
	}while(select != 2 && select != 'q');
	return 0;
}


