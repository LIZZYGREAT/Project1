#include "resources_manager.h"
#include"util.h"

ResourcesManager* ResourcesManager::manager = nullptr;
ResourcesManager* ResourcesManager::instance() {
	if (!manager)
		manager = new ResourcesManager();
	return manager;
}

ResourcesManager::ResourcesManager() = default;
ResourcesManager::~ResourcesManager() {
	delete manager;
}

struct ImageResInfo {
	std::string id;
	LPCTSTR path;
};
struct AtlasResInfo {
	std::string id;
	LPCTSTR path;
	int num_frame = 0;
};

static const std::vector<ImageResInfo>image_info_list = {  //列表初始化+R标记 不需要对反斜杠进行转义处理
	//场景
	{"dawn_bk",_T(R"(background\Dawn\dawn_bk.png)")},

	//玩家
	{"player_attack1_right",_T(R"(Player\Fantasy_Warrior\Attack1.png)")},
	{"player_attack2_right",_T(R"(Player\Fantasy_Warrior\Attack2.png)")},
	{"player_attack3_right",_T(R"(Player\Fantasy_Warrior\Attack3.png)")},

	{"player_dead_right",_T(R"(Player\Fantasy_Warrior\Death.png)")},
	{"player_fall_right",_T(R"(Player\Fantasy_Warrior\Fall.png)")},
	{"player_idle_right",_T(R"(Player\Fantasy_Warrior\Idle.png)")},
	{"player_run_right",_T(R"(Player\Fantasy_Warrior\Run.png)")},
	{"player_jump_right",_T(R"(Player\Fantasy_Warrior\Jump.png)")},
	{"player_take_hit_right",_T(R"(Player\Fantasy_Warrior\Take_hit.png)")},

	{"player_vfx_jump",_T(R"(Player\Fantasy_Warrior\vfx_jump.png)")},
	{"player_vfx_land",_T(R"(Player\Fantasy_Warrior\vfx_land.png)")},
};

static const std::vector<AtlasResInfo> atlas_info_list =
{

};

static inline bool check_image_valid(IMAGE* image) {
	return GetImageBuffer();
}

void ResourcesManager::flip_image(IMAGE* src_image, IMAGE* dst_image, int num_h)
{
	int w = src_image->getwidth();
	int h = src_image->getheight();
	int w_frame = w / num_h;
	Resize(dst_image, w, h);
	DWORD* src_buffer = GetImageBuffer(src_image);
	DWORD* dst_buffer = GetImageBuffer(dst_image);
	for (int i = 0; i < num_h; i++) {
		int x_left = i * w_frame;
		int x_right = (i + 1) * w_frame;
		for (int y = 0; y < h; y++) {
			for (int x = x_left; x < x_right; x++) {
				int idx_src = y * w + x;
				int idx_dst = y * w + x_right - (x - x_left);
				dst_buffer[idx_dst] = src_buffer[idx_src];
			}
		}
	}
}
void ResourcesManager::flip_image(const std::string& src_id, const std::string& dst_id, int num_h) {

	IMAGE* src_image = image_pool[src_id];
	IMAGE* dst_image = new IMAGE();

	flip_image(src_image, dst_image, num_h);

	image_pool[dst_id] = dst_image;
}
void ResourcesManager::flip_atlas(const std::string& src_id, const std::string& dst_id) {
	Atlas* src_atlas = atlas_pool[src_id];
	Atlas* dst_atlas = new Atlas();

	for (int i = 0; i < src_atlas->get_size(); i++) {
		IMAGE img_flipped;
		flip_image(src_atlas->get_image(i), &img_flipped);
		dst_atlas->add_image(img_flipped);
	}
	atlas_pool[dst_id] = dst_atlas;
}

void ResourcesManager::load() {
	for (const auto& info : image_info_list) {
		IMAGE* image = new IMAGE();
		loadimage(image, info.path);
		if (!check_image_valid(image)) {
			throw info.path;
		}
		image_pool[info.id] = image;
	}
	for (const auto& info : atlas_info_list) {
		Atlas* atlas = new Atlas();
		atlas->load(info.path, info.num_frame);
		for (int i = 0; i < atlas->get_size(); i++) {
			IMAGE* image = atlas->get_image(i);
			if (!check_image_valid(image)) {
				throw info.path;
			}
		}
		atlas_pool[info.id] = atlas;
	}

	flip_image("player_attack1_right", "player_attack1_left", 7);//这里加数字只是为了方便反转,而与绘图无关，实际加载的image是一个长条
	flip_image("player_attack2_right", "player_attack2_left", 7);
	flip_image("player_attack3_right", "player_attack3_left", 8);

	flip_image("player_dead_right", "player_dead_left", 7);
	flip_image("player_fall_right", "player_fall_left", 3);
	flip_image("player_idle_right", "player_idle_left", 10);
	flip_image("player_jump_right", "player_jump_left", 3);
	flip_image("player_run_right", "player_run_left", 8);
	flip_image("player_take_hit_right", "player_take_hit_left", 3);


	load_audio(_T(R"(audio\enemy_dash.mp3)"), _T("enemy_dash"));
	load_audio(_T(R"(audio\enemy_run.mp3)"), _T("enemy_run"));
	load_audio(_T(R"(audio\enemy_hurt_1.mp3)"), _T("enemy_hurt_1"));
	load_audio(_T(R"(audio\enemy_hurt_2.mp3)"), _T("enemy_hurt_2"));
	load_audio(_T(R"(audio\enemy_hurt_3.mp3)"), _T("enemy_hurt_3"));


	load_audio(_T(R"(audio\player_attack_1.mp3)"), _T("player_attack_1"));
	load_audio(_T(R"(audio\player_attack_2.mp3)"), _T("player_attack_2"));
	load_audio(_T(R"(audio\player_attack_3.mp3)"), _T("player_attack_3"));
	load_audio(_T(R"(audio\player_dead.mp3)"), _T("player_dead"));
	load_audio(_T(R"(audio\player_hurt.mp3)"), _T("player_hurt"));
	load_audio(_T(R"(audio\player_jump.mp3)"), _T("player_jump"));
	load_audio(_T(R"(audio\player_land.mp3)"), _T("player_land"));
	load_audio(_T(R"(audio\player_run.mp3)"), _T("player_run"));
}

Atlas* ResourcesManager::find_atlas(const std::string& id)const {
	const auto& itor = atlas_pool.find(id);
	if (itor == atlas_pool.end()) {  //迭代器中find()如果未查找到则返回end()
		return nullptr;
	}
	return itor->second;  //注意::itor->first代表id，second代表 键值
}

IMAGE* ResourcesManager::find_image(const std::string& id)const {
	const auto& itor = image_pool.find(id);
	if (itor == image_pool.end()) {
		return nullptr;
	}
	return itor->second;
}


