#pragma once
#include"Screen.h"

ScreenManager screen;

void GameOn(int x, int y, int move_x, int move_y);
void printChessplate();
void printcheck();

int turn = -1;
bool gameon = true;
int gamestate = 0;
int cr_x = 0, cr_y = 0;
int px=-1, py=-1, pmove_x=-1, pmove_y=-1;


void GameOn(int x, int y, int move_x, int move_y) {
	Chess* save;

	if (turn == 1) {
		if (chessplate[y][x]->team == 'a') {
			if (move(x, y, move_x, move_y)) {
				save = chessplate[move_y][move_x];
				moveaction(x, y, move_x, move_y);
				if (ischeck('a') == false) {
					turn *= -1;
					if (ischeck('b')) {
						if (ischeckmate('b')) {
							gameon = false;
						}
					}
				}
				else {
					chessplate[y][x] = chessplate[move_y][move_x];
					chessplate[move_y][move_x] = save;
					chessplate[y][x]->x = x;
					chessplate[y][x]->y = y;
					save = NULL;
				}
			}
		}
	}
	else if (turn == -1)
	{
		if (chessplate[y][x]->team == 'b') {
			if (move(x, y, move_x, move_y)) {
				save = chessplate[move_y][move_x];
				moveaction(x, y, move_x, move_y);
				if (ischeck('b') == false) {
					turn *= -1;
					if (ischeck('a')) {
						if (ischeckmate('a')) {
							gameon = false;
						}
					}
				}
				else {
					chessplate[y][x] = chessplate[move_y][move_x];
					chessplate[move_y][move_x] = save;
					chessplate[y][x]->x = x;
					chessplate[y][x]->y = y;
					save = NULL;
				}

			}
		}
	}
}
int changecolor(int a, int b) {
	int x = 3;
	int y = 4;
	int point = 7;
	int select1 = 3;

	if (chessplate[b][a]->team == 'a') {
		if (a == cr_x && b == cr_y)
			return x + 10*point;
		else if (a == px && b == py)
			return x + 10*select1;
		else
			return x;
	}
	else if (chessplate[b][a]->team == 'b') {
		if (a == cr_x && b == cr_y)
			return y + 10*point;
		else if (a == px && b == py)
			return y + 10*select1;
		else
			return y;
	}
	else {
		if (a == cr_x && b == cr_y)
			return 10*point;
		else if (a == px && b == py)
			return 10*select1;
		else {
			return 15;
		}
	}
}
void printChessplate() {
	screen.Clear(15);
	for (int a = 0; a < 8; a++) {
		for (int b = 0; b < 8; b++) {
			screen.Add(a*2, b, changecolor(a,b) , chessplate[b][a]->name);
		}
	}
	
	printcheck();
	screen.Show();
}

void printcheck() {
	screen.Clear(8, 15);
	if (ischeck('a')) {
		if (ischeckmate('a')) {
			screen.Add(0, 8, 15, "a checkmate, player b win");
		}
		else {
			screen.Add(0, 8, 15, "a check");
		}
	}
	else if (ischeck('b')) {
		if (ischeckmate('b')) {
			screen.Add(0, 8, 15, "b checkmate, player a win");
		}
		else {
			screen.Add(0, 8, 15, "b check");
		}
	}
	else {
		screen.Add(0, 8, 15, "move by arrowkey(enter : select, backspace : cancle)");
	}
}

int Arrowkey() {
	int key;
	while (1) {
		key = _getch();
		switch (key)
		{
		case 8:
			//backspace
			if (gamestate > 0) {
				gamestate--;
				return 1;
			}
			break;
		case 72:
			if (cr_y == 0)
				break;
			cr_y--;
			break;
		case 80:
			if (cr_y == 7)
				break;
			cr_y++;
			break;
		case 75:
			if (cr_x == 0)
				break;
			cr_x--;
			break;
		case 77:
			if (cr_x == 7)
				break;
			cr_x++;

			break;
		case 13:
			//enter
			return 1;
		default:
			key = 0;
			break;
		}
		if (key != 0) {
			printChessplate();
		}
	}
}
