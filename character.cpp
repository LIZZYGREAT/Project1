#include"character.h"
#include"collision_manager.h"
#include"character_manager.h"

Character::Character() {

	hurt_box = CollisionManager::instance()->create_collision_box();

	timer_invulnerable_status.set_one_shot(true);
	timer_invulnerable_status.set_wait_time(0.50f);
	timer_invulnerable_status.set_on_timeout([&]() {
		is_invulnerable = false;
		});

}

Character::~Character() {

	CollisionManager::instance()->destroy_collision_box(hurt_box);
}

void Character::on_input(const ExMessage& msg) {
}

void Character::on_update(float delta) {

	state_machine.on_update(delta);

	if (hp <= 0)
		velocity.x = 0;

	if (enable_gravity)
		velocity.y += gravity * delta;

	position += velocity * delta;

	if (position.y >= Floor_y + frame_size/3) {
		position.y = Floor_y + frame_size/3;
		velocity.y = 0;
	}

	if (position.x <= 0)position.x = 0;

	if (position.x >= getwidth())position.x = (float)getwidth();

	hurt_box->set_position(get_logic_center());

	timer_invulnerable_status.on_update(delta);

	if (!current_animation)return;

	Animation& animation = (is_facing_left ? current_animation->left : current_animation->right);

	animation.on_update(delta);
	animation.set_position(position);
}


void Character::on_render() {
	if (!current_animation)return;


	(is_animation_facing_left ? current_animation->left : current_animation->right).on_render();
}

void Character::on_take_hit() {}


void Character::switch_state(const std::string& id) {
	state_machine.switch_to(id);
}

void Character::set_animation(const std::string& id) {
	current_animation = &animation_pool[id];

	current_animation->left.reset();
	current_animation->right.reset();
}

void Character::decrease_hp() {

};