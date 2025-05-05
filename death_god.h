#pragma once

#include<vector>
#include"character.h"
#include"enemy_attack1.h"
#include"enemy_attack2.h"
#include"enemy_attack3.h"
#include"enemy_attack4.h"
#include"enemy_attack5.h"
#include"enemy_teleport_out.h"


class DeathGod :public Character {
public:
	DeathGod();
	~DeathGod();

	void on_update(float delta)override;
	void on_input(const ExMessage& msg)override {};
	void on_render()override;
	void render_hp_bar();
	void on_take_hit()override;



	void set_facing_left(bool flag) {
		is_facing_left = flag;
	}
	bool get_facing_left( )const {
		return is_facing_left;
	}

	
	void set_is_attack1(bool flag) {
		is_attack1 = flag;
	}
	bool get_is_attack1() {
		return is_attack1;
	}
	void set_is_attack1_anim_end(bool flag) {
		is_attack1_anim_end = flag;
	}
	bool get_is_attack1_anim_end() const {
		return is_attack1_anim_end;
	}
	void set_is_attack2(bool flag) {
		is_attack2 = flag;
	}
	bool get_is_attack2() const {
		return is_attack2;
	}
	void set_is_attack2_anim_end(bool flag) {
		is_attack2_anim_end = flag;
	}

	bool get_is_attack2_anim_end() const {
		return is_attack2_anim_end;
	}
	void set_is_attack3(bool flag) {
		is_attack3 = flag;
	}
	bool get_is_attack3() const{
		return is_attack3;
	}

	void set_is_attack3_anim_end(bool flag) {
		is_attack3_anim_end = flag;
	}
	bool get_is_attack3_anim_end() const {
		return is_attack3_anim_end;
	}
	void set_is_attack4(bool flag) {
		is_attack4 = flag;
	}
	bool get_is_attack4() const {
		return is_attack4;
	}

	void set_is_attack4_anim_end(bool flag) {
		is_attack4_anim_end = flag;
	}
	bool get_is_attack4_anim_end() const {
		return is_attack4_anim_end;
	}
	void set_is_attack5(bool flag) {
		is_attack5 = flag;
	}
	bool get_is_attack5() const {
		return is_attack5;
	}

	void set_is_attack5_anim_end(bool flag) {
		is_attack5_anim_end = flag;
	}
	bool get_is_attack5_anim_end() const {
		return is_attack5_anim_end;
	}
	void set_is_teleport_out(bool flag) {
		is_teleport_out = flag;
	}
	bool get_is_teleport_out() const {
		return is_teleport_out;
	}

	void set_is_teleport_in(bool flag) {
		is_teleport_in = flag;
	}
	bool get_is_teleport_in() const  {
		return is_teleport_in;
	}
	bool get_is_run() const{
		return is_run;
	}

	void on_attack1();
	void on_attack2();
	void on_attack3();
	void on_attack4();
	void on_attack5();
	void on_teleport_out();
	void on_teleport_in();
	void on_run();

private:
	Animation* image_attack1 = nullptr;
	Animation* image_attack2 = nullptr;
	Animation* image_attack3 = nullptr;
	Animation* image_attack4 = nullptr;
	Animation* image_attack5 = nullptr;

	Animation* image_teleport_out = nullptr;
	Animation* image_teleport_in = nullptr;

	Animation* image_dead = nullptr;
	Animation* image_idle = nullptr;
	Animation* image_move = nullptr;

	bool is_attack1 = false;
	bool is_attack2 = false;
	bool is_attack3 = false;
	bool is_attack4 = false;
	bool is_attack5 = false;

	bool is_attack1_anim_end = true;
	bool is_attack2_anim_end = true;
	bool is_attack3_anim_end = true;
	bool is_attack4_anim_end = true;
	bool is_attack5_anim_end = true;


	bool is_teleport_out = false;
	bool is_teleport_in = false;

	bool is_run = false;

	std::vector<EnemyAttack1*> enemy_attack1_list;
	std::vector<EnemyAttack2*> enemy_attack2_list;
	std::vector<EnemyAttack4*> enemy_attack4_list;
	std::vector<EnemyAttack5*> enemy_attack5_list;
	std::vector<EnemyTeleportOut*> enemy_teleport_out_list;

};