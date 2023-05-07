#include<stdio.h>
#include<Windows.h>
#include<string>
#include<math.h>
#include<time.h>
#include<conio.h>
#include"Screen.h"
using namespace std;

enum { UP = 72, DOWN = 80, RIGHT = 77, LEFT = 75, ESC = 27, SPACE = 32, ENTER = 13 }; //����Ű �ƽ�Ű�ڵ� ��

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

	float PlayerX = 4.0f, PlayerY = 5.0f; //ī�޶� ������ġ
	float PlayerFOV = 3.141592f / 1.5f; //�þ߰�
	float PlayerStartAngle = 0.0f; //�����ִ¹���
	float PlayerHeight = 3.0f; //ī�޶� ����
	float FOVHeight;//�þ� ���� ����

	const float WallHeight = 7.0f; //���� ��������
	float RelativeWallHeight; //���� ȭ����� ����
	float WallStart; //�� �ٴ� y��ǥ

	float Raycast; //����
	float rayX, rayY; //���ص� ���� ����
	int rayPos; //ȭ����� ��������
	float CurrentAngle; //���� ���� ����

	Point LastDetected; //���������� ������ ��ü�� ��ǥ
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
			CurrentAngle += PlayerFOV / 120.0f; //120�� ������ ���캻��
			for (;;) {
				Raycast += 0.01f; //�������� õõ�� ����
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