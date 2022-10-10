#include "SnakeGame.h"
using std::thread;

int main() {
	FILE* fp = NULL;

	int mode = 0;
	srand(time(NULL)); //난수 시드 생성
	makeBuffer();

	thread t1(getDir);

	//메뉴창

	loadScore(fp);


	while (isGameOn) {
		clearScreen(BASIC_COLOR);
		
		mode = Menu();
		if (mode == 0) {
			Game(); //인게임
			saveScore(fp);
			isGameOn = doAgain();
		}
		else if (mode == 1) {
			highScore();
		}
		else if (mode == 2) {
			developinform();
		}

	}
	t1.join();
	return 0;
}

void makeBuffer() {
	//콘솔창 생성
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	coordBufSize.X = WIDTH;
	coordBufSize.Y = HEIGHT;

	coordBufCoord.X = 0;
	coordBufCoord.Y = 0;

	srctWriteRect.Left = srctWriteRect.Top = 0;
	srctWriteRect.Right = WIDTH - 1;
	srctWriteRect.Bottom = HEIGHT - 1;

	hNewScreenBuffer = CreateConsoleScreenBuffer(
		GENERIC_WRITE,
		0,
		NULL,                    // default security attributes 
		CONSOLE_TEXTMODE_BUFFER, // must be TEXTMODE 
		NULL);                   // reserved; must be NULL 
	hNewScreenBuffer2 = CreateConsoleScreenBuffer(
		GENERIC_WRITE,
		0,
		NULL,                    // default security attributes 
		CONSOLE_TEXTMODE_BUFFER, // must be TEXTMODE 
		NULL);                   // reserved; must be NULL 

	SetConsoleTitle(L"Snake Game");
	SetConsoleScreenBufferSize(hNewScreenBuffer, coordBufSize);
	SetConsoleWindowInfo(hNewScreenBuffer, TRUE, &srctWriteRect);
	SetConsoleScreenBufferSize(hNewScreenBuffer2, coordBufSize);
	SetConsoleWindowInfo(hNewScreenBuffer2, TRUE, &srctWriteRect);

	if (hNewScreenBuffer2 == INVALID_HANDLE_VALUE ||
		hNewScreenBuffer == INVALID_HANDLE_VALUE)
	{
		printf("CreateConsoleScreenBuffer failed - (%d)\n", GetLastError());
		return;
	}
}

void prCharLine(const char* ch, int atb, int x, int y) {
	for (int j = 0;; j++) {
		if (ch[j] == '\0') {
			break;
		}
		else
		{
			chiBuffer[y * WIDTH + x + j].Char.UnicodeChar = ch[j]; //글자지정
			chiBuffer[y * WIDTH + x + j].Attributes = atb; // 색깔지정

		}
	}
}

void prCharArr(const char* ch[], int len, int atb, int x, int y) {
	for (int i = 0; i < len; i++) {
		prCharLine(ch[i], atb, x, y + i);
	}
}

void clearScreen(int color) {
	for (int y = 0; y < HEIGHT; ++y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			chiBuffer[x + WIDTH * y].Char.UnicodeChar = ' ';
			chiBuffer[x + WIDTH * y].Attributes = color; //126
		}
	}
}

void printScreen() {
	static int i = 0; //무슨 버퍼 사용할지 구분

	if (i % 2 == 0) {
		curScreenBufferHandle = hNewScreenBuffer;
	}
	else {
		curScreenBufferHandle = hNewScreenBuffer2;
	}

	fSuccess = WriteConsoleOutput(
		curScreenBufferHandle, // screen buffer to write to 
		chiBuffer,        // buffer to copy from 
		coordBufSize,     // col-row size of chiBuffer 
		coordBufCoord,    // top left src cell in chiBuffer 
		&srctWriteRect);

	if (!SetConsoleActiveScreenBuffer(curScreenBufferHandle)) {
		printf("SetConsoleActiveScreenBuffer failed - (%d)\n", GetLastError());
		return;
	}

	Sleep(1 / 60.0f * 1000.0f); //잠깐 멈춤
	i++; //버퍼 바꾸기
}

int Menu() {
	dir = 0;
	//& 문자는 출력 안하게 지정해놓음(깜빡임 방지)
	const char* menu[10] = { "**********************************",
							"*           Snake Game           *",
							"*   move: arrow, select: space   *",
							"*                                *",
							"*             START              *",
							"*                                *",
							"*           HIGH_SCORE           *",
							"*                                *",
							"*      DEVELOPER INFORMATION     *",
							"**********************************" };
	const int lx = WIDTH / 2 - 17;
	const int ly = HEIGHT / 2 - 5;
	static int mode = 0;

	while (1) {
		if (dir == SPACE) {
			break;
		}
		if (dir == UP && mode != 0) {
			mode--;
			dir = 0;
		}
		else if (dir == DOWN && mode != 2) {
			mode++;
			dir = 0;
		}

		prCharArr(menu, 10, BASIC_COLOR, lx, ly);

		switch (mode)
		{
		case 0:
			prCharLine("START", SELECT_COLOR, lx + 14, ly + 4);
			break;
		case 1:
			prCharLine("HIGH SCORE", SELECT_COLOR, lx + 12, ly + 6);
			break;
		case 2:
			prCharLine("DEVELOPER INFORMATION", SELECT_COLOR, lx + 7, ly + 8);
			break;
		default:
			break;
		}

		printScreen();
	}
	return mode;
}

void Game() {
	char gamestage[STAGE_SIZE][STAGE_SIZE];
	Snake player;
	Food* foods[STAGE_SIZE * STAGE_SIZE + 1] = { NULL };
	int fnum = 0;
	player.head.x = STAGE_SIZE / 2;
	player.head.y = STAGE_SIZE / 2;
	player.tail = player.head;
	player.dir = LEFT;
	dir = player.dir;
	bool isDead = false;

	clock_t last_movetime; //마지막으로 움직인 시간
	double speed = 0.1f; //이동속도

	clock_t last_gentime; //마지막으로 생성한 시간
	double gentime = 1.5f; //생성속도

	//Game start
	addBody(&player);
	resetStage(gamestage, &player, foods, &fnum);
	printStage(gamestage);

	countDown();

	last_movetime = clock();
	last_gentime = clock();
	while (!isDead)
	{
		//일정 시간마다 dir 확인
		//그 방향으로 이동(죽음판정, 먹이판정)
		//dir 0으로 초기화
		//esc누르면 일시정지 가능하게
		//점수계산
		if ((float)(clock() - last_movetime) / CLOCKS_PER_SEC > speed) {
			isDead = !checkFront(gamestage, &player, foods, &fnum);
			moveSnake(&player);
			last_movetime = clock();
		}
		if ((clock() - last_gentime) / CLOCKS_PER_SEC > gentime) {
			foods[fnum] = makeFood(gamestage);
			fnum++;
			foods[fnum] = NULL;

			last_gentime = clock();
		}

		resetStage(gamestage, &player, foods, &fnum);
		printStage(gamestage);
	}
	deleteAllfood(foods, fnum);
	return;
}

void highScore() {
	const char* menu[6] = { "**********************************",
							"*                                *",
							"*  Your high score:              *",
							"*  (press esc to get out)        *",
							"*                                *",
							"**********************************" };
	const int lx = WIDTH / 2 - 17;
	const int ly = HEIGHT / 2 - 5;
	clearScreen(BASIC_COLOR);

	prCharArr(menu, 6, BASIC_COLOR, lx, ly);
	int a, b, c, d;

	a = highscore / 1000;
	b = (highscore % 1000) / 100;
	c = (highscore % 100) / 10;
	d = (highscore % 10);
	chiBuffer[lx + (ly + 2) * WIDTH + 20].Char.UnicodeChar = a + '0';
	chiBuffer[lx + (ly + 2) * WIDTH + 21].Char.UnicodeChar = b + '0';
	chiBuffer[lx + (ly + 2) * WIDTH + 22].Char.UnicodeChar = c + '0';
	chiBuffer[lx + (ly + 2) * WIDTH + 23].Char.UnicodeChar = d + '0';
	printScreen();

	while (1) {
		if (dir == ESC) {
			dir = 0;
			break;
		}
		Sleep(1 / 60.0f * 1000.0f);
	}

	return;
}

void developinform() {
	const char* menu[9] = { "**********************************",
							"*                                *",
							"*    name: HYD                   *",
							"*                                *",
							"*    blog.naver.com/ydhan1024    *",
							"*                                *",
							"*    (press esc to get out)      *",
							"*                                *",
							"**********************************" };

	const int lx = WIDTH / 2 - 17;
	const int ly = HEIGHT / 2 - 5;
	clearScreen(BASIC_COLOR);

	prCharArr(menu, 9, BASIC_COLOR, lx, ly);

	printScreen();

	while (1) {
		if (dir == ESC) {
			dir = 0;
			break;
		}
		Sleep(1 / 60.0f * 1000.0f);
	}

	return;
}

bool doAgain() {
	dir = 0;
	bool YN = true;
	const char* ment1 = "Back to Menu?";
	const char* ment2 = "Yes";
	const char* ment3 = "No";
	while (1) {

		if (dir == SPACE) {
			break;
		}

		if (dir != 0) {
			if ((dir == RIGHT && YN == true) || (dir == LEFT && YN == false)) {
				YN = !YN;
				dir = 0;
			}
		}

		prCharLine(ment1, BASIC_COLOR, WIDTH / 2, HEIGHT / 2);
		if (YN == true) {
			prCharLine(ment2, SELECT_COLOR, WIDTH / 2, HEIGHT / 2 + 1);
			prCharLine(ment3, BASIC_COLOR, WIDTH / 2 + 7, HEIGHT / 2 + 1);
		}
		else if (YN == false) {
			prCharLine(ment2, BASIC_COLOR, WIDTH / 2, HEIGHT / 2 + 1);
			prCharLine(ment3, SELECT_COLOR, WIDTH / 2 + 7, HEIGHT / 2 + 1);
		}

		printScreen();
	}

	return YN;
}

void resetStage(char(*stage)[STAGE_SIZE], Snake* sk, Food* f[], int* fnum) {
	//전체배열 공백으로 초기화
	int i;
	for (i = 0; i < STAGE_SIZE; i++) {
		for (int j = 0; j < STAGE_SIZE; j++) {
			stage[i][j] = ' ';
		}
	}

	for (i = 0; i < STAGE_SIZE; i++) {
		stage[0][i] = WALL_TEX;
		stage[i][0] = WALL_TEX;
		stage[STAGE_SIZE - 1][i] = WALL_TEX;
		stage[i][STAGE_SIZE - 1] = WALL_TEX;
	}

	//몸통위치에 O 문자
	for (i = 0; i < sk->bodylen; i++) {
		stage[sk->body[i].x][sk->body[i].y] = BODY_TEX;
	}
	//머리 위치에 @ 문자
	stage[sk->head.x][sk->head.y] = HEAD_TEX;
	//먹이에 $문자
	for (i = 0; i < *fnum; i++) {
		stage[f[i]->pos.x][f[i]->pos.y] = FOOD_TEX;
	}

	return;
}

void printStage(char(*stage)[STAGE_SIZE]) {


	//화면 초기화

	clearScreen(BASIC_COLOR);

	//게임스테이지 출력
	for (int i = 0; i < STAGE_SIZE; i++) {
		for (int j = 0; j < STAGE_SIZE; j++) {
			chiBuffer[j * 2 + i * WIDTH].Char.UnicodeChar = stage[j][i];
			chiBuffer[j * 2 + i * WIDTH + 1].Char.UnicodeChar = ' ';
		}
	}

	//현재점수 출력

	int scorex = WIDTH / 2, scorey = STAGE_SIZE / 2;
	int a, b, c, d;

	prCharLine("Score : ", BASIC_COLOR, scorex - 8, scorey);

	a = score / 1000;
	b = (score % 1000) / 100;
	c = (score % 100) / 10;
	d = (score % 10);
	chiBuffer[scorex + scorey * WIDTH].Char.UnicodeChar = a + '0';
	chiBuffer[scorex + scorey * WIDTH + 1].Char.UnicodeChar = b + '0';
	chiBuffer[scorex + scorey * WIDTH + 2].Char.UnicodeChar = c + '0';
	chiBuffer[scorex + scorey * WIDTH + 3].Char.UnicodeChar = d + '0';

	//최고 점수 출력

	prCharLine("HighScore : ", BASIC_COLOR, scorex - 12, scorey + 2);

	a = highscore / 1000;
	b = (highscore % 1000) / 100;
	c = (highscore % 100) / 10;
	d = (highscore % 10);
	chiBuffer[scorex + (scorey + 2) * WIDTH].Char.UnicodeChar = a + '0';
	chiBuffer[scorex + (scorey + 2) * WIDTH + 1].Char.UnicodeChar = b + '0';
	chiBuffer[scorex + (scorey + 2) * WIDTH + 2].Char.UnicodeChar = c + '0';
	chiBuffer[scorex + (scorey + 2) * WIDTH + 3].Char.UnicodeChar = d + '0';

	//적용
	printScreen();
	return;
}

void countDown() {
	prCharLine("1", BASIC_COLOR, WIDTH / 2, HEIGHT / 2);
	printScreen();
	Sleep(1000);
	prCharLine("2", BASIC_COLOR, WIDTH / 2, HEIGHT / 2);
	printScreen();
	Sleep(1000);
	prCharLine("3", BASIC_COLOR, WIDTH / 2, HEIGHT / 2);
	printScreen();
	Sleep(1000);
	prCharLine("GO!", BASIC_COLOR, WIDTH / 2, HEIGHT / 2);
	printScreen();
	Sleep(1000);
	prCharLine("   ", BASIC_COLOR, WIDTH / 2, HEIGHT / 2);
	printScreen();
}

void addBody(Snake* sk) {
	//꼬리위치에 몸통 생성
	sk->body[sk->bodylen] = sk->tail;
	//몸길이+1
	sk->bodylen++;
	return;
}

int checkFront(char(*stage)[STAGE_SIZE], Snake* sk, Food* f[], int* fnum) {
	int _dir = dir;
	Position next = sk->head;

	if ((_dir + sk->dir) != (UP + DOWN) && (_dir == RIGHT || _dir == LEFT || _dir == UP || _dir == DOWN)) {
		sk->dir = _dir;
	}

	switch (sk->dir)
	{
	case UP:
		next.y = sk->head.y - 1; //배열이 0부터 위쪽이라 -1
		break;
	case DOWN:
		next.y = sk->head.y + 1;
		break;
	case RIGHT:
		next.x = sk->head.x + 1;
		break;
	case LEFT:
		next.x = sk->head.x - 1;
		break;
	default:
		break;
	}
	//죽음 0 이동가능 1
	if (stage[next.x][next.y] != ' ') {
		if (stage[next.x][next.y] == WALL_TEX) {
			return 0;
		}
		else if (stage[next.x][next.y] == FOOD_TEX) {
			for (int i = 0; i < *fnum; i++) {
				if (f[i]->pos.x == next.x && f[i]->pos.y == next.y) {
					eatFood(f, i, fnum);
					addBody(sk);
					return 1;
				}
			}
		}
		else if (stage[next.x][next.y] == BODY_TEX) {
			//목은 확인할 필요 없음
			for (int i = 1; i < sk->bodylen; i++) {
				if (sk->body[i].x == next.x && sk->body[i].y == next.y) {
					return 0;
				}
			}
		}
	}
	return 1;
}

void moveSnake(Snake* sk) {
	//자기 몸통쪽으로는 못가게 예외처리 필요(dir 입력받을때 서로 상반되면 값 받지 않기)
	for (int i = sk->bodylen - 1; i > 0; i--) {
		//연결된 앞부분 몸통 좌표로 이동
		sk->body[i] = sk->body[i - 1];
	}
	if (sk->bodylen != 0) {
		//몸통 길이가 0이 아니라면 머리 위치로 body[0] 이동
		sk->body[0] = sk->head;
		sk->tail = sk->body[sk->bodylen - 1];
	}
	//이동방향에 따라 머리 이동
	switch (sk->dir)
	{
	case UP:
		sk->head.y--; //배열이 0부터 위쪽이라 -1
		break;
	case DOWN:
		sk->head.y++;
		break;
	case RIGHT:
		sk->head.x++;
		break;
	case LEFT:
		sk->head.x--;
		break;
	default:
		break;
	}

	return;
}

void getDir() {
	while (isGameOn) {
		//원래 이동방향과 같거나 상반되면 방향 바꾸지 않음
		dir = _getch();

		Sleep(1 / 60.0f);
	}
}

Food* makeFood(char(*stage)[STAGE_SIZE]) {
	//static int fnum = 0; //먹이 갯수
	//Food* food = (Food*)malloc(sizeof(Food)); //힙 공간 할당
	Food* food = new Food;

	while (1) {
		food->pos.x = rand() % (STAGE_SIZE - 2) + 1;
		food->pos.y = rand() % (STAGE_SIZE - 2) + 1;
		if (stage[food->pos.x][food->pos.y] == ' ') {
			break;
		}
	}
	return food;
}

void eatFood(Food* f[], int id, int* fnum) {
	delete f[id];
	for (int i = id; i < *fnum; i++) {
		f[i] = f[i + 1];
	}
	*fnum -= 1;
	score += 5;
}

void deleteAllfood(Food* f[], int fnum) {
	for (int i = 0; i < fnum; i++) {
		delete f[i];
	}
}

void loadScore(FILE* fp) {
	fopen_s(&fp, "highscore.bin", "rb");
	if (fp != NULL) {
		fscanf_s(fp, "%d", &highscore);
		fclose(fp);
	}
}

void saveScore(FILE* fp) {
	if (score > highscore) {
		highscore = score;
		fopen_s(&fp, "highscore.bin", "wb");
		if (fp != NULL) {
			fprintf_s(fp, "%d", score);
			fclose(fp);
		}
	}
	score = 0;
}