#pragma once
#include"character.h"

class Player :public Character {
private:
	Timer timer_attack1_cd;
	bool is_attack1 = false;
	bool is_attack1_cd_cmp = true;

	Timer timer_attack2_cd;
	bool is_attack2 = false;
	bool is_attack2_cd_cmp = true;

	Timer timer_attack3_cd;
	bool is_attack3 = false;
	bool is_attack3_cd_cmp = true;

	bool is_attacking = false;


	bool is_left_key_down = false;
	bool is_right_key_down = false;
	bool is_jump_key_down = false;

	bool is_attack1_key_down = false;
	bool is_attack2_key_down = false;
	bool is_attack3_key_down = false;


	bool is_jump_vfx_visible = false;
	Animation animation_jump_vfx;

	bool is_land_vfx_visible = false;
	Animation animation_land_vfx;

	bool is_taking_hit = false;

private:
	const float CD_ATTACK = 0.60f;
	const float SPEED_RUN = 300.0f;
	const float SPEED_JUMP = 780.0f;
	const float SPEED_ROLL = 800.0f;


public:
	Player();
	~Player();

	void on_input(const ExMessage& msg)override;
	void on_update(float delta)override;
	void on_render()override;

	void on_take_hit()override;
	void set_attacking(bool flag) {
		is_attacking = flag;
	}
	void set_attack1(bool flag) {
		is_attack1 = flag;
	}
	void set_attack2(bool flag) {
		is_attack2 = flag;
	}
	void set_attack3(bool flag) {
		is_attack3 = flag;
	}
	bool get_attacking()const {
		return  is_attacking;
	}
	void set_taking_hit(bool flag) {
		is_taking_hit = flag;
	}
	bool get_is_taking_hit() const {
		return is_taking_hit;
	}
	bool can_attack1() const {  //注意实时更新is_attacking
		return !is_attacking && is_attack1_cd_cmp && is_attack1_key_down;
	}

	bool can_attack2() const {
		return !is_attacking && is_attack2_cd_cmp && is_attack2_key_down;
	}

	bool can_attack3() const {
		return !is_attacking && is_attack3_cd_cmp && is_attack3_key_down;
	}

	bool can_jump()const {
		return is_on_floor() && is_jump_key_down;
	}

	int get_move_axis()const {
		return is_right_key_down - is_left_key_down;
	}
	void on_jump();
	void on_land();
	void on_attack1();
	void on_attack2();
	void on_attack3();

	void update_hit_box_position();
};