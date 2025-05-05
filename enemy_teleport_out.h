#pragma once
#include"enemy_attack_box.h"
#include"animation.h"


class EnemyTeleportOut {
public:
	EnemyTeleportOut(const vector2& position);
	~EnemyTeleportOut();

	bool check_valid()const {
		return is_valid;
	}

	void update(float delta);
	void draw();
private:
	Animation animation;
	bool is_valid = true;
	TeleportOutBox* attack_box = nullptr;

};