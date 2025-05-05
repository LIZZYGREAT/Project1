#include"enemy_attack5.h"
#include"resources_manager.h"
#include"collision_manager.h"

EnemyAttack5::EnemyAttack5(const vector2& position,bool is_facing_left) {

	animation.set_interval(0.07f);
	animation.set_loop(false);
	animation.set_anchor_mode(Animation::AnchorMode::Centered);
	animation.add_frame(ResourcesManager::instance()->find_atlas(is_facing_left?"enemy_attack5_vfx_left":"enemy_attack5_vfx_right"));

	attack_box = (Attack5Box*)CollisionManager::instance()->create_attack5_box();
	attack_box->set_layer_src(CollisionLayer::Enemy);
	attack_box->set_layer_dst(CollisionLayer::Player);
	attack_box->set_on_collide([&]() {
		is_valid = false;
		});
	
	attack_box->init_position(position, is_facing_left);
}

EnemyAttack5::~EnemyAttack5() {
	if (attack_box != nullptr)
		CollisionManager::instance()->destroy_collision_box(attack_box);
	attack_box = nullptr;
}


void EnemyAttack5::update(float delta, bool is_facing_left) {

	attack_box->update_position(delta, is_facing_left);
	attack_box->update_size(delta);
	animation.on_update(delta);

	if (attack_box->get_stage() == CollisionBox::Stage::one) {
		attack_box->get_timer_stage1_to_2()->on_update(delta);
	}

	if (attack_box->get_stage() == CollisionBox::Stage::two) {
		is_valid = false;
	}

}

void EnemyAttack5::draw() {
	animation.on_render();
}

