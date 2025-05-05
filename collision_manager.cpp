#include"collision_manager.h"
#include<graphics.h>

CollisionManager* CollisionManager::manager = nullptr;

CollisionManager* CollisionManager::instance() {
	if (!manager) {
		manager = new CollisionManager();
	}
	return manager;
}

CollisionManager::CollisionManager() {
	attack1_box = new CollisionBox();
	attack2_box = new CollisionBox();
	attack3_box = new CollisionBox();
	seeThrough_box_attack2 = new CollisionBox();
	seeThrough_box_attack3 = new CollisionBox();

	collision_box_list.push_back(attack1_box); 
	collision_box_list.push_back(attack2_box);
	collision_box_list.push_back(attack3_box);
	collision_box_list.push_back(seeThrough_box_attack2);
	collision_box_list.push_back(seeThrough_box_attack3);

	Timer* timer1 = attack1_box->get_timer_stage1_to_2();
	Timer* timer2 = attack1_box->get_timer_stage2_to_3();
	Timer* timer3 = attack1_box->get_timer_stage3_to_end();

	Timer* timer1_2 = attack2_box->get_timer_stage1_to_2();
	Timer* timer2_2 = attack2_box->get_timer_stage2_to_3();
	Timer* timer3_2 = attack2_box->get_timer_stage3_to_end();

	Timer* timer1_3 = attack3_box->get_timer_stage1_to_2();
	Timer* timer2_3 = attack3_box->get_timer_stage2_to_3();
	Timer* timer3_3 = attack3_box->get_timer_stage3_to_end();

	timer1->set_one_shot(true);
	timer1->set_wait_time(0.32f);
	timer1->set_on_timeout([&]() {
		if (attack1_box->get_stage() == CollisionBox::Stage::one) {
			attack1_box->set_stage(CollisionBox::Stage::two);	
		}
	});

	timer2->set_one_shot(true);
	timer2->set_wait_time(0.16f);
	timer2->set_on_timeout([&]() {
		if (attack1_box->get_stage() == CollisionBox::Stage::two) {
			attack1_box->set_stage(CollisionBox::Stage::three);
		}
	});

	timer3->set_one_shot(true);
	timer3->set_wait_time(0.08f);
	timer3->set_on_timeout([&]() {
		if (attack1_box->get_stage() == CollisionBox::Stage::three) {
			attack1_box->set_stage(CollisionBox::Stage::end);
		}
	});

	timer1_2->set_one_shot(true);
	timer1_2->set_wait_time(0.32f);
	timer1_2->set_on_timeout([&]() {
		if (attack2_box->get_stage() == CollisionBox::Stage::one)
			attack2_box->set_stage(CollisionBox::Stage::two);
		});

	timer2_2->set_one_shot(true);
	timer2_2->set_wait_time(0.16f);
	timer2_2->set_on_timeout([&]() {
		if (attack2_box->get_stage() == CollisionBox::Stage::two) {
			attack2_box->set_stage(CollisionBox::Stage::three);
		}
		});

	timer3_2->set_one_shot(true);
	timer3_2->set_wait_time(0.08f);
	timer3_2->set_on_timeout([&]() {
		if (attack2_box->get_stage() == CollisionBox::Stage::three) {
			attack2_box->set_stage(CollisionBox::Stage::end);
		}
		});

	timer1_3->set_one_shot(true);
	timer1_3->set_wait_time(0.32f);
	timer1_3->set_on_timeout([&]() {
		if (attack3_box->get_stage() == CollisionBox::Stage::one)
			attack3_box->set_stage(CollisionBox::Stage::two);
		});

	timer2_3->set_one_shot(true);
	timer2_3->set_wait_time(0.16f);
	timer2_3->set_on_timeout([&]() {
		if (attack3_box->get_stage() == CollisionBox::Stage::two) {
			attack3_box->set_stage(CollisionBox::Stage::three);
		}
		});

	timer3_3->set_one_shot(true);
	timer3_3->set_wait_time(0.08f);
	timer3_3->set_on_timeout([&]() {
		if (attack3_box->get_stage() == CollisionBox::Stage::three) {
			attack3_box->set_stage(CollisionBox::Stage::end);
		}
		});

	Timer* see_inv_2 = seeThrough_box_attack2->get_timer_seeThrough_invalid();
	Timer* see_v_2 = seeThrough_box_attack2->get_timer_seeThrough_valid();

	Timer* see_inv_3 = seeThrough_box_attack3->get_timer_seeThrough_invalid();
	Timer* see_v_3 = seeThrough_box_attack3->get_timer_seeThrough_valid();

	see_inv_2->set_one_shot(true);
	see_inv_2->set_wait_time(0.08f);
	see_inv_2->set_on_timeout([&]() {
		if (seeThrough_box_attack2->get_state() == CollisionBox::State::invalid) {
			seeThrough_box_attack2->set_state(CollisionBox::State::valid);
			seeThrough_box_attack2->set_enabled(true);
		}
	});
	see_inv_2->set_one_shot(true);
	see_inv_2->set_wait_time(0.32f);
	see_inv_2->set_on_timeout([&]() {
		if (seeThrough_box_attack2->get_state() == CollisionBox::State::valid) {
			seeThrough_box_attack2->set_state(CollisionBox::State::invalid);
			seeThrough_box_attack2->set_enabled(false);
		}
		});

	see_inv_3->set_one_shot(true);
	see_inv_3->set_wait_time(0.24f);
	see_inv_3->set_on_timeout([&]() {
		if (seeThrough_box_attack3->get_state() == CollisionBox::State::invalid) {
			seeThrough_box_attack3->set_state(CollisionBox::State::valid);
			seeThrough_box_attack3->set_enabled(true);
		}
		});
	see_inv_3->set_one_shot(true);
	see_inv_3->set_wait_time(0.32f);
	see_inv_3->set_on_timeout([&]() {
		if (seeThrough_box_attack3->get_state() == CollisionBox::State::valid) {
			seeThrough_box_attack3->set_state(CollisionBox::State::invalid);
			seeThrough_box_attack3->set_enabled(false);
		}
	});

	attack1_box->set_layer_src(CollisionLayer::None);
	attack1_box->set_layer_dst(CollisionLayer::Enemy);

	attack2_box->set_layer_src(CollisionLayer::None);
	attack2_box->set_layer_dst(CollisionLayer::Enemy);

	attack3_box->set_layer_src(CollisionLayer::None);
	attack3_box->set_layer_dst(CollisionLayer::Enemy);

	seeThrough_box_attack2->set_layer_src(CollisionLayer::Player);
	seeThrough_box_attack2->set_layer_dst(CollisionLayer::Enemy);

	seeThrough_box_attack3->set_layer_src(CollisionLayer::Player);
	seeThrough_box_attack3->set_layer_dst(CollisionLayer::Enemy);

};

CollisionManager::~CollisionManager() {
	destroy_collision_box(attack1_box);
	destroy_collision_box(attack2_box);
	destroy_collision_box(attack3_box);
	destroy_collision_box(seeThrough_box_attack2);
	destroy_collision_box(seeThrough_box_attack3);

	delete manager;
}

CollisionBox* CollisionManager::create_collision_box() {
	CollisionBox* collision_box = new CollisionBox();
	collision_box_list.push_back(collision_box);

	return collision_box;
}

Attack1Box* CollisionManager::create_attack1_box() {
	Attack1Box* attack1_box = new Attack1Box();
	collision_box_list.push_back(attack1_box);
	return attack1_box;
}

Attack2Box* CollisionManager::create_attack2_box() {
	Attack2Box* attack2_box = new Attack2Box();
	collision_box_list.push_back(attack2_box);
	return attack2_box;
}

Attack4Box* CollisionManager::create_attack4_box() {
	Attack4Box* attack4_box = new Attack4Box();
	collision_box_list.push_back(attack4_box);
	return attack4_box;
}

Attack5Box* CollisionManager::create_attack5_box() {
	Attack5Box* attack5_box = new Attack5Box();
	collision_box_list.push_back(attack5_box);
	return attack5_box;
}

TeleportOutBox* CollisionManager::create_teleport_out_box() {
	TeleportOutBox* teleport_out_box = new TeleportOutBox();
	collision_box_list.push_back(teleport_out_box);
	return teleport_out_box;
}

void CollisionManager::destroy_collision_box(CollisionBox* collision_box) {

	collision_box_list.erase(std::remove(collision_box_list.begin(),
		collision_box_list.end(), collision_box), collision_box_list.end()); //remove是只把所有与collision_box相同的移到vector最后，
	//且得到其迭代器，erase是将从该位置，到end()前部从vector中删除
	delete collision_box;
}

void CollisionManager::process_collide() {
	for (auto* collision_box_src : collision_box_list) {
		if (!collision_box_src->enabled || collision_box_src->layer_dst == CollisionLayer::None)
			continue;
		for (auto* collision_box_dst : collision_box_list) {
			if (!collision_box_dst || collision_box_src == collision_box_dst ||
				collision_box_src->layer_dst != collision_box_dst->layer_src) {
				continue;
			}
			bool is_collide_x = (max(collision_box_src->position.x + collision_box_src->size.x / 2,
				collision_box_dst->position.x + collision_box_dst->size.x / 2)
				- min(collision_box_src->position.x - collision_box_src->size.x / 2,
					collision_box_dst->position.x - collision_box_dst->size.x / 2)
				<= collision_box_src->size.x + collision_box_dst->size.x);
			bool is_collide_y = (max(collision_box_src->position.y + collision_box_src->size.y / 2,
				collision_box_dst->position.y + collision_box_dst->size.y / 2)
				- min(collision_box_src->position.y - collision_box_src->size.y / 2,
					collision_box_dst->position.y - collision_box_dst->size.y / 2)
				<= collision_box_src->size.y + collision_box_dst->size.y);

			if (is_collide_x && is_collide_y && collision_box_dst->on_collide)
				collision_box_dst->on_collide();
		}
	}
}

void CollisionManager::on_debug_render() {
	for (auto* collision_box : collision_box_list) {
		setlinecolor(collision_box->enabled ? RGB(255, 195, 195) : RGB(115, 115, 175));
		rectangle((int)(collision_box->position.x - collision_box->size.x / 2),
			(int)(collision_box->position.y - collision_box->size.y / 2),
			(int)(collision_box->position.x + collision_box->size.x / 2),
			(int)(collision_box->position.y + collision_box->size.y / 2));

	}
}