#include<windows.h>
#include"HMATH.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = TEXT("Rectangle Rotation");

HBITMAP hbit;

#define PI 3.14

class RotatingRect {
private:
	matrix2x2 Rmat;
	HRectangle hrect;
	float w;
public:
	RotatingRect(HRectangle _h, float _w) : hrect(_h), w(_w){}
	void Rotate(float t) {
		float seta = w * t;

		Rmat.m[0][0] = cos(PI * 2 - seta);
		Rmat.m[0][1] = sin(PI * 2 - seta);
		Rmat.m[1][0] = cos(PI / 2.0f - seta);
		Rmat.m[1][1] = sin(PI / 2.0f - seta);

		for (int i = 0; i < 4; i++) {
			hrect.point[i] = MATxVEC(Rmat, hrect.point[i]);
		}
	}
	void Render(HDC hdc) {
		MoveToEx(hdc, hrect.CMS.x + hrect.point[3].x, hrect.CMS.y + hrect.point[3].y, NULL);
		for (int i = 0; i < 4; i++) {
			LineTo(hdc, hrect.CMS.x + hrect.point[i].x, hrect.CMS.y + hrect.point[i].y);
		}
	}
	void Extension(float mlt) {
		for (int i = 0; i < 4; i++) {
			hrect.point[i] = hrect.point[i] * mlt;
		}
		w *= mlt;
	}
};

void DrawBitmap(HDC hdc, int x, int y, HBITMAP hBit) {
	HDC MemDC;
	HBITMAP OldBitmap;
	int bx, by;
	BITMAP bit;

	MemDC = CreateCompatibleDC(hdc);
	OldBitmap = (HBITMAP)SelectObject(MemDC, hBit);

	GetObject(hBit, sizeof(BITMAP), &bit);
	bx = bit.bmWidth;
	by = bit.bmHeight;

	BitBlt(hdc, x, y, bx, by, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, OldBitmap);
	DeleteDC(MemDC);

}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) {
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;

	RECT crt;
	HDC hMemDC;
	HBITMAP OldBit;

	static HRectangle hrect;

	static RotatingRect* r[100];
	static int arlen = 0;

	float seta = -1.0f;

	switch (iMessage) {
	case WM_CREATE:
		hWndMain = hWnd;
		/*
		hrect.point[0] = vector2d(-100, -100);
		hrect.point[1] = vector2d(-100, 100);
		hrect.point[2] = vector2d(100, 100);
		hrect.point[3] = vector2d(100, -100);
		hrect.CMS = vector2d(500, 300);

		r = new RotatingRect(hrect, seta);

		hrect.point[0] = vector2d(-50, -50);
		hrect.point[1] = vector2d(-50, 50);
		hrect.point[2] = vector2d(50, 50);
		hrect.point[3] = vector2d(50, -50);
		hrect.CMS = vector2d(800, 200);
		*/

		SetTimer(hWnd, 0, 50, NULL);
		InvalidateRect(hWnd, NULL, FALSE);
		return 0;
	case WM_LBUTTONDOWN:
		hrect.point[0] = vector2d(-50, -50);
		hrect.point[1] = vector2d(-50, 50);
		hrect.point[2] = vector2d(50, 50);
		hrect.point[3] = vector2d(50, -50);
		hrect.CMS = vector2d(LOWORD(lParam), HIWORD(lParam));
		r[arlen] = new RotatingRect(hrect, seta);
		arlen++;
		SetTimer(hWnd, 1, 50, NULL);
		return 0;
	case WM_LBUTTONUP:
		KillTimer(hWnd, 1);
		return 0;
	case WM_TIMER:
		switch(wParam) {
		case 0:
			for (int i = 0; i < arlen; i++) {
				r[i]->Rotate(0.05f);
			}
			break;
		case 1:
			if (arlen != 0) {
				r[arlen-1]->Extension(1.1f);
			}
			break;
		}
		
		InvalidateRect(hWnd, NULL, FALSE);
		return 0;
	case WM_PAINT:
		GetClientRect(hWnd, &crt); //윈도우 크기 가지고오기
		hdc = BeginPaint(hWnd, &ps); //DC생성

		if (hbit == NULL) {
			hbit = CreateCompatibleBitmap(hdc, crt.right, crt.bottom); //hdc에 호환된는 윈도우 크기의 비트맵 생성
		}
		hMemDC = CreateCompatibleDC(hdc); //hdc와 호환되는 dc 생성
		OldBit = (HBITMAP)SelectObject(hMemDC, hbit); //hbit을 hmemdc에 선택

		FillRect(hMemDC, &crt, GetSysColorBrush(COLOR_WINDOW));
		/*
		MoveToEx(hMemDC, hrect.CMS.x + hrect.point[3].x, hrect.CMS.y + hrect.point[3].y, NULL);
		for (int i = 0; i < 4; i++) {
			LineTo(hMemDC, hrect.CMS.x + hrect.point[i].x, hrect.CMS.y + hrect.point[i].y);
		}
		*/
		for (int i = 0; i < arlen; i++) {
			r[i]->Render(hMemDC);
		}
		DeleteDC(hMemDC);

		if (hbit) DrawBitmap(hdc, 0, 0, hbit); //hbit을 hdc를 이용해 출력

		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		for (int i = 0; i < arlen; i++) {
			delete r[i];
		}
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}