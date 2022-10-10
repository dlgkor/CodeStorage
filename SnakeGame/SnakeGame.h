#define STAGE_SIZE 20
#define WIDTH 120
#define HEIGHT 30
#define WALL_TEX '*'
#define FOOD_TEX '$'
#define HEAD_TEX '@'
#define BODY_TEX 'O'
#define BASIC_COLOR 15
#define SELECT_COLOR 23

#include<stdio.h>
#include<Windows.h>
#include<time.h>
#include<conio.h>
#include<thread>

enum { UP = 72, DOWN = 80, RIGHT = 77, LEFT = 75, ESC = 27, SPACE = 32, ENTER = 13 }; //방향키 아스키코드 값

HANDLE hStdout, hNewScreenBuffer, hNewScreenBuffer2;
SMALL_RECT srctWriteRect;
CHAR_INFO chiBuffer[WIDTH * HEIGHT];
COORD coordBufSize;
COORD coordBufCoord;
BOOL fSuccess;

HANDLE curScreenBufferHandle;

struct Position
{
	int x, y;
};

struct Snake
{
	Position head;
	Position body[STAGE_SIZE*STAGE_SIZE];
	Position tail;
	int bodylen = 0;
	int dir;
};

struct Food
{
	Position pos;
};

int Menu(); //메뉴
void Game(); //게임(점수반환)
void highScore();
void developinform();

bool doAgain();

void resetStage(char(*stage)[STAGE_SIZE], Snake* sk, Food* f[], int* fnum); //배열안에 뱀과 먹이 표시
void printStage(char(*stage)[STAGE_SIZE]); //게임화면출력
void countDown();

void getDir();
void moveSnake(Snake* sk); // 앞에있는 위치로 옮김
Food* makeFood(char(*stage)[STAGE_SIZE]); // 먹이 랜덤생성
void eatFood(Food* f[], int id, int* fnum);
void deleteAllfood(Food* f[], int fnum);
void addBody(Snake* sk); //먹이 먹었을때 꼬리 위치에 몸체+1
int checkFront(char(*stage)[STAGE_SIZE], Snake* sk, Food* f[], int* fnum); //움직이기 전 앞에확인

void loadScore(FILE* fp); //점수 불러옴
void saveScore(FILE* fp); //점수 저장

void makeBuffer(); // 버퍼를 만듬
void prCharLine(const char* ch, int atb, int x, int y); //줄 단위로 출력
void prCharArr(const char* ch[], int len, int atb, int x, int y); //배열 출력
void clearScreen(int color);
void printScreen();

bool isGameOn = true;
int dir = LEFT; //방향키 입력상황
int highscore = 0;
int score = 0; //점수