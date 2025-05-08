#pragma once
#include"timer.h"
#include"state_node.h"

class PlayerAttack1State :public StateNode {
public:
	PlayerAttack1State();
	~PlayerAttack1State() = default;

	void on_enter()override;
	void on_update(float delta) override;
	void on_exit() override;
private:
	Timer timer;
	float run_speed = 300;
	float sum_delta_time = 0;
	float t = 0;
};

class PlayerAttack2State :public StateNode {
public:
	PlayerAttack2State();
	~PlayerAttack2State() = default;

	void on_enter()override;
	void on_update(float delta) override;
	void on_exit() override;
private:
	Timer timer;
};

class PlayerAttack3State :public StateNode {
public:
	PlayerAttack3State();
	~PlayerAttack3State() = default;

	void on_enter()override;
	void on_update(float delta) override;
	void on_exit() override;
private:
	Timer timer;
};

class PlayerDeadState :public StateNode {
public:
	PlayerDeadState();
	~PlayerDeadState() = default;

	void on_enter()override;
	void on_update(float delta) override;

private:
	Timer timer;
};

class PlayerFallState :public StateNode {
public:
	PlayerFallState() = default;
	~PlayerFallState() = default;

	void on_enter()override;
	void on_update(float delta) override;
};

class PlayerIdleState :public StateNode {
public:
	PlayerIdleState() = default;
	~PlayerIdleState() = default;

	void on_enter()override;
	void on_update(float delta) override;
};

class PlayerJumpState :public StateNode {
public:
	PlayerJumpState() = default;
	~PlayerJumpState() = default;

	void on_enter()override;
	void on_update(float delta) override;
};

class PlayerRunState :public StateNode {
public:
	PlayerRunState() = default;
	~PlayerRunState() = default;

	void on_enter()override;
	void on_update(float delta) override;
	void on_exit() override;
};

class PlayerTakeHitState : public StateNode {  //¿‡À∆”⁄roll
public:
	PlayerTakeHitState();
	~PlayerTakeHitState() = default;

	void on_enter()override;
	void on_update(float delta) override;
	void on_exit()override;
private:
	Timer timer;
};