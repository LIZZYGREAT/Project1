#include"player.h"
#include"resources_manager.h"
#include"player_state_nodes.h"
#include"character_manager.h"

#include<cmath>

Player::Player() {
	is_facing_left = false;
	logic_height = 300;
	position = { 250,200 };
	hp = 15;

	hurt_box->set_size({ 40,80 });
	hurt_box->set_layer_src(CollisionLayer::Player);
	hurt_box->set_layer_dst(CollisionLayer::None);


	hurt_box->set_on_collide([&]() {
		decrease_hp();
		});


	timer_attack1_cd.set_wait_time(CD_ATTACK);
	timer_attack1_cd.set_one_shot(true);
	timer_attack1_cd.set_on_timeout([&]() {
		is_attack1_cd_cmp = true; });

	timer_attack2_cd.set_wait_time(CD_ATTACK);
	timer_attack2_cd.set_one_shot(true);
	timer_attack2_cd.set_on_timeout([&]() {
		is_attack2_cd_cmp = true; });

	timer_attack3_cd.set_wait_time(CD_ATTACK);
	timer_attack3_cd.set_one_shot(true);
	timer_attack3_cd.set_on_timeout([&]() {
		is_attack3_cd_cmp = true; });



	{
		AnimationGroup& animation_attack1 = animation_pool["attack1"];
		Animation& animation_attack1_left = animation_attack1.left;
		animation_attack1_left.set_interval(0.08f);
		animation_attack1_left.set_loop(false);
		animation_attack1_left.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_attack1_left.add_frame(ResourcesManager::instance()->find_image("player_attack1_left"), 7);

		Animation& animation_attack1_right = animation_attack1.right;
		animation_attack1_right.set_interval(0.08f);
		animation_attack1_right.set_loop(false);
		animation_attack1_right.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_attack1_right.add_frame(ResourcesManager::instance()->find_image("player_attack1_right"), 7);

		AnimationGroup& animation_attack2 = animation_pool["attack2"];
		Animation& animation_attack2_left = animation_attack2.left;
		animation_attack2_left.set_interval(0.08f);
		animation_attack2_left.set_loop(false);
		animation_attack2_left.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_attack2_left.add_frame(ResourcesManager::instance()->find_image("player_attack2_left"), 7);

		Animation& animation_attack2_right = animation_attack2.right;
		animation_attack2_right.set_interval(0.08f);
		animation_attack2_right.set_loop(false);
		animation_attack2_right.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_attack2_right.add_frame(ResourcesManager::instance()->find_image("player_attack2_right"), 7);

		AnimationGroup& animation_attack3 = animation_pool["attack3"];
		Animation& animation_attack3_left = animation_attack3.left;
		animation_attack3_left.set_interval(0.08f);
		animation_attack3_left.set_loop(false);
		animation_attack3_left.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_attack3_left.add_frame(ResourcesManager::instance()->find_image("player_attack3_left"), 8);

		Animation& animation_attack3_right = animation_attack3.right;
		animation_attack3_right.set_interval(0.08f);
		animation_attack3_right.set_loop(false);
		animation_attack3_right.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_attack3_right.add_frame(ResourcesManager::instance()->find_image("player_attack3_right"), 8);

	}

	{
		AnimationGroup& animation_dead = animation_pool["dead"];
		Animation& animation_dead_left = animation_dead.left;
		animation_dead_left.set_interval(0.15f);
		animation_dead_left.set_loop(false);
		animation_dead_left.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_dead_left.add_frame(ResourcesManager::instance()->find_image("player_dead_left"), 7);

		Animation& animation_dead_right = animation_dead.right;
		animation_dead_right.set_interval(0.15f);
		animation_dead_right.set_loop(false);
		animation_dead_right.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_dead_right.add_frame(ResourcesManager::instance()->find_image("player_dead_right"), 7);
	}
	{
		AnimationGroup& animation_fall = animation_pool["fall"];
		Animation& animation_fall_left = animation_fall.left;
		animation_fall_left.set_interval(0.15f);
		animation_fall_left.set_loop(true);
		animation_fall_left.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_fall_left.add_frame(ResourcesManager::instance()->find_image("player_fall_left"), 3);

		Animation& animation_fall_right = animation_fall.right;
		animation_fall_right.set_interval(0.15f);
		animation_fall_right.set_loop(true);
		animation_fall_right.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_fall_right.add_frame(ResourcesManager::instance()->find_image("player_fall_right"), 3);
	}
	{
		AnimationGroup& animation_idle = animation_pool["idle"];
		Animation& animation_idle_left = animation_idle.left;
		animation_idle_left.set_interval(0.15f);
		animation_idle_left.set_loop(true);
		animation_idle_left.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_idle_left.add_frame(ResourcesManager::instance()->find_image("player_idle_left"), 10);

		Animation& animation_idle_right = animation_idle.right;
		animation_idle_right.set_interval(0.15f);
		animation_idle_right.set_loop(true);
		animation_idle_right.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_idle_right.add_frame(ResourcesManager::instance()->find_image("player_idle_right"), 10);
	}
	{
		AnimationGroup& animation_jump = animation_pool["jump"];
		Animation& animation_jump_left = animation_jump.left;
		animation_jump_left.set_interval(0.15f);
		animation_jump_left.set_loop(false);
		animation_jump_left.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_jump_left.add_frame(ResourcesManager::instance()->find_image("player_jump_left"), 3);

		Animation& animation_jump_right = animation_jump.right;
		animation_jump_right.set_interval(0.15f);
		animation_jump_right.set_loop(false);
		animation_jump_right.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_jump_right.add_frame(ResourcesManager::instance()->find_image("player_jump_right"), 3);
	}
	{
		AnimationGroup& animation_run = animation_pool["run"];
		Animation& animation_run_left = animation_run.left;
		animation_run_left.set_interval(0.075f);
		animation_run_left.set_loop(true);
		animation_run_left.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_run_left.add_frame(ResourcesManager::instance()->find_image("player_run_left"), 8);

		Animation& animation_run_right = animation_run.right;
		animation_run_right.set_interval(0.075f);
		animation_run_right.set_loop(true);
		animation_run_right.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_run_right.add_frame(ResourcesManager::instance()->find_image("player_run_right"), 8);

		//受击
		AnimationGroup& animation_take_hit = animation_pool["take_hit"];
		Animation& animation_take_hit_left = animation_take_hit.left;
		animation_take_hit_left.set_interval(0.10f);
		animation_take_hit_left.set_loop(false);
		animation_take_hit_left.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_take_hit_left.add_frame(ResourcesManager::instance()->find_image("player_take_hit_left"), 3);

		Animation& animation_take_hit_right = animation_take_hit.right;
		animation_take_hit_right.set_interval(0.10f);
		animation_take_hit_right.set_loop(false);
		animation_take_hit_right.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_take_hit_right.add_frame(ResourcesManager::instance()->find_image("player_take_hit_right"), 3);
	}

	{
		animation_jump_vfx.set_interval(0.05f);
		animation_jump_vfx.set_loop(false);
		animation_jump_vfx.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_jump_vfx.add_frame(ResourcesManager::instance()->find_image("player_vfx_jump"), 5);
		animation_jump_vfx.set_on_finished([&]() {
			is_jump_vfx_visible = false;
			});

		animation_land_vfx.set_interval(0.1f);
		animation_land_vfx.set_loop(false);
		animation_land_vfx.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_land_vfx.add_frame(ResourcesManager::instance()->find_image("player_vfx_land"), 2);
		animation_land_vfx.set_on_finished([&]() {
			is_land_vfx_visible = false;

			});

		{
			state_machine.register_state("attack1", new PlayerAttack1State());
			state_machine.register_state("attack2", new PlayerAttack2State());
			state_machine.register_state("attack3", new PlayerAttack3State());
			state_machine.register_state("dead", new PlayerDeadState());
			state_machine.register_state("fall", new PlayerFallState());
			state_machine.register_state("idle", new PlayerIdleState());
			state_machine.register_state("jump", new PlayerJumpState());
			state_machine.register_state("run", new PlayerRunState());
			state_machine.register_state("take_hit", new PlayerTakeHitState());

			state_machine.set_entry("idle");
		}
	}
}

Player::~Player() {}


void Player::on_input(const ExMessage& msg) {
	if (hp <= 0) return;

	switch (msg.message) {
	case WM_KEYDOWN:
		switch (msg.vkcode) {
		case 0x41://A
			is_left_key_down = true;
			break;
		case 0x44://D
			is_right_key_down = true;
			break;
		case 0x57://W
			is_jump_key_down = true;
			break;
		case 0x4A://J键
			is_attack1_key_down = true;
			break;
		case 0x4B://K键
			is_attack2_key_down = true;
			break;
		case 0x52://G键
			is_attack3_key_down = true;
			break;
		}
		break;
	case WM_KEYUP:
		switch (msg.vkcode) {
		case 0x41://A
			is_left_key_down = false;
			break;
		case 0x44://D
			is_right_key_down = false;
			break;
		case 0x57: //W
			is_jump_key_down = false;
			break;
		case 0x4A://J键
			is_attack1_key_down = false;
			break;
		case 0x4B://K键
			is_attack2_key_down = false;
			break;
		case 0x52://G键
			is_attack3_key_down = false;
			break;
		}
		break;
	}
}

void Player::on_update(float delta) {
	if (hp > 0 && (!is_taking_hit)&&(!is_attacking)) {
		velocity.x = get_move_axis() * SPEED_RUN;
	}
	else if (hp > 0  && is_on_floor() == 0 && velocity.x!=0 ) {
		velocity.x =  (is_facing_left==0? 1 : -1)* SPEED_JUMP_RUN;
	}
	else if (hp > 0 && (!is_taking_hit) && is_attacking && velocity.x!=0) {
		velocity.x = (is_facing_left == 0 ? 1 : -1) * SPEED_ATTACK_RUN;
	}

	if (get_move_axis() != 0 ) {
		is_facing_left = (get_move_axis() < 0);
	}

	timer_attack1_cd.on_update(delta);
	timer_attack2_cd.on_update(delta);
	timer_attack3_cd.on_update(delta);

	animation_jump_vfx.on_update(delta);
	animation_land_vfx.on_update(delta);

	Character::on_update(delta);
}

void Player::on_render() {
	if (is_jump_vfx_visible) {
		animation_jump_vfx.on_render();
	}
	if (is_land_vfx_visible) {
		animation_land_vfx.on_render();
	}

	Character::on_render();

}

void Player::on_take_hit() {
	play_audio(_T("player_hurt"), false);
}

void Player::on_jump() {
	velocity.y -= SPEED_JUMP;
	is_jump_vfx_visible = true;
	animation_jump_vfx.set_position({ position.x,position.y - 85 });
	animation_jump_vfx.reset();
}

void Player::on_land() {
	is_land_vfx_visible = true;
	animation_land_vfx.set_position({ position.x,position.y - 85 });
	animation_land_vfx.reset();
}
void Player::on_attack1() {
	timer_attack1_cd.restart();
	is_attack1_cd_cmp = false;
}
void Player::on_attack2() {
	timer_attack2_cd.restart();
	is_attack2_cd_cmp = false;
}
void Player::on_attack3() {
	timer_attack3_cd.restart();
	is_attack3_cd_cmp = false;
}



//to do list
//跳跃优化(提供二段跳，但跳跃效果减弱)
//攻击（识破)
//技能幻影斩击
//增加能量条
//修改攻击和碰撞逻辑和受击逻辑
//优化奔跑