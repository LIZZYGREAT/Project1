#include"enemy_attack_box.h"


Attack1Box::Attack1Box() {
	shape = Shape::round;
	r = 0;
	timer_stage1_to_2.set_one_shot(true);
	timer_stage1_to_2.set_wait_time(0.5f);
	timer_stage1_to_2.set_on_timeout([&]() {
		if (stage == Stage::one)
			stage = Stage::two;

	});
	timer_stage2_to_3.set_one_shot(true);
	timer_stage2_to_3.set_wait_time(1.5f);
	timer_stage2_to_3.set_on_timeout([&]() {
		if (stage == Stage::two)
			stage = Stage::three;
		});
	timer_stage3_to_end.set_one_shot(true);
	timer_stage3_to_end.set_wait_time(1.5f);
	timer_stage3_to_end.set_on_timeout([&]() {
		if (stage == Stage::three) {
			stage = Stage::end;
		}
	});
}

void Attack1Box::update_r(float delta) {
	if (stage == Stage::one&& timer_stage1_to_2.get_pass_time()<=0.32f) {
		r+= R_SPEED_stage1 * delta;
	}
	else if (stage == Stage::two ) {
		r += R_SPEED_stage2 * delta;
	}
	else if (stage == Stage::three) {
		r -= R_SPEED_stage2 * delta;
	}
	else if (stage == Stage::end) {
		r = 0.0f;
	}
}
void Attack1Box::update_r_center(const vector2& enemy_center) {
	r_center = enemy_center;
	set_position(r_center);
}

Attack2Box::Attack2Box() {
	shape = Shape::round;
	r = 0;
	timer_stage1_to_2.set_one_shot(true);
	timer_stage1_to_2.set_wait_time(0.50f);
	timer_stage1_to_2.set_on_timeout([&]() {
		if (stage == Stage::one)
			stage = Stage::two;
		});
	timer_stage2_to_3.set_one_shot(true);
	timer_stage2_to_3.set_wait_time(0.50f);
	timer_stage2_to_3.set_on_timeout([&]() {
		if (stage == Stage::two)
			stage = Stage::three;
		});
}

void Attack2Box::update_r_center(const vector2& player_center) {
	r_center = player_center;
	set_position(r_center);;
}

void Attack2Box::update_r(float delta) {
	if (stage == Stage::one) {
		r += R_SPEED * delta;
	}
	if (stage == Stage::two) {
		r -= R_SPEED * delta;
	}
	if (stage == Stage::three) {
		r = 0.0f;
	}
}

Attack4Box::Attack4Box() {
	shape = Shape::rect;
	timer_stage1_to_2.set_one_shot(true);
	timer_stage1_to_2.set_wait_time(0.72f);
	timer_stage1_to_2.set_on_timeout([&]() {
		if (stage == Stage::one)
			stage = Stage::two;
		});
}
void Attack4Box::init_position(const vector2& position, bool is_facing_left) {
	int x = (is_facing_left == 1) ? 1 : -1;

	set_position({ position.x +130.0f * x , position.y });
	
}
void Attack4Box::update_position(bool is_facing_left,float delta) {
	if(stage == Stage::one)
	position += vector2({ is_facing_left * SPREAD_SPEED * delta,0 });
}

void Attack4Box::update_size(float delta) {
	if(stage == Stage::one)
	size += vector2({ SIZE_SPEED * delta,0 });
	else {
		size = vector2({ 0,0 });
	}
}

Attack5Box::Attack5Box() {
	shape = Shape::rect;

	timer_stage1_to_2.set_one_shot(true);
	timer_stage1_to_2.set_wait_time(0.77f);
	timer_stage1_to_2.set_on_timeout([&]() {
		if (stage == Stage::one)
			stage = Stage::two;
		});

}

void Attack5Box::init_position(const vector2& enemy_position, bool is_facing_left) {
	int x = (is_facing_left == 1) ? 1 : -1;
	set_position({ enemy_position.x + 130.0f * x , enemy_position.y });
}
void Attack5Box::update_position(bool is_facing_left,float delta) {
	if(stage == Stage::one)
	position += vector2({ is_facing_left * SPREAD_SPEED * delta,0 });
}

void Attack5Box::update_size(float delta) {
	if (timer_stage1_to_2.get_pass_time() <= 0.64)
	{
		size += vector2({ SIZE_SPEED * delta,0 });
	}
	else {
		size = { 0.0f,0.0f };
	}
}

TeleportOutBox::TeleportOutBox() {
	shape = Shape::round;
	r = 0;
	timer_stage1_to_2.set_one_shot(true);
	timer_stage1_to_2.set_wait_time(1.2f);
	timer_stage1_to_2.set_on_timeout([&]() {
		if (stage == Stage::one)
			stage = Stage::two;
		});
}

void TeleportOutBox::update_r_center(const vector2 & position) {
	r_center = position;
	set_position(r_center);
}

void TeleportOutBox::update_r(float delta) {
	if (timer_stage1_to_2.get_pass_time() <= 0.20f) {
		r += R_SPEED * delta;
	}
	else if (timer_stage1_to_2.get_pass_time() <= 0.72) {
	}
	else if (timer_stage1_to_2.get_pass_time() <= 0.96f) {
		if (r >= 0) {
			r = min(r - R_SPEED * delta, 0);
		}
	}
	else {
		r = 0.0f;
	}
}