#ifndef __RIGIDBODY_H__
#define __RIGIDBODY_H__
#include<math.h>
#include<iostream>
using namespace std;

//vector2d Ŭ���� �����
//4Ģ���� �����Ű��(������ �����ε�)

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
		//�������͸� ��ȯ�Ѵ�
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
	vector2d v; //�ӵ�
	vector2d F; //��¥��

	float w; //���ӵ�
	vector2d T; //��ũ

	float m; //����
	float reverseM; //���� ����

	vector2d gravity;

	//���ӵ��� ��/�������� ���ϸ� �ȴ�

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

//Rigidbody �����ϴ� Ŭ���� �����
//������ ��ü���� ������� �浹�˻� -> ��������

class RigidBody_Rectangle : public RigidBody {
private:
	HRectangle rec;
public:
	RigidBody_Rectangle(float m, vector2d v1, vector2d v2) :RigidBody(m), rec(v1, v2) {}
	void Move(float t) {
		//�ӵ��� �������� �ӵ�*t ��ŭ �̵�
		// ���ӵ��� �������� 1/2at^2��ŭ �̵�
		//�ﰢ�Լ� �̿��� ��ǥ���
		// �ӵ��� ���ӵ� ���ϱ�
		vector2d MoveVector(0, 0);

		F = F + gravity;

		MoveVector = v * t + (F * reverseM) * t * t * 0.5f;

		v = v + (F * reverseM) * t;

		//2x2 ��� �̿��ؼ� ȸ����Ű��

		rec.vertex[0] = rec.vertex[0] + MoveVector;
		rec.vertex[1] = rec.vertex[1] + MoveVector;

		F = F - gravity;
		//�浹���� �� ��������
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
		//�ӵ��� �������� �ӵ�*t ��ŭ �̵�
		// ���ӵ��� �������� 1/2at^2��ŭ �̵�
		//�ﰢ�Լ� �̿��� ��ǥ���
		// �ӵ��� ���ӵ� ���ϱ�
		vector2d MoveVector(0, 0);

		F = F + gravity;

		MoveVector = v * t + (F * reverseM) * t * t * 0.5f;

		v = v + (F * reverseM) * t;

		//2x2 ��� �̿��ؼ� ȸ����Ű��

		hci.center = hci.center + MoveVector;

		F = F - gravity;

		//�浹���� �� ��������
	}

	bool isCollidedWithRect(HRectangle rt) {
		HRectangle newRec(vector2d(rt.vertex[0].x - hci.r, rt.vertex[0].y - hci.r),vector2d(rt.vertex[1].x + hci.r, rt.vertex[1].y + hci.r));

		//�簢�� Ȯ���Ų �� ���� �߽� �ȿ� �ִ��� Ȯ��
		//�������� ���ȿ� �ִ��� ����Ȯ��

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
		//���� �߽� ���� ���� -> ������ ��������
		//�̸� �̿��ؼ� ���������ϱ�
	}

	void Render(HDC hdc) {
		//�ڱ��ڽ� ����ϱ�

		Ellipse(hdc, hci. center.x - hci.r, hci.center.y - hci.r,
			hci.center.x + hci.r, hci.center.y + hci.r);

		//�������̵� �ұ�?
	}

	void RenderV(HDC hdc) {
		//�ӵ� ���� ������ ����ϱ�
		MoveToEx(hdc, hci.center.x, hci.center.y, NULL);
		LineTo(hdc, hci.center.x + v.UnitVector().x * hci.r, hci.center.y + v.UnitVector().y * hci.r);
		//�������̵� �ұ�?
	}

	void Bounce(HRectangle rt) {
		HRectangle newRec(vector2d(rt.vertex[0].x - hci.r, rt.vertex[0].y - hci.r), vector2d(rt.vertex[1].x + hci.r, rt.vertex[1].y + hci.r));
		vector2d ErrorCorection;

		//vector2d* minDistanceWall;

		vector2d QuadrentD[4];

		int k=0;

		//�簢�� Ȯ���Ų �� ���� �߽� �ȿ� �ִ��� Ȯ��
		//�������� ���ȿ� �ִ��� ����Ȯ��
		QuadrentD[0].x = hci.center.x - newRec.vertex[0].x; //Left
		QuadrentD[0].y = 0.0f;
		QuadrentD[1].x = hci.center.x - newRec.vertex[1].x; //Right
		QuadrentD[1].y = 0.0f;
		QuadrentD[2].y = hci.center.y - newRec.vertex[1].y; //Down
		QuadrentD[2].x = 0.0f;
		QuadrentD[3].y = hci.center.y - newRec.vertex[0].y; //Top
		QuadrentD[3].x = 0.0f;

		//��ǥ �����ε�
		//���� ���� (0,0)���� �ΰ� �ۼ��ϱ�

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

		//���� ��ġ�� ������
		//hci.center = hci.center - ErrorCorection;
		hci.center = hci.center + (QuadrentD[k] * (-2.0f));

		//�� ��ŭ ƨ�ܳ���
		//**

	//�ӵ� ������ ���⸦ �̿��ؼ� �󸶳� ��� ���Դ��� Ȯ��
	//���� ƨ�ܳ���.
	}

	HCircle& GetShape() {
		return hci;
	}
};
#endif __RIGIDBODY_H__