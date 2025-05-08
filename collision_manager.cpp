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


	
	enemy_attack1_box = new CollisionBox();
	enemy_attack2_box = new CollisionBox();
	enemy_attack3_box = new CollisionBox();
	enemy_attack4_box = new CollisionBox();

	enemy_attack3_vfx_box = new CollisionBox();
	enemy_attack4_vfx_box = new CollisionBox();

	enemy_attack1_box->set_layer_src(CollisionLayer::Enemy);
	enemy_attack1_box->set_layer_dst(CollisionLayer::Player);

	enemy_attack2_box->set_layer_src(CollisionLayer::Enemy);
	enemy_attack2_box->set_layer_dst(CollisionLayer::Player);


	enemy_attack3_box->set_layer_src(CollisionLayer::Enemy);
	enemy_attack3_box->set_layer_dst(CollisionLayer::Player);

	enemy_attack4_box->set_layer_src(CollisionLayer::Enemy);
	enemy_attack4_box->set_layer_dst(CollisionLayer::Player);


	enemy_attack3_vfx_box->set_layer_src(CollisionLayer::Enemy);
	enemy_attack3_vfx_box->set_layer_dst(CollisionLayer::Player);

	enemy_attack4_vfx_box->set_layer_src(CollisionLayer::Enemy);
	enemy_attack4_vfx_box->set_layer_dst(CollisionLayer::Player);

	Timer* enemy_timer1_1 = enemy_attack1_box->get_timer_stage1_to_2();
	Timer* enemy_timer2_1 = enemy_attack1_box->get_timer_stage2_to_3();

	Timer* enemy_timer1_2 = enemy_attack2_box->get_timer_stage1_to_2();
	Timer* enemy_timer2_2 = enemy_attack2_box->get_timer_stage2_to_3();

	Timer* enemy_timer1_3 = enemy_attack3_box->get_timer_stage1_to_2();
	Timer* enemy_timer2_3 = enemy_attack3_box->get_timer_stage2_to_3();

	Timer* enemy_timer1_4 = enemy_attack4_box->get_timer_stage1_to_2();
	Timer* enemy_timer2_4 = enemy_attack4_box->get_timer_stage2_to_3();

	Timer* enemy_timer1_3_vfx = enemy_attack3_vfx_box->get_timer_stage1_to_2();

	Timer* enemy_timer1_4_vfx = enemy_attack4_vfx_box->get_timer_stage1_to_2();

	enemy_timer1_1->set_one_shot(true);
	enemy_timer1_1->set_wait_time(0.40f);
	enemy_timer1_1->set_on_timeout([&]() {
		if (enemy_attack1_box->get_stage() == CollisionBox::Stage::one) {
			enemy_attack1_box->set_stage(CollisionBox::Stage::two);
		}
	});

	enemy_timer2_1->set_one_shot(true);
	enemy_timer2_1->set_wait_time(0.20f);
	enemy_timer2_1->set_on_timeout([&]() {
		if (enemy_attack1_box->get_stage() == CollisionBox::Stage::two) {
			enemy_attack1_box->set_stage(CollisionBox::Stage::three);
		}
		});

	enemy_timer1_2->set_one_shot(true);
	enemy_timer1_2->set_wait_time(0.40f);
	enemy_timer1_2->set_on_timeout([&]() {
		if (enemy_attack2_box->get_stage() == CollisionBox::Stage::one) {
			enemy_attack2_box->set_stage(CollisionBox::Stage::two);
		}
		});

	enemy_timer2_2->set_one_shot(true);
	enemy_timer2_2->set_wait_time(0.20f);
	enemy_timer2_2->set_on_timeout([&]() {
		if (enemy_attack2_box->get_stage() == CollisionBox::Stage::two) {
			enemy_attack2_box->set_stage(CollisionBox::Stage::three);
		}
		});

	enemy_timer1_3->set_one_shot(true);
	enemy_timer1_3->set_wait_time(0.40f);
	enemy_timer1_3->set_on_timeout([&]() {
		if (enemy_attack3_box->get_stage() == CollisionBox::Stage::one) {
			enemy_attack3_box->set_stage(CollisionBox::Stage::two);
		}
		});

	enemy_timer2_3->set_one_shot(true);
	enemy_timer2_3->set_wait_time(0.40f);
	enemy_timer2_3->set_on_timeout([&]() {
		if (enemy_attack3_box->get_stage() == CollisionBox::Stage::two) {
			enemy_attack3_box->set_stage(CollisionBox::Stage::three);
		}
		});

	enemy_timer1_4->set_one_shot(true);
	enemy_timer1_4->set_wait_time(0.40f);
	enemy_timer1_4->set_on_timeout([&]() {
		if (enemy_attack4_box->get_stage() == CollisionBox::Stage::one) {
			enemy_attack4_box->set_stage(CollisionBox::Stage::two);
		}
		});

	enemy_timer2_4->set_one_shot(true);
	enemy_timer2_4->set_wait_time(0.40f);
	enemy_timer2_4->set_on_timeout([&]() {
		if (enemy_attack4_box->get_stage() == CollisionBox::Stage::two) {
			enemy_attack4_box->set_stage(CollisionBox::Stage::three);
		}
		});

	enemy_timer1_3_vfx->set_one_shot(true);
	enemy_timer1_3_vfx->set_wait_time(0.80f);
	enemy_timer1_3_vfx->set_on_timeout([&]() {
		if (enemy_attack3_vfx_box->get_stage() == CollisionBox::Stage::one) {
			enemy_attack3_vfx_box->set_stage(CollisionBox::Stage::two);
		}
		});

	enemy_timer1_4_vfx->set_one_shot(true);
	enemy_timer1_4_vfx->set_wait_time(0.80f);
	enemy_timer1_4_vfx->set_on_timeout([&]() {
		if (enemy_attack4_vfx_box->get_stage() == CollisionBox::Stage::one) {
			enemy_attack4_vfx_box->set_stage(CollisionBox::Stage::two);
		}
	});

	collision_box_list.push_back(enemy_attack1_box);
	collision_box_list.push_back(enemy_attack2_box);

	collision_box_list.push_back(enemy_attack3_box);
	collision_box_list.push_back(enemy_attack4_box);

	collision_box_list.push_back(enemy_attack3_vfx_box);
	collision_box_list.push_back(enemy_attack4_vfx_box);

};

CollisionManager::~CollisionManager() {
	destroy_collision_box(attack1_box);
	destroy_collision_box(attack2_box);
	destroy_collision_box(attack3_box);
	destroy_collision_box(seeThrough_box_attack2);
	destroy_collision_box(seeThrough_box_attack3);


	destroy_collision_box(enemy_attack1_box);
	destroy_collision_box(enemy_attack2_box);
	destroy_collision_box(enemy_attack3_box);
	destroy_collision_box(enemy_attack3_vfx_box);
	destroy_collision_box(enemy_attack4_box);
	destroy_collision_box(enemy_attack4_vfx_box);

	delete manager;
}

CollisionBox* CollisionManager::create_collision_box() {
	CollisionBox* collision_box = new CollisionBox();
	collision_box_list.push_back(collision_box);

	return collision_box;
}


void CollisionManager::destroy_collision_box(CollisionBox* collision_box) {

	collision_box_list.erase(std::remove(collision_box_list.begin(),
		collision_box_list.end(), collision_box), collision_box_list.end()); //remove��ֻ��������collision_box��ͬ���Ƶ�vector���
	//�ҵõ����������erase�ǽ��Ӹ�λ�ã���end()ǰ����vector��ɾ��
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
			//��������μ��
			bool is_collide_x = (max(collision_box_src->position.x + collision_box_src->size.x / 2,
				collision_box_dst->position.x + collision_box_dst->size.x / 2)- min(collision_box_src->position.x - collision_box_src->size.x / 2,collision_box_dst->position.x - collision_box_dst->size.x / 2)<= collision_box_src->size.x + collision_box_dst->size.x);

			bool is_collide_y = (max(collision_box_src->position.y + collision_box_src->size.y / 2,
				collision_box_dst->position.y + collision_box_dst->size.y / 2)- min(collision_box_src->position.y - collision_box_src->size.y / 2, collision_box_dst->position.y - collision_box_dst->size.y / 2)<= collision_box_src->size.y + collision_box_dst->size.y);

			if (is_collide_x && is_collide_y && collision_box_dst->on_collide){
					collision_box_dst->on_collide();
			}
		}
	}
}

void CollisionManager::on_debug_render() {
	for (auto* collision_box : collision_box_list) {
		{
			setlinecolor(collision_box->enabled ? RGB(255, 195, 195) : RGB(115, 115, 175));
			rectangle((int)(collision_box->position.x - collision_box->size.x / 2),
				(int)(collision_box->position.y - collision_box->size.y / 2),
				(int)(collision_box->position.x + collision_box->size.x / 2),
				(int)(collision_box->position.y + collision_box->size.y / 2));
		}

	}
}
