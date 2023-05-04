#include <stdio.h>
#include<iostream>
#include<Windows.h>
#include<conio.h>
#include<iostream>
#include "Chess.h"
#include"Game.h"

using namespace std;
int main()
{
	//구조체에 비트맵 이미지 핸들 추가
	phone_a.name = 'P';
	phone_a.team = 'a';
	knight_a.name = 'N';
	knight_a.team = 'a';
	bishop_a.name = 'B';
	bishop_a.team = 'a';
	rook_a.name = 'R';
	rook_a.team = 'a';
	king_a.name = 'K';
	king_a.team = 'a';
	queen_a.name = 'Q';
	queen_a.team = 'a';

	phone_b.name = 'P';
	phone_b.team = 'b';
	knight_b.name = 'N';
	knight_b.team = 'b';
	bishop_b.name = 'B';
	bishop_b.team = 'b';
	rook_b.name = 'R';
	rook_b.team = 'b';
	king_b.name = 'K';
	king_b.team = 'b';
	queen_b.name = 'Q';
	queen_b.team = 'b';

	none.name = '*';
	none.team = 'n';

	resetxy();
	printChessplate(); //API : WM_PAINT에서 항상출력

	while(gameon){
		switch (gamestate)
		{
		case 0:
			Arrowkey();// API : 키입력시 상태따지기
			if (chessplate[cr_y][cr_x]->team == 'n')
				break;
			if (turn == 1 && chessplate[cr_y][cr_x]->team == 'b')
				break;
			if (turn == -1 && chessplate[cr_y][cr_x]->team == 'a')
				break;
			px = cr_x;
			py = cr_y;
			gamestate++;
			break;
		case 1:
			Arrowkey();
			if (gamestate == 1) {
				pmove_x = cr_x;
				pmove_y = cr_y;
				gamestate++;
			}
			else {
				px = -1;
				py = -1;
			}
			break;
		case 2:
			GameOn(px, py, pmove_x, pmove_y);
			resetxy();
			gamestate = 0;
			pmove_x = -1;
			pmove_y = -1;
			px = -1;
			py = -1;
			break;
		default:
			break;
		}	
		resetxy();
		printChessplate();
		Sleep(1000.0 / 60.0);
	}
	system("pause");
	return 0;
}
