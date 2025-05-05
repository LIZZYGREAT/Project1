#include"enemy_attack2.h"
#include"resources_manager.h"
#include"collision_manager.h"

EnemyAttack2::EnemyAttack2() {

	animation.set_interval(0.02f);
	animation.set_loop(false);
	animation.set_anchor_mode(Animation::AnchorMode::Centered);
	animation.add_frame(ResourcesManager::instance()->find_image("enemy_attack2_vfx"), 5, 8);

	attack_box = (Attack2Box*)CollisionManager::instance()->create_attack2_box();
	attack_box->set_layer_src(CollisionLayer::Enemy);
	attack_box->set_layer_dst(CollisionLayer::Player);
	attack_box->set_on_collide([&]() {
		if (attack_box->get_stage() == CollisionBox::Stage::one)
			is_valid_stage1 = false;
		else if (attack_box->get_stage() == CollisionBox::Stage::two)
			is_valid_stage2 = false;
		});

}

EnemyAttack2::~EnemyAttack2() {
	if(attack_box!=nullptr)
	CollisionManager::instance()->destroy_collision_box(attack_box);
	attack_box = nullptr;
}


void EnemyAttack2::update(float delta) {

	attack_box->update_r(delta);
	animation.on_update(delta);

	if (attack_box->get_stage() == CollisionBox::Stage::one) {
		attack_box->get_timer_stage1_to_2()->on_update(delta);
	}
	else if (attack_box->get_stage() == CollisionBox::Stage::two) {
		attack_box->get_timer_stage2_to_3()->on_update(delta);
	}

	if (attack_box->get_stage() == CollisionBox::Stage::three) {
		is_valid_stage1 = false;
		is_valid_stage2 = false;
	}

}

void EnemyAttack2::draw() {

	animation.on_render();

}