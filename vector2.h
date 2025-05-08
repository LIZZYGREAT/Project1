#pragma once

#include<iostream>
#include<cmath>

class vector2 {
public:
	float x = 0;
	float y = 0;

public:
	vector2() = default;
	~vector2() = default;

	vector2(float x, float y) :x(x), y(y) {}

	vector2 operator+(const vector2& vec)const {
		return vector2(x + vec.x, y + vec.y);
	}
	void operator+=(const vector2& vec) {
		x += vec.x;
		y += vec.y;
	}
	vector2 operator-(const vector2& vec)const {
		return vector2(x - vec.x, y - vec.y);
	}

	void operator-=(const vector2& vec) {
		x -= vec.x;
		y -= vec.y;
	}

	vector2 operator*(float val) const {
		return vector2(x * val, y * val);
	}

	float operator*(const  vector2& vec)const {
		return x * vec.x + y * vec.y;
	}

	void operator*=(float val) {
		x *= val;
		y *= val;
	}

	float length() const {
		return sqrt(x * x + y * y);
	}

	vector2 normalize() {
		float len = length();
		if (len == 0) {
			return vector2(0, 0);
		}
		else {
			return vector2(x / len, y / len);
		}
	}
};