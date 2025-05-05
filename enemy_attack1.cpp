#include"enemy_attack1.h"
#include"resources_manager.h"
#include"collision_manager.h"


EnemyAttack1::EnemyAttack1() {

	animation.set_interval(0.05f);
	animation.set_loop(false);
	animation.set_anchor_mode(Animation::AnchorMode::Centered);
	animation.add_frame(ResourcesManager::instance()->find_atlas("enemy_attack1_vfx"));


	attack_box = (Attack1Box*)CollisionManager::instance()->create_attack1_box();
	attack_box->set_layer_src(CollisionLayer::Enemy);
	attack_box->set_layer_dst(CollisionLayer::Player);
	attack_box->set_on_collide([&]() {
		if (attack_box->get_stage() == CollisionBox::Stage::one)
			is_valid_stage1 = false;
		else if (attack_box->get_stage() == CollisionBox::Stage::two)
			is_valid_stage2 = false;
		else if (attack_box->get_stage() == CollisionBox::Stage::three)
			is_valid_stage3 = false;
		});

}

EnemyAttack1::~EnemyAttack1() {
	if (attack_box != nullptr) {
		CollisionManager::instance()->destroy_collision_box(attack_box);
		attack_box = nullptr;
	}
}

void EnemyAttack1::update(float delta) {

	attack_box->update_r(delta);
	animation.on_update(delta);

	if (attack_box->get_stage() == CollisionBox::Stage::one) {
		attack_box->get_timer_stage1_to_2()->on_update(delta);
	}
	else if (attack_box->get_stage() == CollisionBox::Stage::two) {
		attack_box->get_timer_stage2_to_3()->on_update(delta);
	}
	else if (attack_box->get_stage() == CollisionBox::Stage::two) {
		attack_box->get_timer_stage3_to_end()->on_update(delta);
	}

	if (attack_box->get_stage() == CollisionBox::Stage::end) {
		is_valid_stage1 = false;
		is_valid_stage2 = false;
		is_valid_stage3 = false;
	}

}

void EnemyAttack1::draw() {
	if (attack_box->get_stage() == CollisionBox::Stage::one)
	{
		animation.on_render();
	}
	else if (attack_box->get_stage() == CollisionBox::Stage::two || attack_box->get_stage() == CollisionBox::Stage::three)
	{
		circle(attack_box->get_r_center().x,attack_box->get_r_center().y,attack_box->get_r() );
	}

}