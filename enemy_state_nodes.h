#pragma once
#include"timer.h"
#include"state_node.h"

class EnemyAttack1State :public StateNode {
public:
	EnemyAttack1State();
	~EnemyAttack1State() = default;

	void on_enter()override;
	void on_update(float delta) override;
	void on_exit() override;
private:
	Timer timer;
	float attack_speed = 200;
	float sum_delta_time = 0;
};

class EnemyAttack2State :public StateNode {
public:
	EnemyAttack2State();
	~EnemyAttack2State() = default;

	void on_enter()override;
	void on_update(float delta) override;
	void on_exit() override;
private:
	Timer timer;
};

class EnemyAttack3State :public StateNode {
public:
	EnemyAttack3State();
	~EnemyAttack3State() = default;

	void on_enter()override;
	void on_update(float delta) override;
	void on_exit() override;
private:
	Timer timer1;
};

class EnemyAttack4State :public StateNode {
public:
	EnemyAttack4State();
	~EnemyAttack4State() = default;

	void on_enter()override;
	void on_update(float delta) override;
	void on_exit() override;

private:
	Timer timer1;
};

class EnemyDeadState :public StateNode {
public:
	EnemyDeadState();
	~EnemyDeadState() = default;

	void on_enter()override;
	void on_update(float delta) override;

private:
	Timer timer;
};

class EnemyFallState :public StateNode {
public:
	EnemyFallState() = default;
	~EnemyFallState() = default;

	void on_enter()override;
	void on_update(float delta) override;
};

class EnemyIdleState :public StateNode {
public:
	EnemyIdleState();
	~EnemyIdleState() = default;

	void on_enter()override;
	void on_update(float delta) override;
private:
	Timer timer;
};

class EnemyJumpState :public StateNode {
public:
	EnemyJumpState() = default;
	~EnemyJumpState() = default;

	void on_enter()override;
	void on_update(float delta) override;
};

class EnemyRunState :public StateNode {
public:
	EnemyRunState() = default;
	~EnemyRunState() = default;

	void on_enter()override;
	void on_update(float delta) override;
	void on_exit() override;
private:
	const float min_dis = 350.0f;
	float speed_run = 450.0f;
};

class EnemyTakeHitState : public StateNode {  //¿‡À∆”⁄roll
public:
	EnemyTakeHitState();
	~EnemyTakeHitState() = default;

	void on_enter()override;
	void on_update(float delta) override;
	void on_exit()override;
private:
	Timer timer;
};