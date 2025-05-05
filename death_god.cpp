#include"death_god.h"
#include"collision_manager.h"
#include"resources_manager.h"
#include"character_manager.h"
#include"death_god_state_node.h"

DeathGod::DeathGod() {

	is_facing_left = true;
	position = { 880,500 };
	logic_height = 260;

	hurt_box->set_size({230,230});

	hurt_box->set_layer_dst(CollisionLayer::None);
	hurt_box->set_layer_src(CollisionLayer::Enemy);

	hurt_box->set_on_collide([&]() {

	});
	
	{

		image_attack1->set_interval(0.07f);//0.07 * 17
		image_attack1->set_loop(false);
		image_attack1->set_anchor_mode(Animation::AnchorMode::Centered);
		image_attack1->add_frame(ResourcesManager::instance()->find_image("enemy_attack1"),17);

		image_attack2->set_interval(0.06f); //0.06 * 24
		image_attack2->set_loop(false);
		image_attack2->set_anchor_mode(Animation::AnchorMode::Centered);
		image_attack2->add_frame(ResourcesManager::instance()->find_image("enemy_attack2"),24);


		image_attack3->set_interval(0.04f); // 0.04 * 70
		image_attack3->set_loop(false);
		image_attack3->set_anchor_mode(Animation::AnchorMode::Centered);
		image_attack3->add_frame(ResourcesManager::instance()->find_image("enemy_attack3"),70);


		image_attack4->set_interval(0.04f);//0.04 *42
		image_attack4->set_loop(false);
		image_attack4->set_anchor_mode(Animation::AnchorMode::Centered);
		image_attack4->add_frame(ResourcesManager::instance()->find_image("enemy_attack4"),42);

		image_attack5->set_interval(0.04f);//0.04 * 42
		image_attack5->set_loop(false);
		image_attack5->set_anchor_mode(Animation::AnchorMode::Centered);
		image_attack5->add_frame(ResourcesManager::instance()->find_image("enemy_attack5"),42);

		image_teleport_out->set_interval(0.04f);//0.04 * 30
		image_teleport_out->set_loop(false);
		image_teleport_out->set_anchor_mode(Animation::AnchorMode::Centered);
		image_teleport_out->add_frame(ResourcesManager::instance()->find_image("enemy_teleport_out"), 30);

		image_teleport_in->set_interval(0.04f);//0.04 * 30
		image_teleport_in->set_loop(false);
		image_teleport_in->set_anchor_mode(Animation::AnchorMode::Centered);
		image_teleport_in->add_frame(ResourcesManager::instance()->find_image("enemy_teleport_in"), 30);

		image_dead->set_interval(0.05f);//0.05 * 65
		image_dead->set_loop(false);
		image_dead->set_anchor_mode(Animation::AnchorMode::Centered);
		image_dead->add_frame(ResourcesManager::instance()->find_image("enemy_dead"), 65);

		image_idle->set_interval(0.07f);//0.07 * 15
		image_idle->set_loop(true);
		image_idle->set_anchor_mode(Animation::AnchorMode::Centered);
		image_idle->add_frame(ResourcesManager::instance()->find_image("enemy_idle"), 15);


		image_move->set_interval(0.07f);//0.07 * 15
		image_move->set_loop(true);
		image_move->set_anchor_mode(Animation::AnchorMode::Centered);
		image_move->add_frame(ResourcesManager::instance()->find_image("enemy_idle"), 15);

	}

	{

		//×´Ì¬»ú´ý¶¨
		state_machine.register_state("attack1", new Attack1State());
		state_machine.register_state("attack2", new Attack2State());
		state_machine.register_state("attack3", new Attack3State());
		state_machine.register_state("attack4", new Attack4State());
		state_machine.register_state("attack5", new Attack5State());
		state_machine.register_state("dead", new DeadState());
		state_machine.register_state("fall", new FallState());
		state_machine.register_state("idle", new IdleState());
		state_machine.register_state("run", new RunState());
		state_machine.register_state("teleport_out", new TeleportOutState());
		state_machine.register_state("teleport_in", new TeleportInState());


		state_machine.set_entry("idle");
	}
}

DeathGod::~DeathGod() = default;


void DeathGod::on_update(float delta) {

	is_facing_left = CharacterManager::instance()->get_player()->get_position().x < position.x;

	Character::on_update(delta);
	hurt_box->set_position(get_logic_center());

	if (position.y <= 300) {
		velocity.y = 0;
	}

	if (is_attack1_anim_end == false) {
		image_attack1->set_position(get_logic_center());
		image_attack1->on_update(delta);
	}

	if (is_attack2_anim_end == false) {
		image_attack2->set_position(get_logic_center());
		image_attack2->on_update(delta);
	}

	if (is_attack3_anim_end == false) {
		image_attack3->set_position(get_logic_center());
		image_attack3->on_update(delta);
	}

	if (is_attack4_anim_end == false) {
		image_attack4->set_position(get_logic_center());
		image_attack4->on_update(delta);
	}

	if (is_attack5_anim_end == false) {
		image_attack5->set_position(get_logic_center());
		image_attack5->on_update(delta);
	}

	if (is_teleport_out == true) {
		image_teleport_out->set_position(get_logic_center());
		image_teleport_out->on_update(delta);
	}

	if (is_teleport_in == true) {
		image_teleport_in->set_position(get_logic_center());
		image_teleport_in->on_update(delta);
	}

	if (is_attack1) {
		for (EnemyAttack1* attack : enemy_attack1_list) {
			attack->update(delta);
		}
	}
	if (is_attack2) {
		for (EnemyAttack2* attack : enemy_attack2_list) {
			attack->update(delta);
		}
	}
	if (is_attack4) {
		for (EnemyAttack4* attack : enemy_attack4_list) {
			attack->update(delta, is_facing_left);
		}
	}
	if (is_attack5) {
		for (EnemyAttack5* attack : enemy_attack5_list) {
			attack->update(delta, is_facing_left);
		}
	}
	if (is_teleport_out) {
		for (EnemyTeleportOut* attack : enemy_teleport_out_list) {
			attack->update(delta);
		}
	}



	enemy_attack1_list.erase(std::remove_if(enemy_attack1_list.begin(), enemy_attack1_list.end(),
		[](EnemyAttack1* attack) {
			bool can_remove =!attack->check_valid_stage1()&&attack->check_valid_stage2()
		&& attack->check_valid_stage3(); 
			if (can_remove) {
				delete attack;
			}
			return can_remove;
		}), enemy_attack1_list.end());

	enemy_attack2_list.erase(std::remove_if(enemy_attack2_list.begin(), enemy_attack2_list.end(),
		[](EnemyAttack2* attack) {
			bool can_remove = !attack->check_valid_stage1() && attack->check_valid_stage2();
			if (can_remove) {
				delete attack;
			}
			return can_remove;
		}), enemy_attack2_list.end());
	enemy_attack4_list.erase(std::remove_if(enemy_attack4_list.begin(), enemy_attack4_list.end(),
		[](EnemyAttack4* attack) {
			bool can_remove = !attack->check_valid();
			if (can_remove) {
				delete attack;
			}
			return can_remove;
		}), enemy_attack4_list.end());

	enemy_attack5_list.erase(std::remove_if(enemy_attack5_list.begin(), enemy_attack5_list.end(),
		[](EnemyAttack5* attack) {
			bool can_remove = !attack->check_valid();
			if (can_remove) {
				delete attack;
			}
			return can_remove;
		}), enemy_attack5_list.end());

	enemy_teleport_out_list.erase(std::remove_if(enemy_teleport_out_list.begin(), enemy_teleport_out_list.end(),
		[](EnemyTeleportOut* attack) {
			bool can_remove = !attack->check_valid();
			if (can_remove) {
				delete attack;
			}
			return can_remove;
		}), enemy_teleport_out_list.end());
}

void DeathGod::on_render() {
	if (is_attack1) {
		image_attack1->on_render();
	}
	if (is_attack2) {
		image_attack2->on_render();
	}
	if (is_attack3) {
		image_attack3->on_render();
	}
	if (is_attack4) {
		image_attack4->on_render();
	}
	if (is_attack5) {
		image_attack5->on_render();
	}
	if (is_teleport_out) {
		image_teleport_out->on_render();
	}
	if (is_teleport_in) {
		image_teleport_in->on_render();
	}
	for (EnemyAttack1* attack : enemy_attack1_list) {
		attack->draw();
	}
	for (EnemyAttack2* attack : enemy_attack2_list) {
		attack->draw();
	}
	for (EnemyAttack4* attack : enemy_attack4_list) {
		attack->draw();
	}
	for (EnemyAttack5* attack : enemy_attack5_list) {
		attack->draw();
	}
}

void DeathGod::on_take_hit() {
	switch (range_random(1, 3)) {
	case 1:
		play_audio(_T("enemy_hurt_1"), false);
		break;
	case 2:
		play_audio(_T("enemy_hurt_2"), false);
		break;
	case 3:
		play_audio(_T("enemy_hurt_3"), false);
		break;
	}
}

void DeathGod::on_attack1() {
	if (is_attack1) {
		return;
	}
	is_attack1 = true;
	EnemyAttack1* attack = new EnemyAttack1();
	enemy_attack1_list.push_back(attack);

	image_attack1->reset();
}
void DeathGod::on_attack2() {
	if (is_attack2) {
		return;
	}
	is_attack2 = true;
	EnemyAttack2* attack = new EnemyAttack2();
	enemy_attack2_list.push_back(attack);

	image_attack2->reset();
}

void DeathGod::on_attack3() {
	if (is_attack3)
		return;
	is_attack3 = true;


	velocity.y = -100;
	image_attack3->reset();
}

void DeathGod::on_attack4() {
	if (is_attack4) {
		return;
	}
	is_attack4 = true;
	EnemyAttack4* attack = new EnemyAttack4(CharacterManager::instance()->get_player()->get_logic_center(),is_facing_left);
	enemy_attack4_list.push_back(attack);

	image_attack4->reset();
}
void DeathGod::on_attack5() {
	if (is_attack5) {
		return;
	}
	is_attack5 = true;

	EnemyAttack5* attack = new EnemyAttack5(CharacterManager::instance()->get_player()->get_logic_center(),is_facing_left);
	enemy_attack5_list.push_back(attack);

	image_attack5->reset();
}

void DeathGod::on_teleport_out() {
	if (is_teleport_out) {
		return;
	}
	is_teleport_out = true;
	set_position(CharacterManager::instance()->get_player()->get_logic_center());
	EnemyTeleportOut* attack;
	if (hp >= 50) {
		vector2 pos = CharacterManager::instance()->get_player()->get_logic_center()+vector2(range_random(-200, 200), 550);
		if (pos.x >= getwidth()-150) {
			pos.x = getwidth()-150;
		}
		if (pos.x <= 150) {
			pos.x = 150;
		}
		 attack = new EnemyTeleportOut(pos);
	}
	if (hp <= 50) {
		int x = range_random(1, 3);
		vector2 pos = vector2({ 0,220 });
		if (x == 1) {
			pos.x = 150;
		}
		if (x == 2) {
			pos.x = 640;
		}
		if (x == 3) {
			pos.x = getwidth() - 150;
		}
	}
	enemy_teleport_out_list.push_back(attack);
	image_teleport_out->reset();
}

void DeathGod::on_teleport_in() {
	if (is_teleport_in) {
		return;
	}
	is_teleport_in = true;
	
	image_teleport_in->reset();
}
void DeathGod::on_run() {
	if (is_run) {
		return;
	}
	is_run = true;
	int player_x = CharacterManager::instance()->get_player()->get_logic_center().x;
	int x_left = is_facing_left ? max(player_x - 450, 150) : min(player_x + 400, 1050);
	int x_right = is_facing_left ? max(player_x - 400, 200) : min(player_x + 450,1100 );
	
	if (!(position.x >= x_left && position.x <= x_right)) {
		if (position.x >= x_right)
		{
			velocity.x = -150;
		}
		if (position.x <= x_left) {
			velocity.x = 150;
		}
	}
	else {
		velocity.x = 0;
		is_run = false;
	}

}
void DeathGod::render_hp_bar() {

}