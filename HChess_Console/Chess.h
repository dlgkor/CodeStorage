#pragma once
bool move(int x, int y, int move_x, int move_y);
void moveaction(int x, int y, int move_x, int move_y);

void scolor(unsigned short text = 15, unsigned short back = 0) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), text | (back << 4));
}
struct Chess
{
	char name;
	char team;
	int x, y;
};


Chess none;
Chess phone_a;
Chess knight_a;
Chess bishop_a;
Chess rook_a;
Chess king_a;
Chess queen_a;

Chess phone_b;
Chess knight_b;
Chess bishop_b;
Chess rook_b;
Chess king_b;
Chess queen_b;

Chess* chessplate[8][8] = { {&rook_a,&knight_a, &bishop_a,&king_a,&queen_a,&bishop_a,&knight_a,&rook_a},
	{&phone_a,&phone_a,&phone_a,&phone_a,&phone_a,&phone_a,&phone_a,&phone_a },
	{&none,&none,&none,&none,&none,&none,&none,&none},
	{&none,&none,&none,&none,&none,&none,&none,&none},
	{&none,&none,&none,&none,&none,&none,&none,&none},
	{&none,&none,&none,&none,&none,&none,&none,&none},
	{&phone_b,&phone_b,&phone_b,&phone_b,&phone_b,&phone_b,&phone_b,&phone_b},
	{&rook_b,&knight_b, &bishop_b,&king_b,&queen_b,&bishop_b,&knight_b,&rook_b} };


//Chessplate ch;
int errorcode = -1;

void resetxy() {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			chessplate[i][j]->x = j;
			chessplate[i][j]->y = i;
		}
	}
	return;
}

bool phonecanmove(int x, int y, int move_x, int move_y, char teem) {
	if (chessplate[move_y][move_x] != &none)
		return  false;
	if (x == move_x) {
		if (teem == 'a') {
			if (y + 1 == move_y) {
				return true;
			}
			else if (y + 2 == move_y && y == 1) {
				return true;
			}
		}
		else if (teem == 'b') {
			if (y - 1 == move_y) {
				return true;
			}
			else if (y - 2 == move_y && y == 6) {
				return true;
			}
		}
	}

	return false;
}

bool phonecaneat(int x, int y, int move_x, int move_y, char teem) {
	if (chessplate[move_y][move_x]->team == 'n')
		return false;

	if (teem == 'a') {
		if (move_x - x == 1 && move_y - y == 1) {
			return true;
		}
		else if (move_x - x == -1 && move_y - y == 1) {
			return true;
		}
	}
	else if (teem == 'b') {
		if (move_x - x == -1 && move_y - y == -1) {
			return true;
		}
		else if (move_x - x == 1 && move_y - y == -1) {
			return true;
		}

	}

	return false;
}

bool knightcanmove(int x, int y, int move_x, int move_y) {
	if (abs(move_x - x) == 1 && abs(move_y - y) == 2) {
		return true;
	}
	else if (abs(move_x - x) == 2 && abs(move_y - y) == 1) {
		return true;
	}
	return false;
}

bool bishopcanmove(int x, int y, int move_x, int move_y) {
	if (abs(move_x - x) == abs(move_y - y)) {
		if ((move_x - x) * (move_y - y) > 0) {
			if (y < move_y) {
				for (int i = 1; i < move_y - y; i++) {
					if (chessplate[y+i][x+i]->team != 'n')
						return false;
				}
			}
			else if (y > move_y) {
				for (int i = 1; i < y-move_y; i++) {
					if (chessplate[y-i][x-i]->team != 'n')
						return false;
				}
			}
		}
		if ((move_x - x) * (move_y - y) < 0) {
			if (y < move_y) {
				for (int i = 1; i < move_y-y; i++) {
					if (chessplate[y+i][x-i]->team != 'n')
						return false;
				}
			}
			else if (x < move_x) {
				for (int i = 1; i < move_x-x; i++) {
					if (chessplate[y-i][x+i]->team != 'n')
						return false;
				}
			}
		}
		return true;
	}
	return false;
}

bool rookcanmove(int x, int y, int move_x, int move_y) {
	if (move_x == x) {
		if (y < move_y) {
			for (int i = 1; i < move_y - y; i++) {
				if (chessplate[y+i][x]->team != 'n')
					return false;
			}
		}
		else if (y > move_y) {
			for (int i = 1; i < y-move_y; i++) {
				if (chessplate[y-i][x]->team != 'n')
					return false;
			}
		}
		return true;
	}
	else if (move_y == y) {
		if (x < move_x) {
			for (int i = 1; i < move_x-x; i++) {
				if (chessplate[y][x+i]->team != 'n')
					return false;
			}
		}
		else if (x > move_x) {
			for (int i = 1; i < x-move_x; i++) {
				if (chessplate[y][x-i]->team != 'n')
					return false;
			}
		}
		return true;
	}
	return false;
}

bool queencanmove(int x, int y, int move_x, int move_y) {
	if (bishopcanmove(x, y, move_x, move_y) || rookcanmove(x, y, move_x, move_y))
		return true;
	return false;
}

bool ischeck(char team) {
	if (team == 'a') {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (move(i, j, king_a.x, king_a.y))
					return true;
			}
		}
	}
	if (team == 'b') {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (move(i, j, king_b.x, king_b.y))
					return true;
			}
		}
	}
	return false;
}

bool cankingrun(int c, int d, char team) {
	int kx=0, ky=0;
	if (c == 0 && d == 0) {
		return false;
	}

	if (team == 'a') {
		kx = king_a.x;
		ky = king_a.y;
	}
	else if (team == 'b') {
		kx = king_b.x;
		ky = king_b.y;
	}

	if (move(kx, ky, kx + c, ky + d)) {
		return true;
	}

	return false;
}

bool ischeckmate(char team) {
	Chess *stack = NULL;
	int kx = 0, ky = 0;

	int a=0;
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if (cankingrun(i, j, team)) {
				//errorcode = 1;
				return false;
			}
		}
	}
	resetxy();
	if (team == 'a') {
		kx = king_a.x;
		ky = king_a.y;
	}
	else if (team == 'b') {
		kx = king_b.x;
		ky = king_b.y;
	}
	else {
		printf("error\n");
		return false;
	}

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (move(i, j, kx, ky)) {
				stack = chessplate[j][i];
				stack->x = i;
				stack->y = j;
				a++;
				if (a >= 2) {
					//errorcode = 2;
					return true;
				}

			}
		}
	}

	if (stack != NULL) {
		//printf("%d %d\n", stack->x, stack->y);
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (move(i, j, stack->x, stack->y)) {
					//errorcode = 3;
					return false;
				}
			}
		}
		
		
		if (stack->name == 'R' || stack->name == 'Q') {
			if (kx == stack->x) {
				if (stack->y < ky) {
					for (int k = 1; k < ky - stack->y; k++) {
						for (int i = 0; i < 8; i++) {
							for (int j = 0; j < 8; j++) {
								if (stack->team != chessplate[j][i]->team && chessplate[j][i]->name != 'K') {
									if (move(i, j, stack->x, stack->y + k)) {
										//errorcode = 4;
										return false;
									}
								}
							}
						}
					}
				}
				else if (stack->y > ky) {
					for (int k = 1; k < stack->y - ky; k++) {
						for (int i = 0; i < 8; i++) {
							for (int j = 0; j < 8; j++) {
								if (stack->team != chessplate[j][i]->team && chessplate[j][i]->name != 'K') {
									if (move(i, j, stack->x, stack->y - k)) {
										//errorcode = 5;
										return false;
									}
								}
							}
						}
					}
				}
			}
			else if (ky == stack->y) {
				if (stack->x < kx) {
					for (int k = 1; k < kx-stack->x; k++) {
						for (int i = 0; i < 8; i++) {
							for (int j = 0; j < 8; j++) {
								if (stack->team != chessplate[j][i]->team && chessplate[j][i]->name != 'K') {
									if (move(i, j, stack->x + k, stack->y)) {
										//errorcode = 6;
										return false;
									}
								}
							}
						}
					}
				}
				else if (stack->x > kx) {
					for (int k = 1; k < stack->x - kx; k++) {
						for (int i = 0; i < 8; i++) {
							for (int j = 0; j < 8; j++) {
								if (stack->team != chessplate[j][i]->team && chessplate[j][i]-> name != 'K') {
									if (move(i, j, stack->x - k, stack->y)) {
										//errorcode = 7;
										return false;
									}
								}
							}
						}
					}
				}
			}
		}
		if (stack->name == 'B' || stack->name == 'Q') {
			if (abs(kx - stack->x) == abs(ky - stack->y)) {
				if ((kx - stack->x) * (ky - stack->y) > 0) {
					if (stack->y < ky) {
						for (int k = 1; k < ky - stack->y; k++) {
							for (int i = 0; i < 8; i++) {
								for (int j = 0; j < 8; j++) {
									if (stack->team != chessplate[j][i]->team && chessplate[j][i]->name != 'K') {
										if (move(i, j, stack->x + k, stack->y + k)) {
											//errorcode = 8;
											return false;
										}
									}
								}
							}
						}
					}
					else if (stack->y > ky) {
						for (int k = 1; k < stack->y - ky; k++) {
							for (int i = 0; i < 8; i++) {
								for (int j = 0; j < 8; j++) {
									if (stack->team != chessplate[j][i]->team && chessplate[j][i]->name != 'K') {
										if (move(i, j, stack->x - k, stack->y - k)) {
											//errorcode = 9;
											return false;
										}
									}
								}
							}
						}
					}
				}
				if ((kx - stack->x) * (ky - stack->y) < 0) {
					if (stack->y < ky) {
						for (int k = 1; k < ky - stack->y; k++) {
							for (int i = 0; i < 8; i++) {
								for (int j = 0; j < 8; j++) {
									if (stack->team != chessplate[j][i]->team && chessplate[j][i]->name != 'K') {
										if (move(i, j, stack->x - k, stack->y + k)) {
											//errorcode = 10;
											return false;
										}
									}
								}
							}
						}
					}
					else if (stack->x < kx) {
						for (int k = 1; k < kx-stack->x; k++) {
							for (int i = 0; i < 8; i++) {
								for (int j = 0; j < 8; j++) {
									if (stack->team != chessplate[j][i]->team && chessplate[j][i]->name != 'K') {
										if (move(i, j, stack->x + k, stack->y - k)) {
											//errorcode = 11;
											return false;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		
	}
	//errorcode = 12;
	return true;
}

bool kingcanmove(int x, int y, int move_x, int move_y) {
	Chess* save;
	bool k = false;

	if (abs(move_x - x) <= 1) {
		if (abs(move_y - y) <= 1) {
			save = chessplate[move_y][move_x];
			moveaction(x, y, move_x, move_y);

			if (!ischeck(chessplate[move_y][move_x]->team))
				k = true;

			chessplate[y][x] = chessplate[move_y][move_x];
			chessplate[move_y][move_x] = save;

			resetxy();
		}
	}

	return k;
}

void moveaction(int x, int y, int move_x, int move_y) {
	//printf("%d %d %d %d", x, y, move_x, move_y);
	chessplate[move_y][move_x] = chessplate[y][x];
	chessplate[y][x] = &none;
	resetxy();
	if (chessplate[move_y][move_x]->name == 'P') {
		if (move_y == 7) {
			//change team a phone
			//screen.Add(0, 8, 15, "change phone to (N,B,R,Q): ");
			//screen.Show();
			while (chessplate[move_y][move_x]->name == 'P') {
				char change = _getch();

				if (change == 'N') {
					chessplate[move_y][move_x] = &knight_a;
				}
				else if (change == 'B') {
					chessplate[move_y][move_x] = &bishop_a;
				}
				else if (change == 'R') {
					chessplate[move_y][move_x] = &rook_a;
				}
				else if (change == 'Q') {
					chessplate[move_y][move_x] = &queen_a;
				}
			}
		}
		else if (move_y == 0) {
			//change team b phone
			//screen.Add(0, 8, 15, "change phone to (N,B,R,Q): ");
			//screen.Show();
			while (chessplate[move_y][move_x]->name == 'P') {
				char change = _getch();

				if (change == 'N') {
					chessplate[move_y][move_x] = &knight_b;
				}
				else if (change == 'B') {
					chessplate[move_y][move_x] = &bishop_b;
				}
				else if (change == 'R') {
					chessplate[move_y][move_x] = &rook_b;
				}
				else if (change == 'Q') {
					chessplate[move_y][move_x] = &queen_b;
				}
			}
		}
	}
	return;
}

bool move(int x, int y, int move_x, int move_y)
{
	if (x == move_x && y == move_y)
		return false;

	if((0>x || x>7) || (0 > y || y > 7) || (0 > move_x || move_x > 7) || (0 > move_y || move_y > 7))
		return false;

	if (chessplate[y][x]->team == 'n')
		return false;

	if (chessplate[y][x]->team == chessplate[move_y][move_x]->team)
		return false;

	if (chessplate[y][x]->name == 'P') {
		if (phonecanmove(x, y, move_x, move_y, chessplate[y][x]->team)) {
			return true;
			
		}
		else if (phonecaneat(x,y,move_x,move_y, chessplate[y][x]->team)){
			return true;
		}
	}
	if (chessplate[y][x]->name == 'N') {
		if (knightcanmove(x, y, move_x, move_y)) {
			return true;
		}
	}
	if (chessplate[y][x]->name == 'B') {
		if (bishopcanmove(x, y, move_x, move_y)) {
			return true;
		}
	}
	if (chessplate[y][x]->name == 'R') {
		if (rookcanmove(x, y, move_x, move_y)) {
			return true;
		}		
	}
	if (chessplate[y][x]->name == 'Q') {
		if (queencanmove(x, y, move_x, move_y))
			return true;
	}
	if (chessplate[y][x]->name == 'K') {
		if (kingcanmove(x, y, move_x, move_y)) {
			return true;
		}
	}
	return false;
}