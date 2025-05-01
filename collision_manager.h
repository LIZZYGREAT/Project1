#pragma once

#include"collision_box.h"
#include<vector>

class CollisionManager {
public:

	static CollisionManager* instance();

private:
	CollisionBox* attack1_box;
	CollisionBox* attack2_box;
	CollisionBox* attack3_box;
	CollisionBox* seeThrough_box_attack2;
	CollisionBox* seeThrough_box_attack3;

public:

	CollisionBox* get_attack1_box() {
		return attack1_box;
	}
	CollisionBox* get_attack2_box() {
		return attack2_box;
	}
	CollisionBox* get_attack3_box() {
		return attack3_box;
	}
	CollisionBox* get_see_through_box_attack2() {
		return seeThrough_box_attack2;
	}
	CollisionBox* get_see_through_box_attack3() {
		return seeThrough_box_attack3;
	}
	CollisionBox* create_collision_box();

	void destroy_collision_box(CollisionBox* collision_box);

	void process_collide();
	void on_debug_render();

private:
	static CollisionManager* manager;

	std::vector<CollisionBox*>collision_box_list;

public:
	CollisionManager();
	~CollisionManager();
};