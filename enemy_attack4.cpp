#include"enemy_attack4.h"
#include"resources_manager.h"
#include"collision_manager.h"

EnemyAttack4::EnemyAttack4(const vector2& position,bool is_facing_left) {

	animation.set_interval(0.08f);
	animation.set_loop(false);
	animation.set_anchor_mode(Animation::AnchorMode::Centered);
	animation.add_frame(ResourcesManager::instance()->find_atlas(is_facing_left?"enemy_attack4_vfx_left":"enemy_attack4_vfx_right"));

	attack_box = (Attack4Box*)CollisionManager::instance()->create_attack4_box();
	attack_box->set_layer_src(CollisionLayer::Enemy);
	attack_box->set_layer_dst(CollisionLayer::Player);
	attack_box->set_on_collide([&]() {
		is_valid = false;
		});

	attack_box->init_position(position, is_facing_left);

}

EnemyAttack4::~EnemyAttack4() {
	if (attack_box != nullptr)
		CollisionManager::instance()->destroy_collision_box(attack_box);
	attack_box = nullptr;
}


void EnemyAttack4::update(float delta,bool is_facing_left) {

	attack_box->update_position(delta,is_facing_left);
	attack_box->update_size(delta);
	animation.on_update(delta);

	if (attack_box->get_stage() == CollisionBox::Stage::one) {
		attack_box->get_timer_stage1_to_2()->on_update(delta);
	}

	if (attack_box->get_stage() == CollisionBox::Stage::two) {
		is_valid = false;
	}

}

void EnemyAttack4::draw() {

	animation.on_render();

}

