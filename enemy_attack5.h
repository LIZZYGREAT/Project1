#pragma once
#include"enemy_attack_box.h"
#include"animation.h"


class EnemyAttack5 {
public:
	EnemyAttack5(const vector2& positon,bool is_facing_left);
	~EnemyAttack5();

	bool check_valid()const {
		return is_valid;
	}
	void update(float delta,bool is_facing_left);
	void draw();

private:
	Animation animation;
	bool is_valid = true;
	Attack5Box* attack_box = nullptr;

};