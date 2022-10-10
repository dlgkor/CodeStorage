#ifndef __CAL_MOUSE__
#define __CAL_MOUSE__
#include<ctime>
#include<math.h>
#include"RigidBody.h"

class SpecialCircle {
private:
	RigidBody_Circle rig;
public:
	vector2d relative_distance;
	bool ishold;
public:
	SpecialCircle(float m, vector2d v1, float r):rig(m,v1,r),ishold(false){}
	bool CheckColide(vector2d p) {
		vector2d dst;

		dst = rig.GetShape().center - p;

		if (dst.Size() < rig.GetShape().r) {
			return true;
		}

		return false;
	}
	void MoveCenterTo(vector2d p) {
		rig.GetShape().center = p;
	}
	RigidBody_Circle& GetRigid() {
		return rig;
	}
};

class MouseInform {
private:
	vector2d Last; //마지막 측정 좌표
	vector2d Current; //현재 측정좌표
	clock_t LastCheckTime; //마지막 측정시간
	clock_t CurrentTime; //현재시간
	double mass; //마우스 질량
	bool IsClick;
	double deltaTime;
	void SetLastTime(clock_t t) {
		LastCheckTime = t;
	}
public:
	MouseInform(){
		Last.x = 0;
		Last.y = 0;
		Current.x = 0;
		Current.y = 0;

		LastCheckTime = clock();
		CurrentTime = clock();
		mass = 1.0f;
		IsClick = false;

		deltaTime = 0.02f;
	}
	void SetCurrentPoint(vector2d p) {
		Last = Current;
		Current = p;
	}
	void SetCurrentTime(clock_t t) {
		SetLastTime(CurrentTime);
		CurrentTime = t;
	}
	void SetMass(double m) {
		mass = m;
	}
	void SetIsClick(bool c) {
		IsClick = c;
	}
	bool GetIsClick() {
		return IsClick;
	}

	clock_t GetCurTime() {
		return CurrentTime;
	}

	vector2d SuddenForce() {
		vector2d newForce;

		newForce = Current - Last;
		
		return newForce;
	}
	double GetDeltaTime() {
		return deltaTime;
	}

	vector2d GetMouseMoveVector() {
		return Current - Last;
	}

	~MouseInform(){}

};
#endif
