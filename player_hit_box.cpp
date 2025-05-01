/*#include"player_hit_box.h"

PlayerAttack1HitBox::PlayerAttack1HitBox() {
	timer_stage1_to_2.set_one_shot(true);
	timer_stage1_to_2.set_wait_time(0.32f);
	timer_stage1_to_2.set_on_timeout([&]() {
		if (stage == Stage::one)
			stage = Stage::two;
		if (stage == Stage::two) {
			timer_stage2_to_3.restart();
		}
	});
	timer_stage2_to_3.set_one_shot(true);
	timer_stage2_to_3.set_wait_time(0.16f);
	timer_stage2_to_3.set_on_timeout([&]() {
		if (stage == Stage::two) {
			stage = Stage::three;
		}
		if (stage == Stage::three) {
			timer_stage3_to_end.restart();
		}
	});
	timer_stage3_to_end.set_one_shot(true);
	timer_stage3_to_end.set_wait_time(0.08f);
	timer_stage3_to_end.set_on_timeout([&]() {
		if (stage == Stage::three) {
			stage = Stage::end;
		}
	});

	hit_box->set_layer_src(CollisionLayer::None);
	hit_box->set_layer_dst(CollisionLayer::Enemy);
}

void PlayerAttack1HitBox::update_box_size() {
	if (stage == Stage::one) {
		hit_box->set_size({ 50.0f,35.0f });
	}
	else if (stage == Stage::two) {
		hit_box->set_size({ 110.0f,100.0f });
	}
	else if (stage == Stage::three) {
		hit_box->set_size({ 110.0f,100.0f });
	}
}
void PlayerAttack1HitBox::update_box_position(const vector2& position) {
	if (stage == Stage::one) {
		set_position({ position.x-25.0f,position.y+50.0f+hit_box->get_size().y/2});
	}
	if (stage == Stage::two) {
		set_position({ position.x + hit_box->get_size().x/2,position.y });
	}
	if (stage == Stage::three) {
		set_position({ position.x + hit_box->get_size().x/2,position.y });
	}
}


PlayerAttack2HitBox::PlayerAttack2HitBox() {
	timer_stage1_to_2.set_one_shot(true);
	timer_stage1_to_2.set_wait_time(0.16f);
	timer_stage1_to_2.set_on_timeout([&]() {
		if (stage == Stage::one)
			stage = Stage::two;
		});
	timer_stage2_to_3.set_one_shot(true);
	timer_stage2_to_3.set_wait_time(0.08f);
	timer_stage2_to_3.set_on_timeout([&]() {
		if (stage == Stage::two) {
			stage = Stage::three;
		}
	});
	timer_stage3_to_end.set_one_shot(true);
	timer_stage3_to_end.set_wait_time(0.16f);
	timer_stage3_to_end.set_on_timeout([&]() {
		if (stage == Stage::three) {
			stage = Stage::end;
		}
	});

	hit_box->set_layer_src(CollisionLayer::None);
	hit_box->set_layer_dst(CollisionLayer::Enemy);
}

void PlayerAttack2HitBox::update_box_size() {
	if (stage == Stage::one) {
		hit_box->set_size({ 75.0f,50.0f });
	}
	else if (stage == Stage::two) {
		hit_box->set_size({ 180.0f,100.0f });
	}
	else if (stage == Stage::three) {
		hit_box->set_size({ 75.0f,100.0f });
	}

}
void PlayerAttack2HitBox::update_box_position(const vector2& position) {
	
	if (stage == Stage::one) {
		set_position({ position.x - 17.5f,position.y - 25.0f});
	}
	if (stage == Stage::two) {
		set_position({ position.x + hit_box->get_size().x / 2,position.y });
	}
	if (stage == Stage::three) {
		set_position({ position.x -25.0f- hit_box->get_size().x / 2,position.y });
	}
}

PlayerAttack3HitBox::PlayerAttack3HitBox() {
	timer_stage1_to_2.set_one_shot(true);
	timer_stage1_to_2.set_wait_time(0.32f);
	timer_stage1_to_2.set_on_timeout([&]() {
		if (stage == Stage::one)
			stage = Stage::two;
	});
	timer_stage2_to_3.set_one_shot(true);
	timer_stage2_to_3.set_wait_time(0.16f);
	timer_stage2_to_3.set_on_timeout([&]() {
		if (stage == Stage::two) {
			stage = Stage::three;
		}
	});
	timer_stage3_to_end.set_one_shot(true);
	timer_stage3_to_end.set_wait_time(0.08f);
	timer_stage3_to_end.set_on_timeout([&]() {
		if (stage == Stage::three) {
			stage = Stage::end;
		}
	});

	hit_box->set_layer_src(CollisionLayer::None);
	hit_box->set_layer_dst(CollisionLayer::Enemy);
}

void PlayerAttack3HitBox::update_box_size() {
	if (stage == Stage::one) {
		hit_box->set_size({ 100.0f,35.0f });
	}
	else if (stage == Stage::two) {
		hit_box->set_size({ 120.0f,100.0f });
	}
	else if (stage == Stage::three) {
		hit_box->set_size({ 120.0f,100.0f });
	}

}
void PlayerAttack3HitBox::update_box_position(const vector2& position) {
	if (stage == Stage::one) {
		set_position({position.x ,position.y +50.0f+ hit_box->get_size().y/2});
	}
	if (stage == Stage::two) {
		set_position({ position.x + hit_box->get_size().x / 2,position.y });
	}
	if (stage == Stage::three) {
		set_position({ position.x + hit_box->get_size().x / 2,position.y });
	}
}*/