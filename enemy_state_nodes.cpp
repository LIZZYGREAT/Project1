#include"enemy.h"
#include"character_manager.h"
#include"enemy_state_nodes.h"
#include"collision_manager.h"

EnemyAttack1State::EnemyAttack1State() {
	timer.set_wait_time(0.60f);
	timer.set_one_shot(true);
	timer.set_on_timeout([&] {
		Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();
		enemy->set_attack1(false);
		enemy->set_attacking(false);
		enemy->set_changeDirection(true);
		});
}

void EnemyAttack1State::on_enter() {
	CharacterManager::instance()->get_player()->set_animation("enemy_attack1");

	Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();

	enemy->set_changeDirection(false);

	std::cout << "enemy 进入 atttack1" << std::endl;


	enemy->set_attack1(true);
	enemy->set_attacking(true);
	enemy->on_attack1();
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

void EnemyAttack1State::on_update(float delta) {
	timer.on_update(delta);

	Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();



	if (enemy->get_hp() <= 0) {
		enemy->switch_state("enemy_dead");
	}
	else if (!enemy->get_attacking()) {
		if (enemy->get_velocity().y > 0) {
			enemy->switch_state("fall");
		}
		else if (enemy->is_on_floor()) {
			enemy->switch_state("idle");
		}
	}

}
void EnemyAttack1State::on_exit() {
	Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();
	

	enemy->set_attack1(false);
	enemy->set_attacking(false);
}

EnemyAttack2State::EnemyAttack2State() {
	timer.set_wait_time(0.60f);
	timer.set_one_shot(true);
	timer.set_on_timeout([&] {
		Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();
		enemy->set_attack2(false);
		enemy->set_attacking(false);
		enemy->set_changeDirection(true);
		});

}

void EnemyAttack2State::on_enter() {
	CharacterManager::instance()->get_player()->set_animation("enemy_attack2");

	Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();

	enemy->set_changeDirection(false);

	std::cout << "enemy 进入 atttack2" << std::endl;

	enemy->set_attack2(true);
	enemy->set_attacking(true);
	enemy->on_attack2();
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

void EnemyAttack2State::on_update(float delta) {
	timer.on_update(delta);

	Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();


	if (enemy->get_hp() <= 0) {
		enemy->switch_state("enemy_dead");
	}
	else if (!enemy->get_attacking()) {
		if (enemy->get_velocity().y > 0) {
			enemy->switch_state("fall");
		}
		else if (enemy->is_on_floor()) {
			enemy->switch_state("idle");
		}
	}

}
void EnemyAttack2State::on_exit() {
	Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();

	enemy->set_attack2(false);
	enemy->set_attacking(false);
}


EnemyAttack3State::EnemyAttack3State() {
	timer1.set_wait_time(0.80f);
	timer1.set_one_shot(true);
	timer1.set_on_timeout([&] {
		Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();
		enemy->set_attack4(false);
		enemy->set_attacking(false);
		enemy->set_changeDirection(true);
		enemy->set_attack3_vfx_visible(true);
		CollisionBox* vfx_box = CollisionManager::instance()->get_enemy_attack3_vfx_box();
		vfx_box->update_enemy_attack3_vfx_box_position(enemy->get_logic_center(), enemy->get_is_facing_left());
		vfx_box->update_enemy_attack3_vfx_box_size();

		});

}

void EnemyAttack3State::on_enter() {
	CharacterManager::instance()->get_player()->set_animation("enemy_attack3");

	Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();
	CollisionBox* box = CollisionManager::instance()->get_enemy_attack3_box();
	CollisionBox* vfx_box = CollisionManager::instance()->get_enemy_attack3_vfx_box();

	enemy->set_changeDirection(false);
	enemy->set_attack3(true);
	enemy->set_attacking(true);
	enemy->on_attack3();
		
	std::cout << "enemy 进入 atttack3" << std::endl;

	timer1.restart();
	vfx_box->get_timer_stage1_to_2()->restart();
	box->get_timer_stage1_to_2()->restart();
	box->get_timer_stage2_to_3()->restart();


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

void EnemyAttack3State::on_update(float delta) {
	timer1.on_update(delta);

	Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();
	CollisionBox* box = CollisionManager::instance()->get_enemy_attack3_box();
	CollisionBox* vfx_box = CollisionManager::instance()->get_enemy_attack3_vfx_box();

	box->update_enemy_attack3_box_size();
	box->update_enemy_attack3_box_position(enemy->get_logic_center(),enemy->get_is_facing_left());

	box->get_timer_stage1_to_2()->on_update(delta);

	if(box->get_stage() == CollisionBox::Stage::two)
	box->get_timer_stage2_to_3()->on_update(delta);

	if(enemy->get_attack3_vfx_visible())
	vfx_box->get_timer_stage1_to_2()->on_update(delta);

	if (enemy->get_hp() <= 0) {
		enemy->switch_state("enemy_dead");
	}
	else if (!enemy->get_attacking()) {
		if (enemy->get_velocity().y > 0) {
			enemy->switch_state("fall");
		}
		else if (enemy->is_on_floor()) {
			enemy->switch_state("idle");
		}
	}
}
void EnemyAttack3State::on_exit() {
	Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();

	enemy->set_attack3(false);
	enemy->set_attacking(false);
}

EnemyAttack4State::EnemyAttack4State() {
	timer1.set_wait_time(0.80f);
	timer1.set_one_shot(true);
	timer1.set_on_timeout([&] {
		Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();
		enemy->set_attack4(false);
		enemy->set_attacking(false);
		enemy->set_changeDirection(true);
		enemy->set_attack4_vfx_visible(true);

		CollisionBox* vfx_box = CollisionManager::instance()->get_enemy_attack4_vfx_box();
		vfx_box->update_enemy_attack4_vfx_box_position(enemy->get_logic_center(), enemy->get_is_facing_left());
		vfx_box->update_enemy_attack4_vfx_box_size();
	});

}

void EnemyAttack4State::on_enter() {
	CharacterManager::instance()->get_player()->set_animation("enemy_attack4");

	Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();
	CollisionBox* box = CollisionManager::instance()->get_enemy_attack4_box();
	CollisionBox* vfx_box = CollisionManager::instance()->get_enemy_attack4_vfx_box();

	enemy->set_changeDirection(false);
	enemy->set_attack4(true);
	enemy->set_attacking(true);
	enemy->on_attack4();

	std::cout << "enemy 进入 atttack4" << std::endl;


	timer1.restart();
	vfx_box->get_timer_stage1_to_2()->restart();
	box->get_timer_stage1_to_2()->restart();
	box->get_timer_stage2_to_3()->restart();


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

void EnemyAttack4State::on_update(float delta) {
	timer1.on_update(delta);

	Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();
	CollisionBox* box = CollisionManager::instance()->get_enemy_attack4_box();
	CollisionBox* vfx_box = CollisionManager::instance()->get_enemy_attack4_vfx_box();

	box->update_enemy_attack4_box_size();
	box->update_enemy_attack4_box_position(enemy->get_logic_center(), enemy->get_is_facing_left());
	box->get_timer_stage1_to_2()->on_update(delta);


	if (box->get_stage() == CollisionBox::Stage::two)
		box->get_timer_stage2_to_3()->on_update(delta);

	if (enemy->get_attack4_vfx_visible())
		vfx_box->get_timer_stage1_to_2()->on_update(delta);


	if (enemy->get_hp() <= 0) {
		enemy->switch_state("enemy_dead");
	}
	else if (!enemy->get_attacking()) {
		if (enemy->get_velocity().y > 0) {
			enemy->switch_state("fall");
		}
		else if (enemy->is_on_floor()) {
			enemy->switch_state("idle");
		}
	}
}
void EnemyAttack4State::on_exit() {
	Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();
	enemy->set_attack4(false);
	enemy->set_attacking(false);
}



EnemyDeadState::EnemyDeadState() {
	timer.set_wait_time(1.5f);
	timer.set_one_shot(true);
	timer.set_on_timeout([&]() {
		MessageBox(GetHWnd(), _T("很好，你已战胜Martial_Hero。"), _T("挑战成功"), MB_OK);
		exit(0);
		});
}

void EnemyDeadState::on_enter() {
	CharacterManager::instance()->get_player()->set_animation("enemy_dead");

	std::cout << "enemy 进入 dead" << std::endl;

}

void EnemyDeadState::on_update(float delta) {
	timer.on_update(delta);
}



void EnemyFallState::on_enter() {
	CharacterManager::instance()->get_player()->set_animation("enemy_fall");

	std::cout << "enemy 进入 fall" << std::endl;

}

void EnemyFallState::on_update(float delta) {
	Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();

	if (enemy->get_hp() <= 0) {
		enemy->switch_state("enemy_dead");
	}
	else if (enemy->is_on_floor()) {
		enemy->switch_state("enemy_idle");
		enemy->on_land();

		play_audio(_T("player_land"), false);
	}

}


EnemyIdleState::EnemyIdleState() {
	timer.set_one_shot(true);
	timer.set_on_timeout([&]() {
		Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();
		int x = range_random(0, 100);
		if (enemy->get_hp() >= 75) {
			if (x <= 30 && enemy->can_attack1()) {
				enemy->switch_state("enemy_attack1");
			}
			else if (x <= 60 && enemy->can_attack2()) {
				enemy->switch_state("enemy_attack2");
			}
			else if (x <= 70 && enemy->can_attack3()) {
				enemy->switch_state("enemy_attack3");
			}
			else if (x <= 80 && enemy->can_attack4()) {
				enemy->switch_state("enemy_attack4");
			}
			else if (x <= 90 && enemy->can_jump()) {
				enemy->switch_state("enemy_jump");
			}
			else {
				enemy->switch_state("run");
			}
		}
		else if (enemy->get_hp() >= 35) {
			if (x <= 25 && enemy->can_attack1()) {
				enemy->switch_state("enemy_attack1");
			}
			else if (x <= 50 && enemy->can_attack2()) {
				enemy->switch_state("enemy_attack2");
			}
			else if (x <= 65 && enemy->can_attack3()) {
				enemy->switch_state("enemy_attack3");
			}
			else if (x <= 80 && enemy->can_attack4()) {
				enemy->switch_state("enemy_attack4");
			}
			else if (x <= 90 && enemy->can_jump()) {
				enemy->switch_state("enemy_jump");
			}
			else {
				enemy->switch_state("run");
			}
		}
		else {
			if (x <= 10 && enemy->can_attack1()) {
				enemy->switch_state("enemy_attack1");
			}
			else if (x <= 20 && enemy->can_attack2()) {
				enemy->switch_state("enemy_attack2");
			}
			else if (x <= 45 && enemy->can_attack3()) {
				enemy->switch_state("enemy_attack3");
			}
			else if (x <= 70 && enemy->can_attack4()) {
				enemy->switch_state("enemy_attack4");
			}
			else if (x <= 90 && enemy->can_jump()) {
				enemy->switch_state("enemy_jump");
			}
			else {
				enemy->switch_state("run");
			}
		}
	});
}
void EnemyIdleState::on_enter() {
	CharacterManager::instance()->get_enemy()->set_animation("enemy_idle");
	Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();


	std::cout << "enemy 进入 idle" << std::endl;

	float wait_time = 0;
	if (enemy->get_hp() >= 75) {
		wait_time = range_random(1, 2) * 0.35f;
	}
	else if(enemy->get_hp()>=50){
		wait_time = range_random(1, 2) * 0.25f;
	}
	else {
		wait_time = range_random(0, 2) * 0.20f;
	}
	timer.set_wait_time(wait_time);
	timer.restart();
}

void EnemyIdleState::on_update(float delta) {
	Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();

	timer.on_update(delta);

	if (enemy->get_hp() <= 0) {
		enemy->switch_state("enemy_dead");
	}
	if (enemy->get_velocity().y > 0) {
		enemy->switch_state("enemy_fall");
	}
}

void EnemyJumpState::on_enter() {
	CharacterManager::instance()->get_enemy()->set_animation("enemy_jump");

	Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();
	enemy->on_jump();

	std::cout << "enemy 进入 jump" << std::endl;

	play_audio(_T("player_jump"), false);
}

void EnemyJumpState::on_update(float delta) {
	Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();

	if (enemy->get_hp() <= 0) {
		enemy->switch_state("enemy_dead");
	}
	else if (enemy->get_velocity().y > 0) {
		enemy->switch_state("enemy_fall");
	}
}


void EnemyRunState::on_enter() {
	CharacterManager::instance()->get_enemy()->set_animation("enemy_run");
	Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();

	play_audio(_T("player_run"), true);

	std::cout << "enemy 进入 run" << std::endl;

}

void EnemyRunState::on_update(float delta) {

	Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();

	const vector2& pos_enemy = enemy->get_position();
	const vector2& pos_player = CharacterManager::instance()->get_player()->get_position();
	enemy->set_velocity({ pos_enemy.x < pos_player.x ? speed_run : -speed_run,0 });

	if (enemy->get_hp() <= 0) {
		enemy->switch_state("enemy_dead");
	}
	else if (abs(pos_enemy.x - pos_player.x) <= min_dis) {
		int rand_num = range_random(0, 100);
		if (enemy->get_hp() > 75) {
			if (rand_num <= 75) {
				enemy->switch_state("enemy_attack1");
			}
			else
				enemy->switch_state("enemy_attack2");
		}
		else {
			if (rand_num <= 25) {
				enemy->switch_state("enemy_attack1");
			}
			else if(rand_num<=50){
				enemy->switch_state("enemy_attack2");
			}
			else if (rand_num <= 75) {
				enemy->switch_state("enemy_attack3");
			}
			else  {
				enemy->switch_state("enemy_attack4");
			}

		}
		stop_audio(_T("enemy_run"));
	}
}

void EnemyRunState::on_exit() {
	Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();
	enemy->set_velocity({ 0,0 });
}



EnemyTakeHitState::EnemyTakeHitState() {
	timer.set_wait_time(0.50f);
	timer.set_one_shot(true);
	timer.set_on_timeout([&] {
		Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();
		enemy->set_taking_hit(false);
		});
}

void EnemyTakeHitState::on_enter() {
	CharacterManager::instance()->get_player()->set_animation("enemy_take_hit");
	Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();
	enemy->get_hurt_box()->set_enabled(false);
	enemy->set_taking_hit(true);
	enemy->on_take_hit();
	timer.restart();

	std::cout << "enemy 进入 take_hit" << std::endl;

}

void EnemyTakeHitState::on_update(float delta) {
	timer.on_update(delta);

	Enemy* enemy = (Enemy*)CharacterManager::instance()->get_enemy();

	if (!enemy->get_is_taking_hit()) {
		enemy->switch_state("enemy_idle");
	}
}

void EnemyTakeHitState::on_exit() {
	CharacterManager::instance()->get_player()->get_hurt_box()->set_enabled(true);
}