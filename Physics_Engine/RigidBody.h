#ifndef __RIGIDBODY_H__
#define __RIGIDBODY_H__
#include<math.h>
#include<iostream>
using namespace std;

//vector2d 클래스 만들기
//4칙연산 적용시키기(연산자 오버로딩)

struct matrix2x2 {
	float m[2][2] = { 0 };
};


class vector2d {
public:
	float x, y;
public:
	vector2d() :x(0.0f), y(0.0f) {}
	vector2d(float _x, float _y) :x(_x), y(_y) {}

	float Size() {
		float size = sqrt((x * x) + (y * y));
		return size;
	}

	vector2d UnitVector() {
		//단위벡터를 반환한다
		return vector2d(x / Size(), y / Size());
	}

	vector2d operator+(const vector2d& a) {
		vector2d vec(x + a.x, y + a.y);
		return vec;
	}
	vector2d operator-(const vector2d& a) {
		vector2d vec(x - a.x, y - a.y);
		return vec;
	}
	vector2d operator*(const float& a) {
		vector2d vec(x * a, y * a);
		return vec;
	}
};

class HRectangle
{
public:
	vector2d vertex[2];
public:
	HRectangle(vector2d v1, vector2d v2) {
		vertex[0].x = v1.x; vertex[0].y = v1.y;
		vertex[1].x = v2.x; vertex[1].y = v2.y;
	}
	void Inform() {
		cout << "(" << vertex[0].x << "," << vertex[0].y << ")";
		cout << "(" << vertex[1].x << "," << vertex[1].y << ")";
		cout << endl;
	}
};

class HCircle {
public:
	vector2d center;
	float r;
public:
	HCircle(vector2d v1, float _r) :center(v1), r(_r) {}
	void Inform() {
		cout << "(" << center.x << "," << center.y << ")" << endl;
	}
};

class RigidBody
{
protected:
	vector2d v; //속도
	vector2d F; //알짜힘

	float w; //각속도
	vector2d T; //토크

	float m; //질량
	float reverseM; //질량 역수

	vector2d gravity;

	//가속도는 힘/질량으로 구하면 된다

public:
	RigidBody(float m) : m(m), reverseM(1.0f / m) {
		v.x = 0.0f; v.y = 0.0f;
		F.x = 0.0f; F.y = 0.0f;

		w = 0.0f;
		T.x = 0.0f; T.y = 0.0f;

		gravity.x = 0.0f; gravity.y = 0.0f;
	}
	void AddForce(vector2d f) {
		F = F + f;
	}
	void AddImPulse(vector2d Imp) {
		v.x += Imp.x * reverseM;
		v.y += Imp.y * reverseM;
	}
	void ResetF() {
		F.x = 0.0f; F.y = 0.0f;
	}
	void ResetV() {
		v.x = 0.0f; v.y = 0.0f;
	}
	void GravityOn(bool TF) {
		if (TF)
			gravity.y = m * (900.0f);
		else
			gravity.y = 0.0f;
	}
	virtual void Move(float t) {}
	~RigidBody() {}
};

//Rigidbody 관리하는 클래스 만들기
//움직인 물체들을 대상으로 충돌검사 -> 오차수정

class RigidBody_Rectangle : public RigidBody {
private:
	HRectangle rec;
public:
	RigidBody_Rectangle(float m, vector2d v1, vector2d v2) :RigidBody(m), rec(v1, v2) {}
	void Move(float t) {
		//속도의 방향으로 속도*t 만큼 이동
		// 가속도의 방향으로 1/2at^2만큼 이동
		//삼각함수 이용해 좌표계산
		// 속도에 가속도 더하기
		vector2d MoveVector(0, 0);

		F = F + gravity;

		MoveVector = v * t + (F * reverseM) * t * t * 0.5f;

		v = v + (F * reverseM) * t;

		//2x2 행렬 이용해서 회전시키기

		rec.vertex[0] = rec.vertex[0] + MoveVector;
		rec.vertex[1] = rec.vertex[1] + MoveVector;

		F = F - gravity;
		//충돌판정 후 오차수정
	}

	bool isCollidedWithRect(RigidBody_Rectangle rt) {

	}

	void Render(HDC hdc) {
		Rectangle(hdc, rec.vertex[0].x, rec.vertex[0].y, rec.vertex[1].x, rec.vertex[1].y);
	}

	HRectangle& GetShape() {
		return rec;
	}
};

class RigidBody_Circle :public RigidBody {
private:
	HCircle hci;
public:
	RigidBody_Circle(float m, vector2d v1, float r1) :RigidBody(m), hci(v1, r1) {}
	void Move(float t) {
		//속도의 방향으로 속도*t 만큼 이동
		// 가속도의 방향으로 1/2at^2만큼 이동
		//삼각함수 이용해 좌표계산
		// 속도에 가속도 더하기
		vector2d MoveVector(0, 0);

		F = F + gravity;

		MoveVector = v * t + (F * reverseM) * t * t * 0.5f;

		v = v + (F * reverseM) * t;

		//2x2 행렬 이용해서 회전시키기

		hci.center = hci.center + MoveVector;

		F = F - gravity;

		//충돌판정 후 오차수정
	}

	bool isCollidedWithRect(HRectangle rt) {
		HRectangle newRec(vector2d(rt.vertex[0].x - hci.r, rt.vertex[0].y - hci.r),vector2d(rt.vertex[1].x + hci.r, rt.vertex[1].y + hci.r));

		//사각형 확장시킨 후 원의 중심 안에 있는지 확인
		//꼭짓점이 원안에 있는지 따로확인

		if (newRec.vertex[0].x < hci.center.x && hci.center.x < newRec.vertex[1].x &&
			newRec.vertex[0].y < hci.center.y && hci.center.y < newRec.vertex[1].y) {
			return true;
		}

		vector2d distance;

		for (int i = 0; i < 2; i++) {
			distance = hci.center - rt.vertex[i];
			if (distance.Size() < hci.r) {
				return true;
			}
		}

		return false;
	}

	void BouncefromCircle(HCircle ci) {
		//원의 중심 연결 벡터 -> 접점의 수직벡터
		//이를 이용해서 오차수정하기
	}

	void Render(HDC hdc) {
		//자기자신 출려하기

		Ellipse(hdc, hci. center.x - hci.r, hci.center.y - hci.r,
			hci.center.x + hci.r, hci.center.y + hci.r);

		//오버라이딩 할까?
	}

	void RenderV(HDC hdc) {
		//속도 벡터 선으로 출력하기
		MoveToEx(hdc, hci.center.x, hci.center.y, NULL);
		LineTo(hdc, hci.center.x + v.UnitVector().x * hci.r, hci.center.y + v.UnitVector().y * hci.r);
		//오버라이딩 할까?
	}

	void Bounce(HRectangle rt) {
		HRectangle newRec(vector2d(rt.vertex[0].x - hci.r, rt.vertex[0].y - hci.r), vector2d(rt.vertex[1].x + hci.r, rt.vertex[1].y + hci.r));
		vector2d ErrorCorection;

		//vector2d* minDistanceWall;

		vector2d QuadrentD[4];

		int k=0;

		//사각형 확장시킨 후 원의 중심 안에 있는지 확인
		//꼭짓점이 원안에 있는지 따로확인
		QuadrentD[0].x = hci.center.x - newRec.vertex[0].x; //Left
		QuadrentD[0].y = 0.0f;
		QuadrentD[1].x = hci.center.x - newRec.vertex[1].x; //Right
		QuadrentD[1].y = 0.0f;
		QuadrentD[2].y = hci.center.y - newRec.vertex[1].y; //Down
		QuadrentD[2].x = 0.0f;
		QuadrentD[3].y = hci.center.y - newRec.vertex[0].y; //Top
		QuadrentD[3].x = 0.0f;

		//좌표 문제인듯
		//왼쪽 위를 (0,0)으로 두고 작성하기

		for (int i = 0; i < 4; i++) {
			if (QuadrentD[i].Size() < QuadrentD[k].Size()) {
				k = i;
			}
		}

		if (k == 0 || k == 1) {
			v.x = v.x * (-0.5f);
		}
		else {
			v.y = v.y * (-0.5f);
		}

		//원래 위치로 돌리고
		//hci.center = hci.center - ErrorCorection;
		hci.center = hci.center + (QuadrentD[k] * (-2.0f));

		//들어간 만큼 튕겨내기
		//**

	//속도 벡터의 기울기를 이용해서 얼마나 깊게 들어왔는지 확인
	//이후 튕겨낸다.
	}

	HCircle& GetShape() {
		return hci;
	}
};
#endif __RIGIDBODY_H__