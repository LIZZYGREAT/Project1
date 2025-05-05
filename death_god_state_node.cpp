#include"death_god_state_node.h"



Attack1State::Attack1State() {
	timer1.set_wait_time(1.20f + range_random(1,2)*0.4f);
	timer1.set_one_shot(true);
	timer1.set_on_timeout([&] {
		DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
		timer2.restart();
		death_god->set_is_attack1_anim_end(true);
		death_god->set_is_attack1(true);    //第一段动画前摇和攻击是分离的
		});
	 

	timer2.set_wait_time(3.5f);
	timer2.set_one_shot(true);
	timer2.set_on_timeout([&] {
		DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
		death_god->set_is_attack1(false);
		if(death_god->get_hp()>=75)
		death_god->switch_state("idle");
		else if (death_god->get_hp() >= 50) {
			int x = range_random(0, 100);
			if (x <= 25) {
				death_god->switch_state("attack2");
			}
			else if (x <= 45 ) {
				death_god->switch_state("attack4");
			}
			else if (x <= 60) {
				death_god->switch_state("attack5");
			}
			else if(x<=90) {
				death_god->switch_state("teleport_in");
			}
			else {
				death_god->switch_state("attack3");
			}
		}
		else {
			int x = range_random(0, 100);
			if (x <= 15) {
				death_god->switch_state("attack2");
			}
			else if (x <= 30) {
				death_god->switch_state("attack4");
			}
			else if (x <= 45) {
				death_god->switch_state("attack5");
			}
			else if (x <= 75) {
				death_god->switch_state("teleport_in");
			}
			else {
				death_god->switch_state("attack3");
			}
		}
	});
}

void Attack1State::on_enter() {
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
	death_god->set_animation("image_attack1");
	death_god->on_attack1();
	death_god->set_is_attack1(false);
	death_god->set_is_attack1_anim_end(false);
	death_god->on_attack1();
	timer1.restart();
}

void Attack1State::on_update(float delta) {

	timer1.on_update(delta);
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
	
	death_god->on_update(delta);

	if (death_god->get_is_attack1_anim_end() == true) {
		timer2.on_update(delta);
	}
	if (death_god->get_is_attack1()) {
		death_god->on_attack1();

	}
	if (death_god->get_hp() <= 0)
	{
		death_god->switch_state("dead");
	}

}

void Attack1State::on_exit() {
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
	death_god->set_is_attack1(false);
	death_god->set_is_attack1_anim_end(true);
}


Attack2State::Attack2State() {
	timer1.set_wait_time(1.44f + range_random(1, 2) * 0.3f);
	timer1.set_one_shot(true);
	timer1.set_on_timeout([&] {
		DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
		timer2.restart();
		death_god->set_is_attack4_anim_end(true);
		death_god->set_is_attack4(true);    //第一段动画前摇和攻击是分离的
		});


	timer2.set_wait_time(0.8+range_random(1,3)*0.2f);
	timer2.set_one_shot(false);
	timer2.set_on_timeout([&] {
		DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
		if (death_god->get_hp() >= 75) {
			timer2.set_need_complete_times(2);
		}
		else if (death_god->get_hp() >= 50) {
			timer2.set_need_complete_times(3);
		}
		else {
			timer2.set_need_complete_times(4);
		}
		if (timer2.get_complete_times() == timer2.get_need_complete_times()) {
			death_god->set_is_attack2(false);
		}
		if (death_god->get_is_attack2() == false) {
			if (death_god->get_hp() >= 75)
				death_god->switch_state("idle");
			else if (death_god->get_hp() >= 50) {
				int x = range_random(0, 100);
				if (x <= 25) {
					death_god->switch_state("attack1");
				}
				else if (x <= 45) {
					death_god->switch_state("attack4");
				}
				else if (x <= 60) {
					death_god->switch_state("attack5");
				}
				else if (x <= 95) {
					death_god->switch_state("teleport_in");
				}
				else {
					death_god->switch_state("attack3");
				}
			}
			else {
				int x = range_random(0, 100);
				if (x <= 20) {
					death_god->switch_state("attack1");
				}
				else if (x <= 40) {
					death_god->switch_state("attack4");
				}
				else if (x <= 60) {
					death_god->switch_state("attack5");
				}
				else if (x <= 80) {
					death_god->switch_state("teleport_in");
				}
				else {
					death_god->switch_state("attack3");
				}
			}
		}
		});
}

void Attack2State::on_enter() {
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
	death_god->set_animation("image_attack2");
	death_god->on_attack2();
	death_god->set_is_attack2(false);
	death_god->set_is_attack4_anim_end(false);
	death_god->on_attack2();
	timer1.restart();
}

void Attack2State::on_update(float delta) {

	timer1.on_update(delta);
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();

	death_god->on_update(delta);

	if (death_god->get_is_attack2_anim_end() == true) {
		timer2.on_update(delta);
	}
	if (death_god->get_is_attack2()) {
		death_god->on_attack2();

	}

	if (death_god->get_hp() <= 0)
	{
		death_god->switch_state("dead");
	}

}

void Attack2State::on_exit() {
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
	death_god->set_is_attack2(false);
	death_god->set_is_attack2_anim_end(true);
}

Attack3State::Attack3State() {
	timer1.set_wait_time(2.8f + range_random(1, 2) * 0.2f);
	timer1.set_one_shot(true);
	timer1.set_on_timeout([&] {
		DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
		death_god->set_is_attack3_anim_end(true);
		death_god->set_is_attack3(true);    //第一段动画前摇和攻击是分离的
		});
}

void Attack3State::on_enter() {
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
	death_god->set_animation("image_attack3");
	death_god->set_gravity_enabled(false);
	death_god->on_attack3();
	death_god->set_is_attack3(false);
	death_god->set_is_attack3_anim_end(false);
	timer1.restart();
}

void Attack3State::on_update(float delta) {
	timer1.on_update(delta);
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
	death_god->on_update(delta);
	if (death_god->get_hp() <= 0)
	{
		death_god->switch_state("dead");
	}
	else if(death_god->get_is_attack3_anim_end() == true) {
		death_god->switch_state("idle");
	}

}

void Attack3State::on_exit() {
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
	death_god->set_gravity_enabled(true);
	death_god->set_is_attack3(false);
	death_god->set_is_attack3_anim_end(true);
}


Attack4State::Attack4State() {
	timer1.set_wait_time(1.68f + range_random(1, 2) * 0.3f);
	timer1.set_one_shot(true);
	timer1.set_on_timeout([&] {
		DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
		timer2.restart();
		death_god->set_is_attack4_anim_end(true);
		death_god->set_is_attack4(true);    //第一段动画前摇和攻击是分离的
		});


	timer2.set_wait_time(0.72 + range_random(1, 3) * 0.2f);
	timer2.set_one_shot(false);
	timer2.set_on_timeout([&] {
		DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
		if (death_god->get_hp() >= 75) {
			timer2.set_need_complete_times(1);
		}
		else if (death_god->get_hp() >= 25) {
			timer2.set_need_complete_times(2);
		}
		else {
			timer2.set_need_complete_times(3);
		}
		if (timer2.get_complete_times() == timer2.get_need_complete_times()) {
			death_god->set_is_attack4(false);
		}
		if (death_god->get_is_attack4() == false) {
			if (death_god->get_hp() >= 75)
				death_god->switch_state("idle");
			else if (death_god->get_hp() >= 50) {
				int x = range_random(0, 100);
				if (x <= 25) {
					death_god->switch_state("attack1");
				}
				else if (x <= 45) {
					death_god->switch_state("attack2");
				}
				else if (x <= 60) {
					death_god->switch_state("attack5");
				}
				else if (x <= 95) {
					death_god->switch_state("teleport_in");
				}
				else {
					death_god->switch_state("attack3");
				}
			}
			else {
				int x = range_random(0, 100);
				if (x <= 20) {
					death_god->switch_state("attack1");
				}
				else if (x <= 40) {
					death_god->switch_state("attack2");
				}
				else if (x <= 60) {
					death_god->switch_state("attack5");
				}
				else if (x <= 80) {
					death_god->switch_state("teleport_in");
				}
				else {
					death_god->switch_state("attack3");
				}
			}
		}
		});
}

void Attack4State::on_enter() {
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
	death_god->set_animation("image_attack4");
	death_god->on_attack4();
	death_god->set_is_attack4(false);
	death_god->set_is_attack4_anim_end(false);
	death_god->on_attack4();
	timer1.restart();
}

void Attack4State::on_update(float delta) {

	timer1.on_update(delta);
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();

	death_god->on_update(delta);

	if (death_god->get_is_attack4_anim_end() == true) {
		timer2.on_update(delta);
	}
	if (death_god->get_is_attack4()) {
		death_god->on_attack2();

	}

	if (death_god->get_hp() <= 0)
	{
		death_god->switch_state("dead");
	}

}

void Attack4State::on_exit() {
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
	death_god->set_is_attack4(false);
	death_god->set_is_attack4_anim_end(true);
}


Attack5State::Attack5State() {
	timer1.set_wait_time(1.68f);
	timer1.set_one_shot(true);
	timer1.set_on_timeout([&] {
		DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
		timer2.restart();
		death_god->set_is_attack5_anim_end(true);
		death_god->set_is_attack5(true);    //第一段动画前摇和攻击是分离的
		});


	timer2.set_wait_time(0.8 + range_random(1, 3) * 0.2f);
	timer2.set_one_shot(false);
	timer2.set_on_timeout([&] {
		DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
		if (death_god->get_hp() >= 75) {
			timer2.set_need_complete_times(1);
		}
		else if (death_god->get_hp() >= 40) {
			timer2.set_need_complete_times(2);
		}
		else {
			timer2.set_need_complete_times(2);
		}
		if (timer2.get_complete_times() == timer2.get_need_complete_times()) {
			death_god->set_is_attack2(false);
		}
		if (death_god->get_is_attack2() == false) {
			if (death_god->get_hp() >= 75)
				death_god->switch_state("idle");
			else if (death_god->get_hp() >= 50) {
				int x = range_random(0, 100);
				if (x <= 25) {
					death_god->switch_state("attack1");
				}
				else if (x <= 45) {
					death_god->switch_state("attack2");
				}
				else if (x <= 60) {
					death_god->switch_state("attack4");
				}
				else if (x <= 95) {
					death_god->switch_state("teleport_in");
				}
				else {
					death_god->switch_state("attack3");
				}
			}
			else {
				int x = range_random(0, 100);
				if (x <= 20) {
					death_god->switch_state("attack1");
				}
				else if (x <= 40) {
					death_god->switch_state("attack2");
				}
				else if (x <= 60) {
					death_god->switch_state("attack4");
				}
				else if (x <= 80) {
					death_god->switch_state("teleport_in");
				}
				else {
					death_god->switch_state("attack3");
				}
			}
		}
		});
}

void Attack5State::on_enter() {
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
	death_god->set_animation("image_attack4");
	death_god->on_attack5();
	death_god->set_is_attack4(false);
	death_god->set_is_attack4_anim_end(false);
	death_god->on_attack4();
	timer1.restart();
}

void Attack5State::on_update(float delta) {

	timer1.on_update(delta);
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();

	death_god->on_update(delta);

	if (death_god->get_is_attack4_anim_end() == true) {
		timer2.on_update(delta);
	}
	if (death_god->get_is_attack4()) {
		death_god->on_attack4();

	}

	if (death_god->get_hp() <= 0)
	{
		death_god->switch_state("dead");
	}

}

void Attack5State::on_exit() {
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
	death_god->set_is_attack2(false);
	death_god->set_is_attack2_anim_end(true);
}


TeleportInState::TeleportInState() {
	timer.set_wait_time(1.2f+range_random(2,4)*0.5f);
	timer.set_one_shot(true);
	timer.set_on_timeout([&] {
		DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
		death_god->set_is_teleport_in(false);
		death_god->get_hurt_box()->set_enabled(true);
		death_god->switch_state("teleport_out");
		});

}

void TeleportInState::on_enter() {
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
	death_god->on_teleport_in();
	death_god->get_hurt_box()->set_enabled(false);
	death_god->set_animation("image_teleport_in");
	death_god->set_is_teleport_in(true);
	death_god->on_teleport_in();
	timer.restart();
}

void TeleportInState::on_update(float delta) {

	timer.on_update(delta);
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
	death_god->on_update(delta);
}

void TeleportInState::on_exit() {
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
	death_god->set_is_teleport_in(false);
}


TeleportOutState::TeleportOutState() {
	timer.set_wait_time(1.2f);
	timer.set_one_shot(true);
	timer.set_on_timeout([&] {
		DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
		int complete_time = 1;
		if (death_god->get_hp() >= 75)
			complete_time = 2;
		else if (death_god->get_hp() >= 55) {
			complete_time =3;
		}
		else {
			complete_time = range_random(2, 4);
		}
		for (int i = 0; i < complete_time; i++)
		{
			if (death_god->get_hp() >= 75) {
				int x = range_random(0, 100);
				if (x <= 35)
					death_god->on_attack1();
				else if (x <= 65) {
					death_god->on_attack4();
				}
				else {
					death_god->on_attack5();
				}
			}
			else if (death_god->get_hp() >= 55) {
				int x = range_random(0, 100);
				if (x <= 25)
					death_god->on_attack1();
				else if (x <= 65) {
					death_god->on_attack4();
				}
				else {
					death_god->on_attack5();
				}
			}
			else {
				int x = range_random(0, 100);
				if (x <= 45)
					death_god->on_attack1();
				else if (x <= 55) {
					death_god->on_attack2();
				}
				else {
					death_god->set_is_teleport_out(false);
					death_god->switch_state("attack3");
				}
			}
			if (i == complete_time - 1) {
				death_god->set_is_teleport_out(false);
				death_god->switch_state("fall");
			}
		}
	});
}

void TeleportOutState::on_enter() {
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
	death_god->set_animation("image_teleport_out");
	death_god->on_teleport_out();
	death_god->set_gravity_enabled(false);
	death_god->set_is_teleport_out(true);
	death_god->on_teleport_out();
	timer.restart();
}

void TeleportOutState::on_update(float delta) {

	timer.on_update(delta);
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
	death_god->on_update(delta);

	if (death_god->get_hp() <= 0) {
		death_god->switch_state("dead");
	}
}

void TeleportOutState::on_exit() {
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
	death_god->set_gravity_enabled(true);
	death_god->set_is_teleport_out(false);
	death_god->set_is_attack1(false);
	death_god->set_is_attack2(false);
	death_god->set_is_attack3(false);
	death_god->set_is_attack4(false);
	death_god->set_is_attack5(false);
}


DeadState::DeadState() {
	timer.set_wait_time(3.30f);
	timer.set_one_shot(true);
	timer.set_on_timeout([&]() {
		MessageBox(GetHWnd(), _T("很好，这样能行。"), _T("挑战成功"), MB_OK);
		exit(0);
		});
}

void DeadState::on_enter() {
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
	death_god->set_animation("dead");
	timer.restart();
}

void DeadState::on_update(float delta) {
	timer.on_update(delta);
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
	death_god->on_update(delta);
}

void IdleState::on_enter() {
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
	death_god->set_animation("idle");
}

void IdleState::on_update(float delta) {

	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
	death_god->on_update(delta);

	if (death_god->get_hp() <= 0) {
		death_god->switch_state("dead");
	}
	else if (death_god->get_hp() >= 75) {
		int x = range_random(0, 100);
		if (x <= 20) {
			death_god->switch_state("attack1");
		}
		else if (x <= 45) {
			death_god->switch_state("attack4");
		}
		else if (x <= 60) {
			death_god->switch_state("attack5");
		}
		else if (x <= 80) {
			death_god->switch_state("run");
		}
		else {
			death_god->switch_state("teleport_in");
		}
	}
	else if (death_god->get_hp() >= 40) {
		int x = range_random(0, 100);
		if (x <= 15) {
			death_god->switch_state("attack1");
		}
		else if (x <= 25) {
			death_god->switch_state("attack4");
		}
		else if (x <= 35) {
			death_god->switch_state("attack5");
		}
		else if (x <= 45) {
			death_god->switch_state("attack2");
		}
		else if (x <= 65) {
			death_god->switch_state("attack3");
		}
		else if (x <= 80) {
			death_god->switch_state("run");
		}
		else {
			death_god->switch_state("teleport_in");
		}
	}
	else {
		int x = range_random(0, 100);
		if (x <= 10) {
			death_god->switch_state("attack1");
		}
		else if (x <= 20) {
			death_god->switch_state("attack4");
		}
		else if (x <= 30) {
			death_god->switch_state("attack5");
		}
		else if (x <= 40) {
			death_god->switch_state("attack2");
		}
		else if (x <= 60) {
			death_god->switch_state("attack3");
		}
		else if (x <= 75) {
			death_god->switch_state("run");
		}
		else {
			death_god->switch_state("teleport_in");
		}
	}
}

void FallState::on_enter() {
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
	death_god->set_gravity_enabled(true);
	death_god->set_animation("idle");
}

void FallState::on_update(float delta) {
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
	death_god->on_update(delta);
	if (death_god->get_hp() <= 0) {
		death_god->switch_state("dead");
	}
	else if(death_god->is_on_floor()) {
		death_god->switch_state("idle");
	}
};

void RunState::on_enter() {
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();
	death_god->set_gravity_enabled(true);
	death_god->set_animation("idle");
	death_god->on_run();
}

void RunState::on_update(float delta) {

	Player* player = (Player*)CharacterManager::instance()->get_player();
	DeathGod* death_god = (DeathGod*)CharacterManager::instance()->get_death_god();

	death_god->on_update(delta);

	if (player->get_hp() <= 0) {
		death_god->switch_state("dead");
	}
	else if (!death_god->get_is_run()) {
		death_god->switch_state("idle");
	}

}

