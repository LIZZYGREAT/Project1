#pragma once
#include"attack_factory.h"

class Attack1Factory : public AttackFactory {
public:
	Attack1Factory() = default;
	~Attack1Factory() = default;

	Attack1Factory* create_attack() {
		Attack1Factory* attack = new Attack1Factory();
		return attack;
	}
};

class Attack2Factory : public AttackFactory {
public:
	Attack2Factory() = default;
	~Attack2Factory() = default;
	Attack2Factory* create_attack() {
		Attack2Factory* attack = new Attack2Factory();
		return attack;
	}
};

class Attack3Factory : public AttackFactory {
public:
	Attack3Factory() = default;
	~Attack3Factory() = default;
	Attack3Factory* create_attack() {
		Attack3Factory* attack = new Attack3Factory();
		return attack;
	}
};

class Attack4Factory : public AttackFactory {
public:
	Attack4Factory() = default;	
	~Attack4Factory() = default;
	Attack4Factory* create_attack() {
		Attack4Factory* attack = new Attack4Factory();
		return attack;
	}
};

class Attack5Factory : public AttackFactory {
public:
	Attack5Factory() = default;
	~Attack5Factory() = default;
	Attack5Factory* create_attack() {
		Attack5Factory* attack = new Attack5Factory();
		return attack;
	}
};