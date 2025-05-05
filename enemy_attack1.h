#pragma once
#include"enemy_attack_box.h"
#include"animation.h"

class EnemyAttack1 {
public:
	EnemyAttack1();
	~EnemyAttack1();

	bool check_valid_stage1()const {
		return is_valid_stage1;
	}
	bool check_valid_stage2()const {
		return is_valid_stage2;
	}
	bool check_valid_stage3()const {
		return is_valid_stage3;
	}

	void update(float delta);
	void draw();

private:
	Animation animation;
	bool is_valid_stage1 = true;
	bool is_valid_stage2 = true;
	bool is_valid_stage3 = true;
	Attack1Box* attack_box = nullptr;
};








