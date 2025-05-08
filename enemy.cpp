#include"enemy.h"
#include"resources_manager.h"
#include"character_manager.h"
#include"enemy_state_nodes.h"

#include<cmath>

Enemy::Enemy() {

	is_facing_left = true;
	logic_height = 350;
	frame_size = 353.0f;
	position = { 850,200 };

	hp = 100;
	max_hp = hp;

	hurt_box->set_size({ 60,100 });
	hurt_box->set_layer_src(CollisionLayer::Enemy);
	hurt_box->set_layer_dst(CollisionLayer::None);


	hurt_box->set_on_collide([&]() {
		decrease_hp();
		if (!is_attacking&&is_on_floor()) {
			switch_state("enemy_take_hit");
		}
		});


	timer_attack1_cd.set_wait_time(CD_ATTACK);
	timer_attack1_cd.set_one_shot(true);
	timer_attack1_cd.set_on_timeout([&]() {
		is_attack1_cd_cmp = true; });

	timer_attack2_cd.set_wait_time(CD_ATTACK);
	timer_attack2_cd.set_one_shot(true);
	timer_attack2_cd.set_on_timeout([&]() {
		is_attack2_cd_cmp = true; });

	timer_attack3_cd.set_wait_time(3.0f);
	timer_attack3_cd.set_one_shot(true);
	timer_attack3_cd.set_on_timeout([&]() {
		is_attack3_cd_cmp = true; });

	timer_attack4_cd.set_wait_time(3.0f);
	timer_attack4_cd.set_one_shot(true);
	timer_attack4_cd.set_on_timeout([&]() {
		is_attack4_cd_cmp = true; });


	{
		AnimationGroup& animation_attack1 = animation_pool["enemy_attack1"];
		Animation& animation_attack1_left = animation_attack1.left;
		animation_attack1_left.set_interval(0.10f);
		animation_attack1_left.set_loop(false);
		animation_attack1_left.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_attack1_left.add_frame(ResourcesManager::instance()->find_image("enemy_attack1_left"), 6);

		Animation& animation_attack1_right = animation_attack1.right;
		animation_attack1_right.set_interval(0.10f);
		animation_attack1_right.set_loop(false);
		animation_attack1_right.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_attack1_right.add_frame(ResourcesManager::instance()->find_image("enemy_attack1_right"), 6);

		AnimationGroup& animation_attack2 = animation_pool["enemy_attack2"];
		Animation& animation_attack2_left = animation_attack2.left;
		animation_attack2_left.set_interval(0.10f);
		animation_attack2_left.set_loop(false);
		animation_attack2_left.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_attack2_left.add_frame(ResourcesManager::instance()->find_image("enemy_attack2_left"), 6);

		Animation& animation_attack2_right = animation_attack2.right;
		animation_attack2_right.set_interval(0.10f);
		animation_attack2_right.set_loop(false);
		animation_attack2_right.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_attack2_right.add_frame(ResourcesManager::instance()->find_image("enemy_attack2_right"), 6);

		AnimationGroup& animation_attack3 = animation_pool["enemy_attack3"];
		Animation& animation_attack3_left = animation_attack3.left;
		animation_attack3_left.set_interval(0.10f);
		animation_attack3_left.set_loop(false);
		animation_attack3_left.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_attack3_left.add_frame(ResourcesManager::instance()->find_image("enemy_attack1_left"), 6);
		animation_attack3_left.add_frame(ResourcesManager::instance()->find_image("enemy_attack3_left"), 2);

		Animation& animation_attack3_right = animation_attack3.right;
		animation_attack3_right.set_interval(0.10f);
		animation_attack3_right.set_loop(false);
		animation_attack3_right.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_attack3_right.add_frame(ResourcesManager::instance()->find_image("enemy_attack1_right"), 6);
		animation_attack3_right.add_frame(ResourcesManager::instance()->find_image("enemy_attack3_right"), 2);

		AnimationGroup& animation_attack4 = animation_pool["enemy_attack4"];
		Animation& animation_attack4_left = animation_attack4.left;
		animation_attack4_left.set_interval(0.10f);
		animation_attack4_left.set_loop(false);
		animation_attack4_left.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_attack4_left.add_frame(ResourcesManager::instance()->find_image("enemy_attack2_left"), 6);
		animation_attack4_left.add_frame(ResourcesManager::instance()->find_image("enemy_attack4_left"), 2);

		Animation& animation_attack4_right = animation_attack4.right;
		animation_attack4_right.set_interval(0.10f);
		animation_attack4_right.set_loop(false);
		animation_attack4_right.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_attack4_right.add_frame(ResourcesManager::instance()->find_image("enemy_attack2_right"), 6);
		animation_attack4_right.add_frame(ResourcesManager::instance()->find_image("enemy_attack4_right"), 2);

	}

	{
		AnimationGroup& animation_dead = animation_pool["enemy_dead"];
		Animation& animation_dead_left = animation_dead.left;
		animation_dead_left.set_interval(0.15f);
		animation_dead_left.set_loop(false);
		animation_dead_left.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_dead_left.add_frame(ResourcesManager::instance()->find_image("enemy_dead_left"), 6);

		Animation& animation_dead_right = animation_dead.right;
		animation_dead_right.set_interval(0.15f);
		animation_dead_right.set_loop(false);
		animation_dead_right.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_dead_right.add_frame(ResourcesManager::instance()->find_image("enemy_dead_right"), 6);
	}
	{
		AnimationGroup& animation_fall = animation_pool["enemy_fall"];
		Animation& animation_fall_left = animation_fall.left;
		animation_fall_left.set_interval(0.08f);
		animation_fall_left.set_loop(true);
		animation_fall_left.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_fall_left.add_frame(ResourcesManager::instance()->find_image("enemy_fall_left"), 2);

		Animation& animation_fall_right = animation_fall.right;
		animation_fall_right.set_interval(0.08f);
		animation_fall_right.set_loop(true);
		animation_fall_right.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_fall_right.add_frame(ResourcesManager::instance()->find_image("enemy_fall_right"), 2);
	}
	{
		AnimationGroup& animation_idle = animation_pool["enemy_idle"];

		Animation& animation_idle_left = animation_idle.left;
		animation_idle_left.set_interval(0.08f);
		animation_idle_left.set_loop(true);
		animation_idle_left.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_idle_left.add_frame(ResourcesManager::instance()->find_image("enemy_idle_left"), 8);

		Animation& animation_idle_right = animation_idle.right;
		animation_idle_right.set_interval(0.08f);
		animation_idle_right.set_loop(true);
		animation_idle_right.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_idle_right.add_frame(ResourcesManager::instance()->find_image("enemy_idle_right"), 8);
	}
	{
		AnimationGroup& animation_jump = animation_pool["enemy_jump"];
		Animation& animation_jump_left = animation_jump.left;
		animation_jump_left.set_interval(0.08f);
		animation_jump_left.set_loop(false);
		animation_jump_left.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_jump_left.add_frame(ResourcesManager::instance()->find_image("enemy_jump_left"), 2);

		Animation& animation_jump_right = animation_jump.right;
		animation_jump_right.set_interval(0.08f);
		animation_jump_right.set_loop(false);
		animation_jump_right.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_jump_right.add_frame(ResourcesManager::instance()->find_image("enemy_jump_right"), 2);
	}
	{
		AnimationGroup& animation_run = animation_pool["enemy_run"];
		Animation& animation_run_left = animation_run.left;
		animation_run_left.set_interval(0.075f);
		animation_run_left.set_loop(true);
		animation_run_left.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_run_left.add_frame(ResourcesManager::instance()->find_image("enemy_run_left"), 8);

		Animation& animation_run_right = animation_run.right;
		animation_run_right.set_interval(0.075f);
		animation_run_right.set_loop(true);
		animation_run_right.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_run_right.add_frame(ResourcesManager::instance()->find_image("enemy_run_right"), 8);

		//受击
		AnimationGroup& animation_take_hit = animation_pool["enemy_take_hit"];
		Animation& animation_take_hit_left = animation_take_hit.left;
		animation_take_hit_left.set_interval(0.125f);
		animation_take_hit_left.set_loop(false);
		animation_take_hit_left.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_take_hit_left.add_frame(ResourcesManager::instance()->find_image("player_take_hit_left"), 4);

		Animation& animation_take_hit_right = animation_take_hit.right;
		animation_take_hit_right.set_interval(0.125f);
		animation_take_hit_right.set_loop(false);
		animation_take_hit_right.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_take_hit_right.add_frame(ResourcesManager::instance()->find_image("player_take_hit_right"), 4);
	}

	{
		animation_jump_vfx.set_interval(0.05f);
		animation_jump_vfx.set_loop(false);
		animation_jump_vfx.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_jump_vfx.add_frame(ResourcesManager::instance()->find_image("enemy_vfx_jump"), 5);
		animation_jump_vfx.set_on_finished([&]() {
			is_jump_vfx_visible = false;
			});

		animation_land_vfx.set_interval(0.1f);
		animation_land_vfx.set_loop(false);
		animation_land_vfx.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		animation_land_vfx.add_frame(ResourcesManager::instance()->find_image("enemy_vfx_land"), 2);
		animation_land_vfx.set_on_finished([&]() {
			is_land_vfx_visible = false;
			});

		attack3_vfx.set_interval(0.08f);
		attack3_vfx.set_loop(false);
		attack3_vfx.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		attack3_vfx.add_frame(ResourcesManager::instance()->find_image("enemy_attack3_vfx"), 2,5);
		attack3_vfx.set_on_finished([&]() {
			is_attack3_vfx_visible = false;
			});

		attack4_vfx.set_interval(0.08f);
		attack4_vfx.set_loop(false);
		attack4_vfx.set_anchor_mode(Animation::AnchorMode::BottomCentered);
		attack4_vfx.add_frame(ResourcesManager::instance()->find_image("enemy_attack4_vfx"), 2,5);
		attack4_vfx.set_on_finished([&]() {
			is_attack4_vfx_visible = false;
			});


		{
			state_machine.register_state("enemy_attack1", new EnemyAttack1State());
			state_machine.register_state("enemy_attack2", new EnemyAttack2State());
			state_machine.register_state("enemy_attack3", new EnemyAttack3State());
			state_machine.register_state("enemy_attack4", new EnemyAttack4State());

			state_machine.register_state("enemy_dead", new EnemyDeadState());
			state_machine.register_state("enemy_fall", new EnemyFallState());
			state_machine.register_state("enemy_idle", new EnemyIdleState());
			state_machine.register_state("enemy_jump", new EnemyJumpState());
			state_machine.register_state("enemy_run", new EnemyRunState());
			state_machine.register_state("enemy_take_hit", new EnemyTakeHitState());

			state_machine.set_entry("enemy_idle");
		}
	}
}

Enemy::~Enemy() {}

void Enemy::on_update(float delta) {
	
	if (CharacterManager::instance()->get_player()->get_position().x < position.x) {
		is_facing_left = true;
	}

	timer_attack1_cd.on_update(delta);
	timer_attack2_cd.on_update(delta);
	timer_attack3_cd.on_update(delta);
	timer_attack4_cd.on_update(delta);

	if(is_jump_vfx_visible)
	animation_jump_vfx.on_update(delta);

	if(is_land_vfx_visible)
	animation_land_vfx.on_update(delta);

	if (is_attack3_vfx_visible) {
		attack3_vfx.on_update(delta);
	}
	if (is_attack4_vfx_visible) {
		attack4_vfx.on_update(delta);
	}

	Character::on_update(delta);
}

void Enemy::on_render() {

	if (is_jump_vfx_visible) {
		animation_jump_vfx.on_render();
	}
	if (is_land_vfx_visible) {
		animation_land_vfx.on_render();
	}
	if (is_attack3_vfx_visible) {
		attack3_vfx.on_render();
	}
	if (is_attack4_vfx_visible) {
		attack4_vfx.on_render();
	}


	Character::on_render();

	draw_hp();
}


void Enemy::on_jump() {
	velocity.y -= SPEED_JUMP;
	is_jump_vfx_visible = true;
	animation_jump_vfx.set_position({ position.x,position.y - 85 });
	animation_jump_vfx.reset();
}

void Enemy::on_land() {
	is_land_vfx_visible = true;
	animation_land_vfx.set_position({ position.x,position.y - 85 });
	animation_land_vfx.reset();
}
void Enemy::on_attack1() {
	timer_attack1_cd.restart();
	is_attack1_cd_cmp = false;
}
void Enemy::on_attack2() {
	timer_attack2_cd.restart();
	is_attack2_cd_cmp = false;
}
void Enemy::on_attack3() {
	timer_attack3_cd.restart();
	is_attack3_cd_cmp = false;
	attack3_vfx.reset();
}
void Enemy::on_attack4() {
	timer_attack4_cd.restart();
	is_attack4_cd_cmp = false;
	attack4_vfx.reset();
}

void Enemy::draw_hp() {
	int start_x = 300;
	int end_x = 980;
	int start_y = 60;
	int end_y = 77;
}

void Enemy::decrease_hp() {
	Character::decrease_hp();
	if (hurt_box->get_enabled() == false && is_invulnerable) return;
	hp -= 5;
	if (hp > 0) {
		make_invulnerable();
	}
	on_take_hit();
}

//to do list
//跳跃优化(提供二段跳，但跳跃效果减弱)
//攻击（识破)
//技能幻影斩击
//增加能量条
//修改攻击和碰撞逻辑和受击逻辑
//优化奔跑
//优化边功击边移动