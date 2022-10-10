#include<iostream>
#include"ScreenManager.h"

using namespace std;

ScreenManager screen;

class CT {
private:
	float transparency[3][4];
	float Xray[19];
public:
	void getRay() {
		int i = 0;
		cout << "위쪽 3개 입력" << endl;
		for (;i < 3; i++) {
			cin >> Xray[i];
		}
		cout << "옆쪽 4개 입력" << endl;
		for (; i < 7; i++) {
			cin >> Xray[i];
		}
		cout << "오른쪽 위 대각선 6개 입력(위에서부터)" << endl;
		for (; i < 13; i++) {
			cin >> Xray[i];
		}
		cout << "오른쪽 아래 대각선 6개 입력(아래서부터)" << endl;
		for (; i < 19; i++) {
			cin >> Xray[i];
		}
	}
	void getCT() {
		transparency[0][0] = Xray[7];
		transparency[2][3] = Xray[12];

		transparency[0][3] = Xray[13];
		transparency[2][0] = Xray[18];

		transparency[1][0] = Xray[3] - transparency[0][0] - transparency[2][0];
		transparency[1][3] = Xray[6] - transparency[0][3] - transparency[2][3];

		transparency[0][2] = Xray[14] - transparency[1][3];
		transparency[2][1] = Xray[17] - transparency[1][0];

		transparency[2][2] = Xray[11] - transparency[1][3];
		transparency[0][1] = Xray[8] - transparency[1][0];

		transparency[1][1] = Xray[4] - transparency[0][1] - transparency[2][1];
		transparency[1][2] = Xray[5] - transparency[0][2] - transparency[2][2];
	}
	void printCT() {
		screen.Clear(0);
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 3; j++) {
				screen.Add(j, i, GetCTColor(transparency[j][i]), "/");
			}
		}
		screen.Show();
	}
	int GetCTColor(float transp) {
		if (0 <= transp && transp < 3) {
			return 15;
		}
		if (3 <= transp && transp < 6) {
			return 128;
		}
		if (6 <= transp && transp < 10) {
			return 112;
		}
		else {
			return 112;
		}
	}
};

int main() {
	CT newCT;
	newCT.getRay();
	newCT.getCT();
	newCT.printCT();

	while (1)
	{

	}
}