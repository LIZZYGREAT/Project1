#pragma once

#include"enemy_attack_box.h"
#include"animation.h"

class EnemyAttack2 {
public:
	EnemyAttack2();
	~EnemyAttack2();

	bool check_valid_stage1()const {
		return is_valid_stage1;
	}
	bool check_valid_stage2()const {
		return is_valid_stage2;
	}
	void update(float delta);
	void draw();
private:
	Animation animation;
	bool is_valid_stage1 = true;
	bool is_valid_stage2 = true;
	Attack2Box* attack_box = nullptr;
};