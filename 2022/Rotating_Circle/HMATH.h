#ifndef __HMATH_H__
#define __HMATH_H__
#include<math.h>

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

struct matrix2x2 {
	float m[2][2] = { 0 };
};

struct HRectangle
{
	vector2d point[4];
	vector2d CMS; //center of mass
};

vector2d MATxVEC(matrix2x2 mat, vector2d vec) {
	vector2d newvec;

	newvec.x = (vec.x * mat.m[0][0]) + (vec.y * mat.m[1][0]);
	newvec.y = (vec.x * mat.m[0][1]) + (vec.y * mat.m[1][1]);

	return newvec;
}

#endif