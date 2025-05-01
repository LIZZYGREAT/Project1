#pragma once

#include"animation.h"
#include"state_machine.h"
#include"vector2.h"

#include"collision_box.h"

#include<string>
#include<graphics.h>
#include<unordered_map>

class Character {
public:
	Character();
	~Character();

	int get_hp() const {
		return hp;
	}
	void decrease_hp();
	void set_position(const vector2& position) {
		this->position = position;
	}
	const vector2& get_position()const {
		return position;
	}
	void set_velocity(const vector2& velocity) {
		this->velocity = velocity;
	}
	const vector2& get_velocity() const {
		return velocity;
	}
	vector2 get_logic_center() const {
		return vector2(position.x, position.y - logic_height / 2);//一定要知道是-而不是+ 1/2height
	}
	void set_gravity_enabled(bool flag) {
		enable_gravity = flag;
	}
	CollisionBox* get_hurt_box() {
		return hurt_box;
	}
	bool is_on_floor()const {
		return position.y >= Floor_y+100;
	}
	float get_floor_y() const {
		return Floor_y;
	}
	void make_invulnerable() {
		is_invulnerable = true;
		timer_invulnerable_status.restart();
	}
	virtual void on_input(const ExMessage& msg);
	virtual void on_update(float delta);
	virtual void on_render();

	virtual void on_take_hit();
	void switch_state(const std::string& id);
	void set_animation(const std::string& id);
protected:
	struct AnimationGroup
	{
		Animation left;
		Animation right;
	};

protected:
	const float Floor_y = 680;
	const float gravity = 980 * 1.5;
protected:
	int hp = 10;
	vector2 position;
	vector2 velocity;

	float logic_height = 0;
	bool is_facing_left = true;

	StateMachine state_machine;

	bool enable_gravity = true;
	bool is_invulnerable = false;

	Timer timer_invulnerable_status;


	CollisionBox* hurt_box = nullptr;


	AnimationGroup* current_animation = nullptr;

	std::unordered_map<std::string, AnimationGroup> animation_pool;
};