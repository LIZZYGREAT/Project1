#pragma once
#include"vector2.h"

class AttackFactory {
public:
	AttackFactory() = default;
	~AttackFactory() = default;

	AttackFactory* create_attack() {
		AttackFactory* attack = new AttackFactory();
		return attack;
	}
};