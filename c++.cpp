#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <easyx.h>
#include <time.h>
#include<graphics.h>
using namespace std;


const int window_width = 1280;
const int window_height = 720;
const int button_width = 192;
const int button_height = 75;

int idx_current_anim = 0;
const int player_anim_num = 6;

bool is_game_started = false;
bool running = true;

#pragma comment(lib,"MSIMG32.LIB")
#pragma comment(lib,"Winmm.lib")


inline void putimage_alpha(int x, int y, IMAGE* img) {
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h, GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}//处理不同图片

//截取一张图片不同部位
void putimage_beta(int dstx, int dsty, int dstwidth, int dstheight, IMAGE* img, int srcX, int srcY) {
	// 检查参数有效性
	if (img == nullptr || dstwidth <= 0 || dstheight <= 0 || srcX < 0 || srcY < 0 || srcX >= img->getwidth() || srcY >= img->getheight()) {
		return;
	}

	// 计算源图像要复制的实际宽度和高度
	int srcWidth = min(img->getwidth() - srcX, dstwidth);
	int srcHeight = min(img->getheight() - srcY, dstheight);

	// 使用 AlphaBlend 函数进行绘制
	AlphaBlend(
		GetImageHDC(NULL), // 目标设备上下文句柄
		dstx,              // 目标位置 x 坐标
		dsty,              // 目标位置 y 坐标
		dstwidth,          // 目标宽度
		dstheight,         // 目标高度
		GetImageHDC(img),  // 源设备上下文句柄
		srcX,              // 源图像起始 x 坐标
		srcY,              // 源图像起始 y 坐标
		srcWidth,          // 源图像宽度
		srcHeight,         // 源图像高度
		{ AC_SRC_OVER, 0, 255, AC_SRC_ALPHA } // 混合操作标志
	);
}


class Animation {
public:
	Animation(LPCTSTR path, int num, int interval) {
		interval_ms = interval;
		TCHAR path_file[256];

		for (size_t i = 0; i < num; i++) {
			_stprintf_s(path_file, path, i);
			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);
			frame_list.push_back(frame);
		}
	}
	~Animation() {
		for (size_t i = 0; i < frame_list.size(); i++) {
			delete frame_list[i];
		}
	}
	void play(int x, int y, int delta) {
		timer += delta;
		if (timer > interval_ms) {
			idx_frame = (idx_frame + 1) % frame_list.size();
			timer = 0;
		}
		putimage_alpha(x, y, frame_list[idx_frame]);
	}
private:
	int interval_ms = 0;
	int idx_frame = 0;
	int timer = 0;
	vector<IMAGE*>frame_list;
};


class Button {
public:
	Button(RECT rect,LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed) {
		region = rect;

		loadimage(&img_idle, path_img_idle);
		loadimage(&img_hovered, path_img_hovered);
		loadimage(&img_pushed, path_img_pushed);
	}
	~Button() = default;
	void draw() {
		switch (status) {
		case Status::Idle:
			putimage(region.left, region.top, &img_idle);
			break;
		case Status::Hovered:
			putimage(region.left, region.top, &img_hovered);
			break;
		case Status::Pushed:
			putimage(region.left, region.top, &img_pushed);
			break;
		}
	}
	void processevent(const ExMessage& msg) {
		switch (msg.message) {
		case WM_MOUSEMOVE:
			if (status == Status::Idle && checkcursorhit(msg.x, msg.y)) {
				status = Status::Hovered;
			}
			else if (status == Status::Hovered &&  !checkcursorhit(msg.x, msg.y)) {
				status = Status::Idle;
			}
			break;
		case WM_LBUTTONDOWN:
			if (checkcursorhit(msg.x, msg.y)) {
				status = Status::Pushed;
			}
			break;
		case WM_LBUTTONUP:
			if (status == Status::Pushed) {
				OnClick();
			}
			break;
		default:break;
		}
	}
protected:
	virtual void OnClick() = 0;
private:
	enum class Status {
		Idle = 0,
		Hovered,
		Pushed
	};
private:
	RECT region;
	IMAGE img_idle;
	IMAGE img_hovered;
	IMAGE img_pushed;
	Status status = Status::Idle;
private:
	bool checkcursorhit(int x, int y) {
		return x >= region.left && x <= region.right && y >= region.top && y <= region.bottom;
	}
};


class startgamebutton:public Button {
public:
	startgamebutton(RECT rect,LPCTSTR path_img_idle,LPCTSTR path_img_hovered,LPCTSTR path_img_pushed): 
		Button(rect,path_img_idle,path_img_hovered,path_img_pushed) {}
	~startgamebutton() = default;
protected:
	void OnClick() {
		is_game_started = true;

		mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);
	}
};


class quitgamebutton :public Button {
public:
	quitgamebutton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed) :
		Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {}
	~quitgamebutton() = default;
protected:
	void OnClick() {
		running = false;
	}
};


class Player {
public:
	const int frame_width = 80;
	const int frame_height = 80;
public:
	Player() {
		loadimage(&player_shadow, _T("images/shadow_player.png"));
		anim_left = new Animation(_T("images/paimon_left_%d.png"), 6, 45);
		anim_right = new Animation(_T("images/paimon_right_%d.png"), 6, 45);
	}
	~Player() {
		delete anim_left;
		delete anim_right;
	}
	void pocessevent(const ExMessage& msg) {
			if (msg.message == WM_KEYDOWN) {
				switch (msg.vkcode) {
				case VK_UP:
					is_move_up = true;
					break;
				case VK_DOWN:
					is_move_down = true;
					break;
				case VK_LEFT:
					is_move_left = true;
					break;
				case VK_RIGHT:
					is_move_right = true;
					break;
				}
			}
			else if (msg.message == WM_KEYUP) {
				switch (msg.vkcode) {
				case VK_UP:
					is_move_up = false;
					break;
				case VK_DOWN:
					is_move_down = false;
					break;
				case VK_LEFT:
					is_move_left = false;
					break;
				case VK_RIGHT:
					is_move_right = false;
					break;
				}
			}
	}
	void move() {
		int dir_x = is_move_right - is_move_left;
		int dir_y = is_move_down - is_move_up;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0) {
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			position.x += (int)(speed * normalized_x);
			position.y += (int)(speed * normalized_y);
		}
		if (position.x <= 0) position.x = 0;
		if (position.y <= 0) position.y = 0;
		if (position.x + frame_width >= window_width)
			position.x = window_width - frame_width;
		if (position.y + frame_height >= window_height)
			position.y = window_height - frame_height;
	}
	void draw(int delta) {
		int shadow_pos_x = position.x + (frame_width - shadow_width) / 2;
		int shadow_pos_y = position.y + frame_height - 8;

		putimage_alpha(shadow_pos_x, shadow_pos_y, &player_shadow);

		static bool facing_left = false;
		int dir_x = is_move_right - is_move_left;
		if (dir_x > 0) {
			facing_left = false;
		}
		else if (dir_x < 0) {
			facing_left = true;
		}
		if (facing_left) {
			anim_left->play(position.x, position.y, 45);
		}
		else {
			anim_right->play(position.x, position.y, 45);
		}
	}
	const POINT& GetPosition() const {
		return position;
	}
private:
	const int speed = 3;
	const int shadow_width = 32;

private:
	IMAGE player_shadow;
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;
	Animation* anim_left;
	Animation* anim_right;
	POINT position = {500,500};
};


class Bullet {
public:
	POINT position = { 0,0 };
public:
	Bullet() = default;
	~Bullet() = default;

	void draw() const {
		setlinecolor(RGB(255, 155, 50));
		setfillcolor(RGB(200, 75, 10));
		fillcircle(position.x, position.y, RADIUS);
		
	}
private:
	const int RADIUS = 10;
};


class Enemy {
public:
	Enemy() {
		loadimage(&boar_shadow, _T("images/shadow_enemy.png"));
		anim_left = new Animation(_T("images/boar_left_%d.png"), 6, 45);
		anim_right = new Animation(_T("images/boar_right_%d.png"), 6, 45);

		enum class spawnedge {
			Up = 0,
			Down,
			Left,
			Right
		};
		spawnedge edge = (spawnedge)(rand() % 4);
		switch (edge) {
		case spawnedge::Up:
			position.x = rand() % window_width;
			position.y = -frame_height;
			break;
		case spawnedge::Down:
			position.x = rand() % window_width;
			position.y = window_height;
			break;
		case spawnedge::Left:
			position.x = -frame_width;
			position.y = rand() % window_height;
			break;
		case spawnedge::Right:
			position.x = window_width;
			position.y = rand() % window_height;
			break;
		default:
			break;
		}
	}
	bool checkbulletcollision(const Bullet& bullet) {
		bool is_overlap_x = bullet.position.x > position.x && bullet.position.x <= position.x + frame_width;
		bool is_overlap_y = bullet.position.y > position.y && bullet.position.y <= position.y + frame_height;
		return is_overlap_x && is_overlap_y;
	}
	bool checkplayercollision(const Player& paimon) {
		POINT check_position = { position.x + 0.5 * frame_width,position.y + 0.5 * frame_height };
		const POINT& paimon_position = paimon.GetPosition();
		bool is_overlap_x = check_position.x > paimon_position.x && check_position.x < paimon_position.x + paimon.frame_width;
		bool is_overlap_y = check_position.y > paimon_position.y && check_position.y < paimon_position.y + paimon.frame_height;
		return is_overlap_x && is_overlap_y;
	}
	void move(const Player&player) {
		const POINT& player_position = player.GetPosition();
		int dir_x = player_position.x - position.x;
		int dir_y = player_position.y - position.y;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0) {
				double normalized_x = dir_x / len_dir;
				double normalized_y = dir_y / len_dir;
				position.x += (int)(speed * normalized_x);
				position.y += (int)(speed * normalized_y);
			
		}
		if (dir_x < 0) {
			facing_left = true;
		}
		else if (dir_x > 0) {
			facing_left = false;
		}
	}
	void draw(int delta) {
		int shadow_pos_x = position.x + (frame_width - shadow_width) / 2;
		int shadow_pos_y = position.y + frame_height - 25;

		putimage_alpha(shadow_pos_x, shadow_pos_y, &boar_shadow);

		if (facing_left) {
			anim_left->play(position.x, position.y, 45);
		}
		else {
			anim_right->play(position.x, position.y, 45);
		}
	}
	void Hurt() {
		alive = false;
	}
	bool checkalive() {
		return alive;
	}
	~Enemy() {
		delete anim_left;
		delete anim_right;
	}

private:
	const int speed = 2;
	const int frame_width = 80;
	const int frame_height = 80;
	const int shadow_width = 48;
private:
	IMAGE boar_shadow;
	Animation* anim_left;
	Animation* anim_right;
	POINT position = { 0,0 };
	bool facing_left = false;
	bool alive = true;
};


void generateboar(vector<Enemy*>& enemy_list) {
	const int interval = 100;
	static int counter = 0;
	if ((++counter) % interval == 0)
		enemy_list.push_back(new Enemy());
}


void updatebullets(vector<Bullet>& bullet_list, const Player& paimon) {
	const double r_speed = 0.0025;
	const double t_speed = 0.0025;
	double radian_interval = 2 * 3.14159 / bullet_list.size();
	POINT player_position = paimon.GetPosition();
	double radius = 100 + 25 * sin(GetTickCount() * r_speed);
	for (size_t i = 0; i < bullet_list.size(); i++) {
		double radian = GetTickCount() * t_speed + radian_interval * i;
		bullet_list[i].position.x = player_position.x + paimon.frame_width / 2 + (int)(radius * sin(radian));
		bullet_list[i].position.y = player_position.y + paimon.frame_height / 2 + (int)(radius * cos(radian));
	}
}


void drawscore(int score) {
	static TCHAR text[64];
	_stprintf_s(text, _T("当前玩家得分:%d"), score);

	setbkmode(TRANSPARENT);
	settextcolor(RGB(255, 85, 185));
	outtextxy(10, 10, text);

}


int main() {
	initgraph(1280, 720);

	mciSendString(_T("open mus/bgm.mp3 alias bgm"), NULL, 0, NULL);
	mciSendString(_T("open mus/hit.wav alias hit"), NULL, 0, NULL);


	int score = 0;
	Player paimon;
	ExMessage msg;

	IMAGE img_menu;
	IMAGE image_background;

	vector<Enemy*> boar_list;
	vector<Bullet>bullet_list(3);

	RECT region_btn_start_game, region_btn_quit_game;

	region_btn_start_game.left = (window_width - button_width)/2;
	region_btn_start_game.right = region_btn_start_game.left + button_width;
	region_btn_start_game.top = 430;
	region_btn_start_game.bottom = region_btn_start_game.top + button_height;

	region_btn_quit_game.left = (window_width - button_width) / 2;
	region_btn_quit_game.right = region_btn_quit_game.left + button_width;
	region_btn_quit_game.top = 550;
	region_btn_quit_game.bottom = region_btn_quit_game.top + button_height;

	startgamebutton btn_start_game = startgamebutton(region_btn_start_game, _T("images/start_idle.png"), _T("images/start_hovered.png"),
		_T("images/start_pushed.png"));

	quitgamebutton btn_quit_game = quitgamebutton(region_btn_quit_game, _T("images/quit_idle.png"), _T("images/quit_hovered.png"),
		_T("images/quit_pushed.png"));

	loadimage(&img_menu, _T("images/menu.png"));
	loadimage(&image_background, _T("images/background.png"));

	BeginBatchDraw();

	while (running) {
		DWORD start_time = GetTickCount();
		while (peekmessage(&msg)) {
			if(is_game_started)
				paimon.pocessevent(msg);
			else {
				btn_start_game.processevent(msg);
				btn_quit_game.processevent(msg);
			}
		}
		if (is_game_started)
		{
			paimon.move();
			updatebullets(bullet_list, paimon);
			generateboar(boar_list);
			for (Enemy* boar : boar_list) {
				boar->move(paimon);
			}
			for (Enemy* boar : boar_list) {
				if (boar->checkplayercollision(paimon)) {
					static TCHAR text[128];
					_stprintf_s(text, _T("最终得分：%d!"), score);
					MessageBox(GetHWnd(), text, _T("游戏结束"), MB_OK);
					running = false;
					break;
				}
			}
			for (Enemy* boar : boar_list) {
				for (const Bullet& bullet : bullet_list) {
					if (boar->checkbulletcollision(bullet)) {
						mciSendString(_T("play hit from 0"), NULL, 0, NULL);
						boar->Hurt();
						score++;
					}
				}
			}

		}
		cleardevice();
		if (is_game_started) {
			putimage(0, 0, &image_background);
			paimon.draw(1000 / 144);
			for (Enemy* boar : boar_list) {
				boar->draw(1000 / 144);
			}
			for (const Bullet& bullet : bullet_list) {
				bullet.draw();
			}
			for (size_t i = 0; i < boar_list.size(); i++) {
				Enemy* boar = boar_list[i];
				if (!boar->checkalive()) {
					swap(boar_list[i], boar_list.back());
					boar_list.pop_back();
					delete boar;
				}
			}
			drawscore(score);
		}
		else {
			putimage(0, 0, &img_menu);
			btn_start_game.draw();
			btn_quit_game.draw();
		}

		FlushBatchDraw();


		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;
		if (delta_time < 1000 / 144) {
			Sleep(1000 / 144 - delta_time);
		}
	}
	EndBatchDraw();
	return 0;
}