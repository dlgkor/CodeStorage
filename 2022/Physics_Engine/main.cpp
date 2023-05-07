//마우스 힘계산
#include<windows.h>
#include<string>
#include"CalculateMouse.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = TEXT("Class");

HBITMAP hbit;

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

	HBRUSH hbrush, oldbrush;

	//초기화 작업은 WM_CREATE에서!
	//값을 보존해야 하면 static 붙이자
	static MouseInform MIF;

	static vector2d tmp(0, 0);
	vector2d saveCpos(0, 0);

	static SpecialCircle CIRCLE(1.0f, vector2d(400, 400), 25.0f);

	static RigidBody_Rectangle HRectList[6] = { {1.0f, vector2d(50, 500), vector2d(950, 550)},
									{1.0f,vector2d(1, 1), vector2d(50, 550)},
									{1.0f,vector2d(950, 1), vector2d(1000, 550)},
									{1.0f,vector2d(50, 1), vector2d(950, 50)},
									{1.0f,vector2d(150, 130), vector2d(350, 200)},
									{1.0f,vector2d(450, 230), vector2d(650, 300)}, };
	static int HRectListSize = 6;


	float FPS = 1000.0f / 60.0f;

	switch (iMessage) {
	case WM_CREATE:
		hWndMain = hWnd;
		MIF.SetMass(1.0f);
		CIRCLE.GetRigid().GravityOn(true);
		SetTimer(hWnd, 1, FPS, NULL);
		InvalidateRect(hWnd, NULL, FALSE);
		return 0;
	case WM_LBUTTONDOWN:
		MIF.SetIsClick(true);
		tmp.x = LOWORD(lParam);
		tmp.y = HIWORD(lParam);
		MIF.SetCurrentPoint(tmp);
		MIF.SetCurrentTime(clock());
		if (CIRCLE.CheckColide(tmp)) {
			CIRCLE.ishold = true;
			CIRCLE.GetRigid().ResetV();
			CIRCLE.GetRigid().GravityOn(false);
			CIRCLE.relative_distance = CIRCLE.GetRigid().GetShape().center - tmp;
		}
		return 0;
	case WM_MOUSEMOVE:
		if (MIF.GetIsClick()) {
			tmp.x = LOWORD(lParam);
			tmp.y = HIWORD(lParam);
			if ((clock() - MIF.GetCurTime()) / CLOCKS_PER_SEC > MIF.GetDeltaTime()) {
				MIF.SetCurrentPoint(tmp);
				MIF.SetCurrentTime(clock());
			}
			if (CIRCLE.ishold) {
				CIRCLE.GetRigid().ResetV();
				saveCpos = CIRCLE.GetRigid().GetShape().center;
				CIRCLE.MoveCenterTo(CIRCLE.relative_distance + tmp);
				for (int i = 0; i < HRectListSize; i++) {
					if (CIRCLE.GetRigid().isCollidedWithRect(HRectList[i].GetShape())) {
						CIRCLE.MoveCenterTo(saveCpos);
						break;
					}
				}
			}
		}
		return 0;
	case WM_LBUTTONUP:
		MIF.SetIsClick(false);
		tmp.x = LOWORD(lParam);
		tmp.y = HIWORD(lParam);
		//요거 정수로 표현됨
		MIF.SetCurrentPoint(tmp);
		MIF.SetCurrentTime(clock());
		if (CIRCLE.ishold) {
			CIRCLE.GetRigid().AddImPulse(MIF.SuddenForce() * 5.0f);
			CIRCLE.ishold = false;
			CIRCLE.GetRigid().GravityOn(true);

			//SetTimer(hWnd, 2, 50, NULL);
			// 
			//--------------------------------------------
			//std::string pfj = std::to_string(CIRCLE.GetForce());
			//MessageBoxA(NULL, pfj.c_str(), NULL, MB_OK);
			//--------------------------------------------			
		}
		return 0;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			CIRCLE.GetRigid().Move(FPS * 0.001f);
			//벽과 충돌처리
			for (int i = 0; i < HRectListSize; i++) {
				if (CIRCLE.GetRigid().isCollidedWithRect(HRectList[i].GetShape())) {
					CIRCLE.GetRigid().Bounce(HRectList[i].GetShape());
				}
			}
			InvalidateRect(hWnd, NULL, FALSE);
			break;
			/*
		case 2:
			CIRCLE.GetRigid().ResetF();
			KillTimer(hWnd, 2);
			break;
			*/
		default:
			break;
		}
		return 0;
	case WM_PAINT:
		GetClientRect(hWnd, &crt); //윈도우 크기 가지고오기
		hdc = BeginPaint(hWnd, &ps); //DC생성

		//SetMapMode(hdc, MM_LOMETRIC);

		if (hbit == NULL) {
			hbit = CreateCompatibleBitmap(hdc, crt.right, crt.bottom); //hdc에 호환된는 윈도우 크기의 비트맵 생성
		}
		hMemDC = CreateCompatibleDC(hdc); //hdc와 호환되는 dc 생성
		OldBit = (HBITMAP)SelectObject(hMemDC, hbit); //hbit을 hmemdc에 선택

		FillRect(hMemDC, &crt, GetSysColorBrush(COLOR_WINDOW));

		CIRCLE.GetRigid().Render(hMemDC);
		CIRCLE.GetRigid().RenderV(hMemDC);

		for (int i = 0; i < HRectListSize; i++) {
			HRectList[i].Render(hMemDC);
		}


		DeleteDC(hMemDC);

		if (hbit) DrawBitmap(hdc, 0, 0, hbit); //hbit을 hdc를 이용해 출력

		EndPaint(hWnd, &ps);

		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
