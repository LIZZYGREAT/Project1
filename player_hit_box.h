/*#pragma once
#include"collision_box.h"
#include"character_manager.h"


class PlayerAttack1HitBox :public CollisionBox{
private:
	enum class  Stage {
		one,two,three,end,
	};
public:
	PlayerAttack1HitBox();
	~PlayerAttack1HitBox() =default;

	void update_box_position(const vector2& position);
	void update_box_size();

	Timer* get_timer1_to_2()  {
		return &timer_stage1_to_2;
	}
private:
	int attack_val_stage1 = 1;
	int attack_val_stage2 = 3;
	int attack_val_stage3 = 1;
	Timer timer_stage1_to_2;
	Timer timer_stage2_to_3;
	Timer timer_stage3_to_end;
	Stage stage = Stage::one;

	PlayerAttack1HitBox* hit_box = nullptr;
};

class PlayerAttack2HitBox : public CollisionBox {
private:
	enum class  Stage {
		one, two, three,end,
	};
public:
	PlayerAttack2HitBox();
	~PlayerAttack2HitBox() =default;

	void update_box_position(const vector2& position) ;
	void update_box_size() ;
private:
	int attack_val_stage1 = 1;
	int attack_val_stage2 = 4;
	int attack_val_stage3 = 1;
	Timer timer_stage1_to_2;
	Timer timer_stage2_to_3;
	Timer timer_stage3_to_end;
	Stage stage = Stage::one;
	PlayerAttack2HitBox* hit_box = nullptr;
};

class PlayerAttack3HitBox : public CollisionBox {
private:
	enum class  Stage {
		one, two, three,end,
	};
public:
	PlayerAttack3HitBox();
	~PlayerAttack3HitBox() = default;

	void update_box_position(const vector2& position);
	void update_box_size() ;
private:
	int attack_val_stage1 = 1;
	int attack_val_stage2 = 4;
	int attack_val_stage3 = 1;
	Timer timer_stage1_to_2;
	Timer timer_stage2_to_3;
	Timer timer_stage3_to_end;
	Stage stage = Stage::one;
	PlayerAttack3HitBox* hit_box = nullptr;
};


/*class SeeThroughHitBox : public CollisionBox {
public:
	SeeThroughHitBox();
	~SeeThroughHitBox() = default;

private:
	Timer timer_see_through;
};*/

*/