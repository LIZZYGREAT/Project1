#pragma once
#include"attack_factory.h"
#include"collision_box.h"
#include"vector2.h"


class Attack1Box :public CollisionBox {
public:
	Attack1Box();
	~Attack1Box() = default;

	void init_position(const vector2& position) {
		r_center = position;
	}
	void update_r(float delta);
	void update_r_center(const vector2& enemy_center);
	float get_r()const {
		return r;
	}
	const vector2& get_r_center() const{
		return r_center;
	}
private:
	Shape shape;
	float  r;
	vector2 r_center;
private:
	const float R_SPEED_stage1 = 200.0f;
	const float R_SPEED_stage2 = 600.0f;
};


class Attack2Box :public CollisionBox {
public:
	Attack2Box() ;
	~Attack2Box() = default;

	void init_position(const vector2& position) {
		r_center = position;
	}
	void update_r_center(const vector2& player_center);
	void update_r(float delta);
	float get_r()const {
		return r;
	}
	const vector2 get_r_center()const {
		return r_center;
	}
private:
	Shape shape;
	float r;
	vector2 r_center;
private:
	const float R_SPEED = 200.0f;
};

/*class Attack3 :public CollisionBox {     //实际上攻击3并非一个攻击箱，而是特效
public:
	Attack3() = default;
	~Attack3() = default;

	void update_position(const vector2& enemy_position);
	void update_size(float delta);
private:
	Timer timer;
	Shape shape;
};*/

class Attack4Box :public CollisionBox {
public:
	Attack4Box();
	~Attack4Box() = default;

	void init_position(const vector2& position, bool is_facing_left);
	void update_position(bool is_facing_left,float delta);
	void update_size(float delta);
private:
	Shape shape;
private:
	const float SIZE_SPEED = 417.0f;
	const float SPREAD_SPEED = 200.0f;

};

class Attack5Box :public CollisionBox {
public:
	Attack5Box();
	~Attack5Box() = default;

	void init_position(const vector2& enemy_position, bool is_facing_left);
	void update_position(bool is_facing_left,float delta);
	void update_size(float delta);
private:
	Shape shape;
private:
	const float SIZE_SPEED = 417.0f;
	const float SPREAD_SPEED = 200.0f;
};

class TeleportOutBox :public CollisionBox {
public:
	TeleportOutBox();
	~TeleportOutBox() = default;

	void init_position(const vector2& position) {
		r_center = position;
	}
	void update_r_center(const vector2& position);
	void update_r(float delta);
private:
	Shape shape;
	float r;
	vector2 r_center;
private:
	const float R_SPEED = 650.0f;
};