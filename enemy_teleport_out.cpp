#include"enemy_teleport_out.h"
#include"resources_manager.h"
#include"collision_manager.h"

EnemyTeleportOut::EnemyTeleportOut(const vector2& position) {

	attack_box = (TeleportOutBox*)CollisionManager::instance()->create_teleport_out_box();
	attack_box->set_layer_src(CollisionLayer::Enemy);
	attack_box->set_layer_dst(CollisionLayer::Player);
	attack_box->set_on_collide([&]() {
		is_valid = false;
		});

	attack_box->init_position(position);
}

EnemyTeleportOut::~EnemyTeleportOut() {
	if (attack_box != nullptr)
		CollisionManager::instance()->destroy_collision_box(attack_box);
	attack_box = nullptr;
}


void EnemyTeleportOut::update(float delta) {

	attack_box->update_r(delta);
	animation.on_update(delta);

	if (attack_box->get_stage() == CollisionBox::Stage::one) {
		attack_box->get_timer_stage1_to_2()->on_update(delta);
	}

	if (attack_box->get_stage() == CollisionBox::Stage::two) {
		is_valid = false;
	}

}

void EnemyTeleportOut::draw() {
	animation.on_render();
}

