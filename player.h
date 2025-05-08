#pragma once
#pragma once
#include"character.h"
#include"collision_manager.h"

class Player : public Character {

protected:
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

	bool can_change_direction = true;

	Timer seeThrough_invulnerable;

	Animation animation_seeThrough1;
	Animation animation_seeThrough2;

	bool is_seeThrough_visible = false;


protected:
	const float CD_ATTACK = 0.60f;
	const float SPEED_RUN = 300.0f;
	const float SPEED_JUMP = 780.0f;
	const float SPEED_JUMP_RUN = 350.f;
	const float SPEED_ATTACK_RUN = 0.0f;

public:

	Player();
	~Player();

	void on_input(const ExMessage& msg)override;
	void on_update(float delta)override;
	void on_render()override;

	void on_take_hit()override;
	void decrease_hp()override;
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
	bool can_changeDirection()const {
		return can_change_direction;
	}
	void set_changeDirection(bool flag) {
		can_change_direction = flag;
	}
	bool can_jump()const {
		return is_on_floor() && is_jump_key_down;
	}
	bool get_player_facing_left() {
		return is_facing_left;
	}
	int get_move_axis() const{
		if(!is_attacking)
		return is_right_key_down - is_left_key_down;
		else {
			return 0;
		}
	}
	void on_jump();
	void on_land();
	void on_attack1();
	void on_attack2();
	void on_attack3();

	void draw_hp();
};