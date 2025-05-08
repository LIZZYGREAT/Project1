#include"player.h"
#include"character_manager.h"
#include"player_state_nodes.h"
#include"collision_manager.h"

PlayerAttack1State::PlayerAttack1State() {
	timer.set_wait_time(0.56f);
	timer.set_one_shot(true);
	timer.set_on_timeout([&] {
		Player* player = (Player*)CharacterManager::instance()->get_player();
		player->set_attack1(false);
		player->set_attacking(false);
		player->set_changeDirection(true);
		});

}

void PlayerAttack1State::on_enter() {
	CharacterManager::instance()->get_player()->set_animation("attack1");

	Player* player = (Player*)CharacterManager::instance()->get_player();

	player->set_changeDirection(false);

	CollisionManager::instance()->get_attack1_box()->set_enabled(true);

	player->set_attack1(true);
	player->set_attacking(true);

	CollisionManager::instance()->get_attack1_box()->update_attack1_size();

	CollisionManager::instance()->get_attack1_box()->update_attack1_position(player->get_logic_center(), player->get_player_facing_left());

	player->on_attack1();
	timer.restart();
	CollisionManager::instance()->get_attack1_box()->get_timer_stage1_to_2()->restart();
	CollisionManager::instance()->get_attack1_box()->get_timer_stage2_to_3()->restart();
	CollisionManager::instance()->get_attack1_box()->get_timer_stage3_to_end()->restart();
	switch (range_random(1, 3)) {
	case 1:
		play_audio(_T("player_attack_1"), false);
		break;
	case 2:
		play_audio(_T("player_attack_2"), false);
		break;
	case 3:
		play_audio(_T("player_attack_3"), false);
		break;
	}
}

void PlayerAttack1State::on_update(float delta) {
	timer.on_update(delta);

	Player* player = (Player*)CharacterManager::instance()->get_player();

	sum_delta_time += delta;
	
	t = sum_delta_time / 0.56;

	/*run_speed = 250 / 0.32 * sum_delta_time * (t < 0.58) + 250 * (0.58 <= t && t < 0.79) - (250 / 0.12) * sum_delta_time * (t >= 0.79 & t < 1);

	player->set_velocity({ run_speed,0 });*/

	CollisionManager::instance()->get_attack1_box()->update_attack1_size();
	CollisionManager::instance()->get_attack1_box()->update_attack1_position(player->get_logic_center(), player->get_player_facing_left());
	if (CollisionManager::instance()->get_attack1_box()->get_stage() == CollisionBox::Stage::one)
		CollisionManager::instance()->get_attack1_box()->get_timer_stage1_to_2()->on_update(delta);
	if (CollisionManager::instance()->get_attack1_box()->get_stage() == CollisionBox::Stage::two)
		CollisionManager::instance()->get_attack1_box()->get_timer_stage2_to_3()->on_update(delta);
	if (CollisionManager::instance()->get_attack1_box()->get_stage() == CollisionBox::Stage::three)
		CollisionManager::instance()->get_attack1_box()->get_timer_stage3_to_end()->on_update(delta);


	if (player->get_hp() <= 0) {
		player->switch_state("dead");
	}
	else if (!player->get_attacking()) {
		if (player->get_velocity().y > 0) {
			player->switch_state("fall");
		}
		else if (player->is_on_floor()) {
			if (player->get_move_axis() == 0) {
				player->set_velocity({ 300.0f * player->get_player_facing_left(),0 });
			}
			player->switch_state("run");
		}
	}
	else if (player->can_attack2()) {
		player->switch_state("attack2");
	}
	else if (player->can_attack3()) {
		player->switch_state("attack3");
	}
}
void PlayerAttack1State::on_exit() {
	Player* player = (Player*)CharacterManager::instance()->get_player();
	CollisionManager::instance()->get_attack1_box()->set_enabled(false);
	CollisionManager::instance()->get_attack1_box()->set_stage(CollisionBox::Stage::one);
	player->set_attack1(false);
	player->set_attacking(false);

}



PlayerAttack2State::PlayerAttack2State() {
	timer.set_wait_time(0.56f);
	timer.set_one_shot(true);
	timer.set_on_timeout([&] {
		Player* player = (Player*)CharacterManager::instance()->get_player();
		player->set_attack2(false);
		player->set_attacking(false);
		player->set_changeDirection(true);
		});
}

void PlayerAttack2State::on_enter() {
	CharacterManager::instance()->get_player()->set_animation("attack2");

	Player* player = (Player*)CharacterManager::instance()->get_player();
	player->set_changeDirection(false);

	CollisionManager::instance()->get_see_through_box_attack2()->set_enabled(false);

	CollisionManager::instance()->get_see_through_box_attack2()->update_seeThrough_box_size_attack2();
	CollisionManager::instance()->get_see_through_box_attack2()->update_seeThrough_box_position_attack2(player->get_logic_center(), player->get_player_facing_left());



	CollisionManager::instance()->get_attack2_box()->set_enabled(true);
	CollisionManager::instance()->get_attack2_box()->update_attack2_size();
	CollisionManager::instance()->get_attack2_box()->update_attack2_position(player->get_logic_center(), player->get_player_facing_left());

	player->set_attack2(true);
	player->set_attacking(true);
	player->on_attack2();

	CollisionManager::instance()->get_attack2_box()->get_timer_stage1_to_2()->restart();
	CollisionManager::instance()->get_attack2_box()->get_timer_stage2_to_3()->restart();
	CollisionManager::instance()->get_attack2_box()->get_timer_stage3_to_end()->restart();

	CollisionManager::instance()->get_see_through_box_attack2()->get_timer_seeThrough_invalid()->restart();
	CollisionManager::instance()->get_see_through_box_attack2()->get_timer_seeThrough_valid()->restart();

	timer.restart();

	switch (range_random(1, 3)) {
	case 1:
		play_audio(_T("player_attack_1"), false);
		break;
	case 2:
		play_audio(_T("player_attack_2"), false);
		break;
	case 3:
		play_audio(_T("player_attack_3"), false);
		break;
	}
}

void PlayerAttack2State::on_update(float delta) {
	timer.on_update(delta);

	Player* player = (Player*)CharacterManager::instance()->get_player();
	//普通技能碰撞
	CollisionManager::instance()->get_attack2_box()->update_attack2_size();
	CollisionManager::instance()->get_attack2_box()->update_attack2_position(player->get_logic_center(), player->get_player_facing_left());
	if (CollisionManager::instance()->get_attack2_box()->get_stage() == CollisionBox::Stage::one)
		CollisionManager::instance()->get_attack2_box()->get_timer_stage1_to_2()->on_update(delta);
	if (CollisionManager::instance()->get_attack2_box()->get_stage() == CollisionBox::Stage::two)
		CollisionManager::instance()->get_attack2_box()->get_timer_stage2_to_3()->on_update(delta);
	if (CollisionManager::instance()->get_attack2_box()->get_stage() == CollisionBox::Stage::three)
		CollisionManager::instance()->get_attack2_box()->get_timer_stage3_to_end()->on_update(delta);
	//识破碰撞
	CollisionManager::instance()->get_see_through_box_attack2()->update_seeThrough_box_size_attack2();
	CollisionManager::instance()->get_see_through_box_attack2()->update_seeThrough_box_position_attack2(player->get_logic_center(), player->get_player_facing_left());
	if (CollisionManager::instance()->get_see_through_box_attack2()->get_state() == CollisionBox::State::invalid) {
		CollisionManager::instance()->get_see_through_box_attack2()->get_timer_seeThrough_invalid()->on_update(delta);
	}
	if (CollisionManager::instance()->get_see_through_box_attack2()->get_state() == CollisionBox::State::valid) {
		CollisionManager::instance()->get_see_through_box_attack2()->get_timer_seeThrough_valid()->on_update(delta);
	}

	if (player->get_hp() <= 0) {
		player->switch_state("dead");
	}
	else if (!player->get_attacking()) {
		if (player->get_velocity().y > 0) {
			player->switch_state("fall");
		}
		else if (player->is_on_floor()) {
			if (player->get_move_axis() == 0) {
				player->set_velocity({ 300.0f * player->get_player_facing_left(),0 });
			}
			player->switch_state("run");
		}
	}
	else if (player->can_attack1()) {
		player->switch_state("attack1");
	}
	else if (player->can_attack3()) {
		player->switch_state("attack3");
	}
}
void PlayerAttack2State::on_exit() {
	Player* player = (Player*)CharacterManager::instance()->get_player();

	CollisionManager::instance()->get_attack2_box()->set_enabled(false);
	CollisionManager::instance()->get_attack2_box()->set_stage(CollisionBox::Stage::one);

	player->set_attack2(false);
	player->set_attacking(false);
}

PlayerAttack3State::PlayerAttack3State() {
	timer.set_wait_time(0.64f);
	timer.set_one_shot(true);
	timer.set_on_timeout([&] {
		Player* player = (Player*)CharacterManager::instance()->get_player();
		player->set_attack3(false);
		player->set_attacking(false);
		player->set_changeDirection(true);
		});

}

void PlayerAttack3State::on_enter() {
	CharacterManager::instance()->get_player()->set_animation("attack3");

	Player* player = (Player*)CharacterManager::instance()->get_player();

	player->set_changeDirection(false);
	player->set_attack3(true);
	player->set_attacking(true);

	CollisionManager::instance()->get_attack3_box()->set_enabled(true);
	CollisionManager::instance()->get_attack3_box()->update_attack3_size();
	CollisionManager::instance()->get_attack3_box()->update_attack3_position(player->get_logic_center(), player->get_player_facing_left());

	CollisionManager::instance()->get_see_through_box_attack3()->set_enabled(false);
	CollisionManager::instance()->get_see_through_box_attack3()->update_seeThrough_box_position_attack3
	(player->get_logic_center(), player->get_player_facing_left());
	CollisionManager::instance()->get_see_through_box_attack3()->update_seeThrough_box_size_attack3();


	CollisionManager::instance()->get_attack3_box()->get_timer_stage1_to_2()->restart();
	CollisionManager::instance()->get_attack3_box()->get_timer_stage2_to_3()->restart();
	CollisionManager::instance()->get_attack3_box()->get_timer_stage3_to_end()->restart();

	CollisionManager::instance()->get_see_through_box_attack3()->get_timer_seeThrough_invalid()->restart();
	CollisionManager::instance()->get_see_through_box_attack3()->get_timer_seeThrough_valid()->restart();

	player->on_attack3();
	timer.restart();

	switch (range_random(1, 3)) {
	case 1:
		play_audio(_T("player_attack_1"), false);
		break;
	case 2:
		play_audio(_T("player_attack_2"), false);
		break;
	case 3:
		play_audio(_T("player_attack_3"), false);
		break;
	}
}

void PlayerAttack3State::on_update(float delta) {
	timer.on_update(delta);

	Player* player = (Player*)CharacterManager::instance()->get_player();

	CollisionManager::instance()->get_attack3_box()->update_attack3_size();
	CollisionManager::instance()->get_attack3_box()->update_attack3_position(player->get_logic_center(), player->get_player_facing_left());
	if (CollisionManager::instance()->get_attack3_box()->get_stage() == CollisionBox::Stage::one)
		CollisionManager::instance()->get_attack3_box()->get_timer_stage1_to_2()->on_update(delta);
	if (CollisionManager::instance()->get_attack3_box()->get_stage() == CollisionBox::Stage::two)
		CollisionManager::instance()->get_attack3_box()->get_timer_stage2_to_3()->on_update(delta);
	if (CollisionManager::instance()->get_attack3_box()->get_stage() == CollisionBox::Stage::three)
		CollisionManager::instance()->get_attack3_box()->get_timer_stage3_to_end()->on_update(delta);


	CollisionManager::instance()->get_see_through_box_attack3()->update_seeThrough_box_position_attack3
	(player->get_logic_center(), player->get_player_facing_left());

	CollisionManager::instance()->get_see_through_box_attack3()->update_seeThrough_box_size_attack3();
	if (CollisionManager::instance()->get_see_through_box_attack3()->get_state() == CollisionBox::State::invalid) {
		CollisionManager::instance()->get_see_through_box_attack3()->get_timer_seeThrough_invalid()->on_update(delta);
	}
	if (CollisionManager::instance()->get_see_through_box_attack3()->get_state() == CollisionBox::State::valid) {
		CollisionManager::instance()->get_see_through_box_attack3()->get_timer_seeThrough_valid()->on_update(delta);
	}

	if (player->get_hp() <= 0) {
		player->switch_state("dead");
	}
	else if (!player->get_attacking()) {
		if (player->get_velocity().y > 0) {
			player->switch_state("fall");
		}
		else if (player->is_on_floor()) {
			if (player->get_move_axis() == 0) {
				player->set_velocity({ 300.0f * player->get_player_facing_left(),0 });
			}
			player->switch_state("run");
		}
	}
	else if (player->can_attack1()) {
		player->switch_state("attack1");
	}
	else if (player->can_attack2()) {
		player->switch_state("attack2");
	}
}
void PlayerAttack3State::on_exit() {
	Player* player = (Player*)CharacterManager::instance()->get_player();

	CollisionManager::instance()->get_attack3_box()->set_enabled(false);
	CollisionManager::instance()->get_attack3_box()->set_stage(CollisionBox::Stage::one);

	player->set_attack3(false);
	player->set_attacking(false);
}


PlayerDeadState::PlayerDeadState() {
	timer.set_wait_time(2.0f);
	timer.set_one_shot(true);
	timer.set_on_timeout([&]() {
		MessageBox(GetHWnd(), _T("不对...\n这样不行。"), _T("挑战失败"), MB_OK);
		exit(0);
		});
}

void PlayerDeadState::on_enter() {
	CharacterManager::instance()->get_player()->set_animation("dead");
	play_audio(_T("player_dead"), false);
}

void PlayerDeadState::on_update(float delta) {
	timer.on_update(delta);
}

void PlayerFallState::on_enter() {
	CharacterManager::instance()->get_player()->set_animation("fall");
}

void PlayerFallState::on_update(float delta) {
	Player* player = (Player*)CharacterManager::instance()->get_player();

	if (player->get_hp() <= 0) {
		player->switch_state("dead");
	}
	else if (player->is_on_floor()) {
		player->switch_state("idle");
		player->on_land();

		play_audio(_T("player_land"), false);
	}
	else if (player->can_attack1()) {
		player->switch_state("attack1");
	}
	else if (player->can_attack2()) {
		player->switch_state("attack2");
	}
	else if (player->can_attack3()) {
		player->switch_state("attack3");
	}
}

void PlayerIdleState::on_enter() {
	CharacterManager::instance()->get_player()->set_animation("idle");
}
void PlayerIdleState::on_update(float delta) {
	Player* player = (Player*)CharacterManager::instance()->get_player();

	if (player->get_hp() <= 0) {
		player->switch_state("dead");
	}
	else if (player->can_attack1()) {
		player->switch_state("attack1");
	}
	else if (player->can_attack2()) {
		player->switch_state("attack2");
	}
	else if (player->can_attack3()) {
		player->switch_state("attack3");
	}
	else if (player->get_velocity().y > 0) {
		player->switch_state("fall");
	}
	else if (player->can_jump()) {
		player->switch_state("jump");
	}
	else if (player->is_on_floor() && player->get_move_axis() != 0) {
		player->switch_state("run");
	}
}

void PlayerJumpState::on_enter() {
	CharacterManager::instance()->get_player()->set_animation("jump");

	Player* player = (Player*)CharacterManager::instance()->get_player();
	player->on_jump();

	play_audio(_T("player_jump"), false);
}

void PlayerJumpState::on_update(float delta) {
	Player* player = (Player*)CharacterManager::instance()->get_player();

	if (player->get_hp() <= 0) {
		player->switch_state("dead");
	}
	else if (player->get_velocity().y > 0) {
		player->switch_state("fall");
	}
	else if (player->can_attack1()) {
		player->switch_state("attack1");
	}
	else if (player->can_attack2()) {
		player->switch_state("attack2");
	}
	else if (player->can_attack3()) {
		player->switch_state("attack3");
	}
}


void PlayerRunState::on_enter() {
	CharacterManager::instance()->get_player()->set_animation("run");

	play_audio(_T("player_run"), true);
}

void PlayerRunState::on_update(float delta) {

	Player* player = (Player*)CharacterManager::instance()->get_player();

	if (player->get_hp() <= 0) {
		player->switch_state("dead");
	}
	else if (player->get_move_axis() == 0) {
		player->switch_state("idle");
	}
	else if (player->can_jump()) {
		player->switch_state("jump");
	}
	else if (player->can_attack1()) {
		player->switch_state("attack1");
	}
	else if (player->can_attack2()) {
		player->switch_state("attack2");
	}
	else if (player->can_attack3()) {
		player->switch_state("attack3");
	}
}

void PlayerRunState::on_exit() {
	stop_audio(_T("player_run"));
}

PlayerTakeHitState::PlayerTakeHitState() {
	timer.set_wait_time(0.50f);
	timer.set_one_shot(true);
	timer.set_on_timeout([&] {
		Player* player = (Player*)CharacterManager::instance()->get_player();
		player->set_taking_hit(false);
		});
}

void PlayerTakeHitState::on_enter() {
	CharacterManager::instance()->get_player()->set_animation("take_hit");

	Player* player = (Player*)CharacterManager::instance()->get_player();
	player->get_hurt_box()->set_enabled(false);
	player->set_taking_hit(true);
	player->on_take_hit();
	timer.restart();
}

void PlayerTakeHitState::on_update(float delta) {
	timer.on_update(delta);

	Player* player = (Player*)CharacterManager::instance()->get_player();

	if (!player->get_is_taking_hit()) {
		if (player->get_move_axis() != 0) {
			player->switch_state("run");
		}
		else if (player->can_jump()) {
			player->switch_state("jump");
		}
		else {
			player->switch_state("idle");
		}
	}
}

void PlayerTakeHitState::on_exit() {
	CharacterManager::instance()->get_player()->get_hurt_box()->set_enabled(true);
}