#pragma once
#include"enemy_attack_box.h"
#include"animation.h"


class EnemyAttack4 {
public:

	EnemyAttack4(const vector2& position,bool is_facing_left);
	~EnemyAttack4();

	bool check_valid()const {
		return is_valid;
	}
	void update(float delta,bool is_facing_left);
	void draw();
private:
	Animation animation;
	bool is_valid = true;
	Attack4Box* attack_box = nullptr;
};