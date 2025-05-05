#include"player.h"
#include"character_manager.h"
#include"death_god.h"

//#include"enemy.h"

CharacterManager* CharacterManager::manager = nullptr;
CharacterManager* CharacterManager::instance() {
	if (!manager)
		manager = new CharacterManager();

	return manager;
}

void CharacterManager::on_input(const ExMessage& msg) {
	player->on_input(msg);

}
void CharacterManager::on_update(float delta) {
	death_god->on_update(delta);
	player->on_update(delta);
}
void CharacterManager::on_render() {
	death_god->on_render();
	player->on_render();
}
CharacterManager::CharacterManager() {
	death_god = new DeathGod();
	player = new Player();
}
CharacterManager::~CharacterManager() {
	delete death_god;
	delete player;

	delete manager;
}