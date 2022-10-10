#include<stdio.h>
#include<Windows.h>
#include<string>
#include<math.h>
#include<time.h>
#include<conio.h>
#include"Screen.h"
using namespace std;

enum { UP = 72, DOWN = 80, RIGHT = 77, LEFT = 75, ESC = 27, SPACE = 32, ENTER = 13 }; //방향키 아스키코드 값

struct Point
{
	int x, y;
};

wstring map;

int main() {
	map += L"####################";
	map += L"#                  #";
	map += L"#                  #";
	map += L"#           #      #";
	map += L"#                  #";
	map += L"#       #          #";
	map += L"#                  #";
	map += L"#                  #";
	map += L"#                  #";
	map += L"#                  #";
	map += L"####################";

	ScreenManager SCR;
	const int color = 11;
	clock_t LastScreenPrint = clock();
	const float FPS = 0.01f;

	clock_t LastMoveTime = clock();
	const float speed = 0.01f;

	float PlayerX = 4.0f, PlayerY = 5.0f; //카메라 현재위치
	float PlayerFOV = 3.141592f / 1.5f; //시야각
	float PlayerStartAngle = 0.0f; //보고있는방향
	float PlayerHeight = 3.0f; //카메라 높이
	float FOVHeight;//시야 끝의 높이

	const float WallHeight = 7.0f; //벽의 실제높이
	float RelativeWallHeight; //벽의 화면상의 높이
	float WallStart; //벽 바닥 y좌표

	float Raycast; //광선
	float rayX, rayY; //분해된 광선 벡터
	int rayPos; //화면상의 광선벡터
	float CurrentAngle; //현재 광선 각도

	Point LastDetected; //마지막으로 감지한 물체의 좌표
	LastDetected.x = -1;
	LastDetected.y = -1;

	int getkey;

	while (1) {
		if (_kbhit()) {
			getkey = _getch();
			if (getkey == 224) {
				getkey = _getch();
				if ((float)(clock() - LastMoveTime) / CLOCKS_PER_SEC > speed) {
					switch (getkey)
					{
					case UP:
						PlayerY -= 0.5f;
						break;
					case DOWN:
						PlayerY += 0.5f;
						break;
					case LEFT:
						PlayerX += 0.5f;
						break;
					case RIGHT:
						PlayerX -= 0.5f;
						break;
					default:
						break;
					}
				}
			}
		}

		SCR.Clear(color);
		CurrentAngle = PlayerStartAngle;
		for (int i = 0; i < 120; i++) {
			Raycast = 0.0f; 
			CurrentAngle += PlayerFOV / 120.0f; //120번 나눠서 살펴본다
			for (;;) {
				Raycast += 0.01f; //광선길이 천천히 증가
				rayX = PlayerX + (Raycast * cos(CurrentAngle));
				rayY = PlayerY - (Raycast * sin(CurrentAngle));
				rayPos = floor(rayX) + (floor(rayY) * 20);
				if (map[rayPos] == '#')
					break;
			}

			float distance = Raycast;

			FOVHeight = tan(PlayerFOV / 2.0f) * distance * 2.0f;

			RelativeWallHeight = (WallHeight / FOVHeight) * HEIGHT;
			WallStart = (((FOVHeight / 2.0f) - (float)(WallHeight - PlayerHeight)) / FOVHeight) * HEIGHT;

			

			short nshade;

			if (distance < 3.0f) nshade = 0x2588;
			else if (distance < 5.0f) nshade = 0x2593;
			else if (distance < 10.0f) nshade = 0x2592;
			else if (distance < 20.0f) nshade = 0x2591;
			else nshade = ' ';
			int j=0;

			if (LastDetected.x != floor(rayX) || LastDetected.y != floor(rayY)) {
				LastDetected.x = floor(rayX);
				LastDetected.y = floor(rayY);
				
				j = floor(WallStart) + floor(RelativeWallHeight);
			}
			else {
				for (j = floor(WallStart); j < floor(WallStart) + floor(RelativeWallHeight); j++) {
					SCR.Add(i, j, color, nshade);
				}
			}

			for (; j < HEIGHT; j++) {
				SCR.Add(i, j, color, "*");
			}
		}
		
		if ((float)(clock() - LastScreenPrint) / CLOCKS_PER_SEC > FPS) {
			SCR.Show();
			LastScreenPrint = clock();
		}
	}
	return 0;
}