#ifndef __SCREEN_H__
#define __SCREEN_H_
#include<stdio.h>
#include<Windows.h>

#define WIDTH 120
#define HEIGHT 30

class ScreenManager {
private:
	HANDLE hStdout, hNewScreenBuffer, hNewScreenBuffer2;
	SMALL_RECT srctWriteRect;
	CHAR_INFO chiBuffer[WIDTH * HEIGHT];
	COORD coordBufSize;
	COORD coordBufCoord;
	BOOL fSuccess;

	HANDLE curScreenBufferHandle;
public:
	ScreenManager() {
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

		SetConsoleTitle(L"Program");
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
	void Clear(int color) {
		//clear all
		for (int y = 0; y < HEIGHT; ++y)
		{
			for (int x = 0; x < WIDTH; ++x)
			{
				chiBuffer[x + WIDTH * y].Char.UnicodeChar = ' ';
				chiBuffer[x + WIDTH * y].Attributes = color; //126
			}
		}
	}
	void Clear(int y, int color) {
		//clear line
		for (int x = 0; x < WIDTH; ++x)
		{
			chiBuffer[x + WIDTH * y].Char.UnicodeChar = ' ';
			chiBuffer[x + WIDTH * y].Attributes = color; //126
		}
		
	}
	void Show() {
		static int i = 0; //decide what buffer would be use

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
		i++; //버퍼 바꾸기
	}

	void Add(int x, int y, int atb, char ch) {
		//화면밖으로 나가는거 예외처리
		if (x<0 || x>WIDTH - 1)
			return;
		if (y<0 || y>HEIGHT - 1)
			return;

		chiBuffer[y * WIDTH + x].Char.UnicodeChar = ch; //글자지정
		chiBuffer[y * WIDTH + x].Attributes = atb; // 색깔지정
	}
	void Add(int x, int y, int atb, const char* ch) {
		for (int j = 0;; j++) {
			if (ch[j] == '\0') {
				break;
			}
			else
			{
				Add(x+j, y, atb, ch[j]);

			}
		}
	}
	void Add(int x, int y, int atb, const char* ch[], int len) {
		for (int i = 0; i < len; i++) {
			Add(x, y + i, atb, ch[i]);
		}
	}
};

#endif // !__SCREEN_H__
