#pragma once

#include"vector2.h"
#include"collision_layer.h"
#include"timer.h"
#include<graphics.h>
#include<functional>

class CollisionManager;

class CollisionBox {

	friend class CollisionManager;
public:
	enum class  Stage {
		one, two, three, end,
	};
	enum class State {
		valid,invalid,
	};
private:
	vector2 size;
	vector2 position;  //碰撞箱中心点所处位置
	bool enabled = true;
	std::function<void()>on_collide;
	CollisionLayer layer_src = CollisionLayer::None;    //src代表源 source
	CollisionLayer layer_dst = CollisionLayer::None;	//dst代表目的

	int attack_val_stage1 = 1;
	int attack_val_stage2 = 3;
	int attack_val_stage3 = 1;
	Timer timer_stage1_to_2;
	Timer timer_stage2_to_3;
	Timer timer_stage3_to_end;

	Timer timer_seeThrough_valid;   //有效状态计时器
	Timer timer_seeThrough_invalid; //无效状态计时器

	Stage stage = Stage::one;
	State state = State::invalid;

public:
	CollisionBox() = default;
	~CollisionBox() = default;
public:
	void set_enabled(bool flag) {
		enabled = flag;
	}
	bool get_enabled()const {
		return enabled;
	}
	void set_layer_src(CollisionLayer layer) {
		layer_src = layer;
	}
	void set_layer_dst(CollisionLayer layer) {
		layer_dst = layer;
	}
	void set_on_collide(std::function<void()> on_collide) {
		this->on_collide = on_collide;
	}
	void set_size(const vector2& size) {
		this->size = size;
	}
	const vector2& get_size()const {
		return size;
	}
	void set_position(const vector2& position) {
		this->position = position;
	}
	const vector2& get_position()const {
		return position;
	}


	//新加接口
	void set_attack_stage1(int val) {
		attack_val_stage1 = val;
	}
	void set_attack_stage2(int val) {
		attack_val_stage2 = val;
	}
	void set_attack_stage3(int val) {
		attack_val_stage3 = val;
	}


	void update_attack1_position(const vector2& position,bool is_facing_left) {
		int x = is_facing_left == 1 ? 1 : -1;
		if (stage == Stage::one) {
			set_position({ position.x + x * get_size().x / 2,position.y + 25.0f });
		}
		else if (stage == Stage::two) {
			set_position({ position.x - get_size().x / 2 *x,position.y });
		}
		else if (stage == Stage::three) {
			set_position({ position.x - get_size().x / 2 *x,position.y });
		}
	}
	void update_attack1_size() {
		if (stage == Stage::one) {
		    set_size({ 80.0f,75.0f });
		}
		else if (stage == Stage::two) {
			set_size({ 110.0f,100.0f });
		}
		else if (stage == Stage::three) {
			set_size({ 110.0f,100.0f });
		}
	}


	void update_attack2_position(const vector2& position,bool is_facing_left) {
		int x = is_facing_left == 1 ? 1 : -1;
		if (stage == Stage::one) {
			set_position({ position.x - x * get_size().x / 2,position.y - get_size().y / 2 });
		}
		else if (stage == Stage::two) {
			set_position({ position.x - x * get_size().x / 2 ,position.y });
		}
		else if (stage == Stage::three) {
			set_position({ position.x + x * get_size().x / 2,position.y });
		}
	}
	void update_attack2_size() {
		if (stage == Stage::one) {
			set_size({ 50.0f,50.0f });
		}
		else if (stage == Stage::two) {
			set_size({ 100.0f,100.0f });
		}
		else if (stage == Stage::three) {
			set_size({ 100.0f,100.0f });
		}
	}


	void update_attack3_position(const vector2& position,bool is_facing_left) {
		int x = is_facing_left == 1 ? 1 : -1;
		if (stage == Stage::one) {
			set_position({ position.x+ 20.0f * x ,position.y - get_size().y / 2 - 50 });
		}
		else if (stage == Stage::two) {
			set_position({ position.x - x *(-25+ get_size().x / 2) ,position.y -40.0f});
		}
		else if (stage == Stage::three) {
			set_position({ position.x - x*(-25 + get_size().x / 2),position.y-40.0f });
		}
	}
	void update_attack3_size() {
		if (stage == Stage::one) {
			set_size({ 160.0f,60.0f });
		}
		else if (stage == Stage::two) {
			set_size({ 140.0f,200.0f });
		}
		else if (stage == Stage::three) {
			set_size({ 140.0f,200.0f });
		}
	}


	void update_seeThrough_box_size_attack2() {
		set_size({ 150.0f,150.0f });
	}
	void update_seeThrough_box_position_attack2(const vector2& position, bool is_facing_left) {
		set_position({ position.x ,position.y });
	}


	void update_seeThrough_box_size_attack3() {
		set_size({ 140.0f,200.0f });
	}
	void update_seeThrough_box_position_attack3(const vector2& position, bool is_facing_left) {
		int x = is_facing_left == 1 ? 1 : -1;
		set_position({ position.x - x * (-25 + get_size().x / 2) ,position.y - 40.0f });
		
	}


	Timer* get_timer_stage1_to_2() {
		return &timer_stage1_to_2;
	}

	Timer* get_timer_stage2_to_3() {
		return &timer_stage2_to_3;
	}

	Timer* get_timer_stage3_to_end() {
		return &timer_stage3_to_end;
	}

	Timer* get_timer_seeThrough_valid() {
		return &timer_seeThrough_valid;
	}

	Timer* get_timer_seeThrough_invalid() {
		return &timer_seeThrough_invalid;
	}

	const Stage& get_stage() const{
		return stage;
	}
	void set_stage(Stage stage) {
		this->stage = stage;
	}

	const State& get_state() const{
		return state;
	}
	void set_state(State state) {
		this->state = state;
	}


	void on_debug_render() {
		setlinecolor(enabled ? RGB(255, 195, 195) : RGB(115, 115, 175));
		rectangle((int)(position.x - size.x / 2),
			(int)(position.y - size.y / 2),
			(int)(position.x + size.x / 2),
			(int)(position.y + size.y / 2));
	}
};


