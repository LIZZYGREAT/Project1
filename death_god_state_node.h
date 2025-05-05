#pragma once
#include"death_god.h"
#include"character_manager.h"
#include"collision_manager.h"
#include"timer.h"
#include"state_node.h"
#include"player.h"



class Attack1State :public StateNode {
public:
	Attack1State();
	~Attack1State()=default;
	void on_enter();
	void on_update(float delta);
	void on_exit();

private:
	Timer timer1; //人物动画的持续时间
	Timer timer2; //攻击的持续时间
};


class Attack2State :public StateNode {
public:
	Attack2State();
	~Attack2State() =default;
	void on_enter();
	void on_update(float delta);
	void on_exit();

private:
	Timer timer1; //人物动画的持续时间
	Timer timer2; //攻击的持续时间
};

class Attack3State :public StateNode {
public:
	Attack3State();
	~Attack3State() = default;
	void on_enter();
	void on_update(float delta);
	void on_exit();

private:
	Timer timer1; //人物动画的持续时间
};

class Attack4State :public StateNode {
public:
	Attack4State();
	~Attack4State() = default;
	void on_enter();
	void on_update(float delta);
	void on_exit();

private:
	Timer timer1; //人物动画的持续时间
	Timer timer2; //攻击的持续时间
};

class Attack5State :public StateNode {
public:
	Attack5State();
	~Attack5State() =default;
	void on_enter();
	void on_update(float delta);
	void on_exit();

private:
	Timer timer1; //人物动画的持续时间
	Timer timer2; //攻击的持续时间
};


class TeleportOutState :public StateNode {
public:
	TeleportOutState();
	~TeleportOutState() = default;
	void on_enter();
	void on_update(float delta);
	void on_exit();
private:
	Timer timer;
};

class TeleportInState : public StateNode {
public:
	TeleportInState();
	~TeleportInState() =default;
	void on_enter();
	void on_update(float delta);
	void on_exit();
private:
	Timer timer;
};

class DeadState : public StateNode {
public:
	DeadState();
	~DeadState() = default;
	void on_enter();
	void on_update(float delta);

private:
	Timer timer;
};


class IdleState : public StateNode {
public:
	IdleState() = default;
	~IdleState() =default;
	void on_enter();
	void on_update(float delta);
};

class RunState : public StateNode {
public:
	RunState() = default;
	~RunState() = default;
	void on_enter();
	void on_update(float delta);
};

class FallState : public StateNode {
public:
	FallState() = default;
	~FallState() = default;
	void on_enter();
	void on_update(float delta);
};