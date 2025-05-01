#pragma once
#include"collision_manager.h"
#include"player.h"

class PlayerAttack1HitBox :public CollisionBox{
public:
	PlayerAttack1HitBox();
	~PlayerAttack1HitBox();

	void update_box_position(const vector2& position);
private:

};
