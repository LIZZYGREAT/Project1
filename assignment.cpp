#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <easyx.h>
#include <time.h>
#include<graphics.h>
#include <mmsystem.h>
#include<conio.h>

using namespace std;


const int window_width = 1280;
const int window_height = 720;
const int button_width = 192;
const int button_height = 75;

int idx_current_anim = 0;
const int player_anim_num = 6;

int current_hp_player = 100;

bool is_game_started = false;
bool running = true;



#pragma comment(lib,"MSIMG32.LIB")
#pragma comment(lib,"Winmm.lib")


inline void putimage_alpha(int x, int y, IMAGE* img) {
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h, GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}//处理不同图片,去除背景

//截取一张图片不同部位，去除背景
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
	);//
}

// 封装的 loadimage_alpha 函数
void loadimage_alpha(IMAGE* pDstImg, LPCTSTR pImgFile, int srcX, int srcY, int dstWidth, int dstHeight) {
	// 首先创建一个临时 IMAGE 对象，用于加载完整的图片
	IMAGE tempImg;
	// 加载完整的图片到临时对象
	loadimage(&tempImg, pImgFile);

	// 调整目标 IMAGE 对象的大小为指定的截取区域大小
	Resize(pDstImg, dstWidth, dstHeight);

	// 获取临时图片和目标图片的设备上下文句柄
	HDC hdcTemp = GetImageHDC(&tempImg);
	HDC hdcDst = GetImageHDC(pDstImg);

	// 使用 BitBlt 函数从临时图片中复制指定区域到目标图片
	BitBlt(hdcDst, 0, 0, dstWidth, dstHeight, hdcTemp, srcX, srcY, SRCCOPY);
}

class Animation {
public:
	Animation(LPCTSTR path, int num, int interval) {
		//z变量中z = 1表示绘制面左，z = 2表示绘制面右
		interval_ms = interval;
		TCHAR path_file[256];
		frame_list_opposite.resize(num); // 初始化 frame_list_opposite
		frame_sketch.resize(num);
		frame_sketch_opposite.resize(num);
		

		for (size_t i = 0; i < num; i++) {
			_stprintf_s(path_file, path, i);
			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);
			frame_list.push_back(frame);
			
			int width = frame->getwidth();
			int height = frame->getheight();
			frame_list_opposite[i] = new IMAGE();
			Resize(frame_list_opposite[i], width, height);
				
			DWORD* color_buffer = GetImageBuffer(frame_list[i]);
			DWORD* color_buffer_opposite = GetImageBuffer(frame_list_opposite[i]);
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					int idx_img = y * width + x;
					int idx_opposite_img = y * width + width - x - 1;
					color_buffer_opposite[idx_opposite_img] = color_buffer[idx_img];
				}
			}

			frame_sketch[i] = new IMAGE();
			frame_sketch_opposite[i] = new IMAGE();
			Resize(frame_sketch[i], width, height);
			Resize(frame_sketch_opposite[i], width, height);

			DWORD* color_buffer_sketch = GetImageBuffer(frame_sketch[i]);
			DWORD* color_buffer_sketch_opposite = GetImageBuffer(frame_sketch_opposite[i]);

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					int idx = y * width + x;
					if ((color_buffer[idx] & 0xFF000000) >> 24)
						color_buffer_sketch[idx] = BGR(RGB(255, 255, 255)) | (((DWORD)(BYTE)(255)) << 24);
					if ((color_buffer_opposite[idx] & 0xFF000000) >> 24)
						color_buffer_sketch_opposite[idx] = BGR(RGB(255, 255, 255)) | (((DWORD)(BYTE)(255)) << 24);
				}
			}
		}
	}
	~Animation() {
		for (size_t i = 0; i < frame_list.size(); i++) {
			delete frame_list[i];
		}
		for (size_t i = 0; i < frame_list_opposite.size(); i++) {
			delete frame_list_opposite[i]; // 释放 frame_list_opposite 的内存
		}
	}
	void play(int x, int y, int delta,int dir,int is_sketch) {
		timer += delta;
		if (timer > interval_ms) {
			idx_frame = (idx_frame + 1) % frame_list.size();
			timer = 0;
		}
		if ( dir == 1 && is_sketch ==0)
		{
			putimage_alpha(x, y, frame_list[idx_frame]);
		}
		if (dir == 2 && is_sketch ==0) {
			putimage_alpha(x, y, frame_list_opposite[idx_frame]);
		}
		if (dir == 1 && is_sketch == 1)
		{
			putimage_alpha(x, y, frame_sketch[idx_frame]);
		}
		if (dir == 2 && is_sketch == 1) {
			putimage_alpha(x, y, frame_sketch_opposite[idx_frame]);
		}
	}
private:
	int interval_ms = 0;
	int idx_frame = 0;
	int timer = 0;
	vector<IMAGE*>frame_list;
	vector<IMAGE*>frame_list_opposite;
	vector<IMAGE*>frame_sketch;
	vector<IMAGE*>frame_sketch_opposite;
};

class Animation_new {
public:
	Animation_new(LPCTSTR path, int x, int y, int width, int height, int interval) {
		interval_ms = interval;
		frame_list.resize(x * y);
		frame_list_opposite.resize(x * y);

		for (int i = 0; i < y; ++i) {
			for (int j = 0; j < x; ++j) {
				frame_list[i * x + j] = new IMAGE();
				loadimage_alpha(frame_list[i * x + j], path, j * width, i * height, width, height);

				frame_list_opposite[i * x + j] = new IMAGE();
				Resize(frame_list_opposite[i * x + j], width, height);

				DWORD* color_buffer = GetImageBuffer(frame_list[i * x + j]);
				DWORD* color_buffer_opposite = GetImageBuffer(frame_list_opposite[i * x + j]);

				for (int height_y = 0; height_y < height; ++height_y) {
					for (int width_x = 0; width_x < width; ++width_x) {
						int idx_img = height_y * width + width_x;
						int idx_opposite_img = height_y * width + width - width_x - 1;
						color_buffer_opposite[idx_opposite_img] = color_buffer[idx_img];
					}
				}
			}
		}
	}

	~Animation_new() {
		// 释放 frame_list 中的内存
		for (auto frame : frame_list) {
			delete frame;
		}
		// 释放 frame_list_opposite 中的内存
		for (auto frame : frame_list_opposite) {
			delete frame;
		}
	}
	void play(int pos_x,int pos_y,int delta,int dir) {
		timer += delta;
		if (timer > interval_ms) {
			idx_frame = (idx_frame + 1) % frame_list.size();
			timer = 0;
		}
		if (dir == 1) {
			putimage_alpha(pos_x, pos_y, frame_list[idx_frame]);
		}
		if (dir == 2) {
			putimage_alpha(pos_x, pos_y, frame_list_opposite[idx_frame]);
		}
	}
private:
	int interval_ms = 0;
	int idx_frame = 0;
	int timer = 0;
	std::vector<IMAGE*> frame_list;
	std::vector<IMAGE*> frame_list_opposite;
};

class Button {
public:
	Button(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed) {
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
			else if (status == Status::Hovered && !checkcursorhit(msg.x, msg.y)) {
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


class startgamebutton :public Button {
public:
	startgamebutton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed) :
		Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {
	}
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
		Button(rect, path_img_idle, path_img_hovered, path_img_pushed) {
	}
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

private:
	bool visited[5] = { false };
	bool visited_energy[5] = { false };

	DWORD lastHitTime = 0;
	
	int blinkTimer = 0;
	bool isBlinking = false;

	int max_energy = 100;
	int current_energy = 100;

	int idx_energy = 0;

private:
	Animation_new* fireball;
	Animation_new* fireflush_left;
	Animation_new* explode;
	Animation_new* lava;
	Animation_new* shift;

	IMAGE shield;
public:
	Player() {
		loadimage(&player_shadow, _T("images/shadow_player.png"));
		anim_left = new Animation(_T("images/paimon_left_%d.png"), 6, 45);
		loadimage(&shield, _T("skill/Q.png"));

		fireball = new Animation_new(_T("skill/fireball.png"), 4, 4, 100, 100, 45);
		fireflush_left = new Animation_new(_T("skill/3.png"), 5, 5, 70, 70, 45);
		explode = new Animation_new(_T("skill/F.png"), 5, 8, 192, 192, 45);
		lava = new Animation_new(_T("skill/2.png"), 2, 2, 170, 171, 45);
		shift = new Animation_new(_T("skill/shift.png"), 10, 10, 80, 107, 45);
	}
	~Player() {
		delete anim_left;
		delete fireball;
		delete fireflush_left;
		delete explode;
		delete lava;
		delete shift;
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
			case 0x31:
				Fireball(msg.x, msg.y);
				break;
			case VK_SPACE:
				Shift();
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
	void max_hp(int x) {
		//每10长度对应1滴血//x代表score
		if (0 <= x && x < 10) {
			max_length = 100;
		}
		if (10 <= x && x < 25) {
			max_length = 120;
		}
		if (25 <= x && x < 50) {
			max_length = 150;
		}
		if (50 <= x && x < 75) {
			max_length = 180;
		}
		if (75 <= x && x < 100) {
			max_length = 210;
		}
		if (x >= 100) {
			max_length = 250;
		}
	}
	void current_hp(int x) {
		//随等级增加而增加血量//最大生命值增加也会增加当前生命值
		if (10 <= x && x < 25 && !visited[0]) {
			current_hp_player += 20;
			visited[0] = true;
		}
		if (25 <= x && x < 50 && !visited[1]) {
			current_hp_player += 30;
			visited[1] = true;
		}
		if (50 <= x && x < 75 && !visited[2]) {
			current_hp_player += 30;
			visited[2] = true;
		}
		if (75 <= x && x < 100 && !visited[3]) {
			current_hp_player += 30;
			visited[3] = true;
		}
		if (x >= 100 && !visited[4]) {
			current_hp_player += 40;
			visited[4] = true;
		}
		//每击杀10只小猪回复20滴血，最大回满
		if (x - idx_score == 10) {
			if (current_hp_player <= max_length - 20) {
				current_hp_player += 20;
				idx_score = x;
			}
			else if (current_hp_player < max_length) {
				current_hp_player = max_length;
				idx_score = x;
			}
		}
	}
	bool canBeHit() {
		DWORD currentTime = GetTickCount();
		return currentTime - lastHitTime >= 500;
	}
	void setLastHitTime() {
		lastHitTime = GetTickCount();
	}

	void  Max_energy(int score) {
		if (score <= 10) {
			max_energy = 100;
		}
		else if (score <= 25 && score > 10) {
			max_energy = 150;
		}
		else if (score <= 50 && score > 25) {
			max_energy = 200;
		}
		else if (score <= 75 && score > 50) {
			max_energy = 250;
		}
		else if (score <= 100 && score > 75) {
			max_energy = 300;
		}
		else if (score > 100) {
			max_energy = 400;
		}
	}

	void new_energy(int score) {
		if (score <= 25 && score > 10 && !visited_energy[0]) {
			current_energy += 50;
			visited_energy[0] = true;
		}
		else if (score <= 50 && score > 25 && !visited_energy[1]) {
			current_energy += 50;
			visited_energy[1] = true;
		}
		else if (score <= 75 && score > 50 && !visited_energy[2]) {
			current_energy += 50;
			visited_energy[2] = true;
		}
		else if (score <= 100 && score > 75 && !visited_energy[3]) {
			current_energy += 50;
			visited_energy[3] = true;
		}
		else if (score > 100 && !visited_energy[4]) {
			current_energy += 100;
			visited_energy[4] = true;
		}
	}

	void Fireball(int x, int y) {	//技能1  耗蓝25  CD 5S  效果：鼠标决定 击中地点 造成40点伤害
		//函数效果 ：绘制图像+蓝耗解决 CD处理
		if(current_energy >= 25){
			current_energy -= 25;
			fireball->play(x, y, 45, 1);
		}
	}
	void Shift() {  //space  //不耗蓝  CD 3S  释放后有0.5s无敌帧  向前位移若干个身位，但位置不超过地图边缘，若超过则恰好卡在地图边缘
		int dir_x = is_move_right - is_move_left;
		int dir_y = is_move_down - is_move_up;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0) {
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;

			shift->play(position.x + 4 * frame_width * normalized_x, position.y + 3 * frame_height * normalized_y, 45, 1);

			position.x = position.x + 4 * frame_width * normalized_x;
			position.y = position.y + 3 * frame_height * normalized_y;
		}
	}
	void Flashfire() {  //技能3	 //耗蓝50  10S	像指定方向发射冲击  伤害75点

	}
	void Expolde() {  //技能F  //耗蓝75   CD 15s 在自身周围产生一团爆炸圈 3 次 每次造成50点伤害

	}
	void Sheild() { //技能Q  //额外护盾槽 消耗护盾值50  // CD 1S  增加的护盾可以抵消50点伤害

	}
	void Lava() {  //技能2  //耗蓝 50  CD 8 S 存在4s的岩浆池 对指定位置 生成 释放时造成25点伤害，每秒灼烧伤害25点

	}
	void draw(int delta) {
		setlinecolor(RGB(0, 0, 0));
		rectangle(0, 30, max_length, 40);

		if (current_hp_player < max_length) {
			setfillcolor(RGB(255, 193, 193));
			fillrectangle(current_hp_player, 30, max_length, 40);
		}

		setfillcolor(RGB(238, 44, 44));
		fillrectangle(0, 30, current_hp_player, 40);

		setlinecolor(RGB(0, 0, 0));
		rectangle(position.x+0.5*(frame_width-0.5*max_length), position.y - 14, position.x + 0.5 *(frame_width+0.5* max_length), position.y - 10);

		if (current_hp_player < max_length) {
			setfillcolor(RGB(255, 193, 193));
			fillrectangle(position.x + 0.5 * (frame_width -0.5*max_length)+0.5*current_hp_player, position.y - 14, position.x + 0.5 *(frame_width+0.5* max_length), position.y - 10);
		}

		setfillcolor(RGB(238, 44, 44));
		fillrectangle(position.x+0.5*(frame_width -0.5*max_length), position.y - 14, position.x + 0.5 *(frame_width-0.5*max_length)+0.5*current_hp_player, position.y - 10);

		static TCHAR text[64];
		swprintf_s(text, _T("当前玩家的血量为:%d / %d"), current_hp_player,max_length);


		setbkmode(TRANSPARENT);
		settextcolor(RGB(255, 85, 185));
		outtextxy(current_hp_player + 10, 27, text);

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

		DWORD currentTime = GetTickCount();
		if (currentTime - lastHitTime < 500) { // 0.5 秒内
			blinkTimer += delta;
			if (blinkTimer >= 100) { // 每 100 毫秒切换一次状态
				blinkTimer = 0;
				isBlinking = !isBlinking;
			}
			if (isBlinking) {
				if (facing_left) {
					anim_left->play(position.x, position.y, 45, 1, 1);
				}
				else {
					anim_left->play(position.x, position.y, 45, 2, 1);
				}
			}
			else {
				if (facing_left) {
					anim_left->play(position.x, position.y, 45, 1, 0);
				}
				else {
					anim_left->play(position.x, position.y, 45, 2, 0);
				}
			}
		}
		else {
			if (facing_left) {
				anim_left->play(position.x, position.y, 45, 1, 0);
			}
			else {
				anim_left->play(position.x, position.y, 45, 2, 0);
			}
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

	bool is_fireball = true;


	bool is_fireflush = false;
	bool is_explode = false;


	bool is_shift = true;


	bool is_shield = false;
	bool is_lava = false;

	Animation* anim_left;
	POINT position = { 500,500 };
	int max_length = 100;
	int idx_score = 0;

};


class Bullet {
public:
	POINT position = { 0,0 };
	int power = 20;
private:
	bool visited[4] = { false };
public:
	Bullet() = default;
	~Bullet() = default;

	void draw() const {
		setlinecolor(RGB(255, 155, 50));
		setfillcolor(RGB(200, 75, 10));
		fillcircle(position.x, position.y, RADIUS);
	}
	void attackpower(int x) {
		if (x < 25&&x>=10 && !visited[0]) {
			power += 10;
			visited[0] = true;
		}
		else if (x >= 25 && x < 50 && !visited[1]) {
			power += 10;
			visited[1] = true;
		}
		else if (x >= 50 && x < 100 && !visited[2]) {
			power += 10;
			visited[2] = true;
		}
		else if (x >= 10 && !visited[3]) {
			power += 10;
			visited[3] = true;
		}
	}
private:
	const int RADIUS = 10;
};


class Enemy {
private:
	DWORD lastHitTime = 0;

	int max_length = 40;

public:
	int current_hp_boar = 40;

public:
	Enemy() {
		loadimage(&boar_shadow, _T("images/shadow_enemy.png"));
		anim_left = new Animation(_T("images/boar_left_%d.png"), 6, 45);

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
	bool canBeHit() {
		DWORD currentTime = GetTickCount();
		return currentTime - lastHitTime >= 100;
	}
	void setLastHitTime() {
		lastHitTime = GetTickCount();
	}
	bool checkbulletcollision(const Bullet& bullet) {
		bool is_overlap_x = bullet.position.x > position.x && bullet.position.x <= position.x + frame_width;
		bool is_overlap_y = bullet.position.y > position.y && bullet.position.y <= position.y + frame_height;

		if (is_overlap_x && is_overlap_y && canBeHit()) {
			current_hp_boar -= bullet.power;
			setLastHitTime();
		}
		return is_overlap_x && is_overlap_y;
	}
	bool checkplayercollision(Player& paimon) {
		POINT check_position = { position.x + 0.5 * frame_width,position.y + 0.5 * frame_height };
		const POINT& paimon_position = paimon.GetPosition();
		bool is_overlap_x = check_position.x > paimon_position.x && check_position.x < paimon_position.x + paimon.frame_width;
		bool is_overlap_y = check_position.y > paimon_position.y && check_position.y < paimon_position.y + paimon.frame_height;

		if (is_overlap_x && is_overlap_y && paimon.canBeHit()) {
			current_hp_player -= 30;
			paimon.setLastHitTime();
		}
		return is_overlap_x && is_overlap_y;
	}
	void move(const Player& player) {
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
	void max_hp(int x) {
		if (x>=25 && x<50 ) {
			max_length = 0.75 * frame_width;
			speed = 2.2;
		}
		if (x >= 50 && x<100) {
			max_length = frame_width;
			speed = 2.5;
		}
		if (x >= 100 ) {
			max_length = 1.25 * frame_width;
			speed = 2.75;
		}
	}
	void current_hp(int x) {
		if (x >= 25 && !visited[0]) {
			current_hp_boar += 20;
			visited[0] = true;
		}
		if (x >= 50  && !visited[1]) {
			current_hp_boar += 20;
			visited[1] = true;
		}
		if (x >= 100 && !visited[2]) {
			current_hp_boar += 20;
			visited[2] = true;
		}
	}
	void speed_up(const Player& paimon,int score) {
		int distance_x = (paimon.GetPosition().x - position.x) > (position.x - paimon.GetPosition().x) ? (paimon.GetPosition().x - position.x) : (position.x - paimon.GetPosition().x);
		int distance_y = (paimon.GetPosition().y - position.y) > (position.y - paimon.GetPosition().y) ? (paimon.GetPosition().y - position.y) : (position.y - paimon.GetPosition().y);
		if (score < 50) {
			if ((distance_x+distance_y) <= paimon.frame_width * 8 && !detected_speedup[0]) {
				speed *= 1.3;
				detected_speedup[0] = true;
			}
		}
		else {
			if ((distance_x+distance_y) <= paimon.frame_width * 10 && !detected_speedup[1]) {
				speed *= 1.1;
				detected_speedup[1] = true;
			}
		}
	}
	void draw(int delta) {

		setlinecolor(RGB(0, 0, 0));
		rectangle(position.x + 0.5 * (frame_width - max_length), position.y -5 , position.x + 0.5 * (frame_width + max_length), position.y -2);

		if (current_hp_boar < max_length) {//损失血条
			setfillcolor(RGB(255, 193, 193));
			fillrectangle(position.x + 0.5 * (frame_width -max_length)+current_hp_boar, position.y - 5, position.x + 0.5 * (frame_width + max_length), position.y - 2);
		}

		//实时血条
		setfillcolor(RGB(238, 44, 44));
		fillrectangle(position.x + 0.5 * (frame_width - max_length), position.y -5, position.x + 0.5 * (frame_width -max_length)+current_hp_boar, position.y -2);


		int shadow_pos_x = position.x + (frame_width - shadow_width) / 2;
		int shadow_pos_y = position.y + frame_height - 25;

		putimage_alpha(shadow_pos_x, shadow_pos_y, &boar_shadow);

		if (facing_left) {
			anim_left->play(position.x, position.y, 45,1,0);
		}
		else {
			anim_left->play(position.x, position.y, 45,2,0);
		}
	}
	bool checkalive() {
		if (current_hp_boar <= 0) {
			alive = false;
		}
		return alive;
	}
	~Enemy() {
		delete anim_left;
	}

private:
	double speed = 2;

	const int frame_width = 80;
	const int frame_height = 80;
	const int shadow_width = 48;
private:
	IMAGE boar_shadow;
	Animation* anim_left;
	POINT position = { 0,0 };
	bool facing_left = false;
	bool alive = true;

	bool visited[3] = { false };
	bool detected_speedup[3] = { false };
};


class Boss {
private:
	int max_length = 800;
public:
	const int frame_width = 170;
	const int frame_height = 188;

	int current_hp_boss = 800;

public:
	Boss() {
		loadimage(&boss_shadow, _T("images/shadow_enemy.png"));
		anim_left = new Animation_new(_T("boss1/boss1.png"), 4, 2, frame_width, frame_height, 45);
	}
	~Boss(){
		delete anim_left;
	}
	void draw(int delta) {

		//boss上方小血条
		setlinecolor(RGB(0, 0, 0));
		rectangle(position.x + 0.5 * (frame_width - 0.1*max_length), position.y - 5, position.x + 0.5 * (frame_width + 0.1*max_length), position.y - 2);

		if (current_hp_boss < max_length) {//损失血条
			setfillcolor(RGB(255, 193, 193));
			fillrectangle(position.x + 0.5 * (frame_width - 0.1*max_length) + 0.1*current_hp_boss, position.y - 5, position.x + 0.5 * (frame_width + 0.1*max_length), position.y - 2);
		}

		//实时血条
		setfillcolor(RGB(238, 44, 44));
		fillrectangle(position.x + 0.5 * (frame_width - 0.1*max_length), position.y - 5, position.x + 0.5 * (frame_width - 0.1*max_length) + 0.1*current_hp_boss, position.y - 2);

		//屏幕上方大血条
		setlinecolor(RGB(0, 0, 0));
		rectangle(0.5 * (window_width - max_length), 90, 0.5 * (window_width + max_length), 100);

		if (current_hp_boss < max_length) {//损失血条
			setfillcolor(RGB(255, 193, 193));
			fillrectangle(0.5*(window_width - max_length) + current_hp_boss, 90, 0.5 * (window_width + max_length), 100);
		}

		//实时血条
		setfillcolor(RGB(238, 44, 44));
		fillrectangle(0.5 * (window_width - max_length), 90, 0.5 * (window_width - max_length)+current_hp_boss,100);

		int shadow_pos_x = position.x + (frame_width - shadow_width) / 2;
		int shadow_pos_y = position.y + frame_height  -10;
		
		putimage_alpha(shadow_pos_x, shadow_pos_y, &boss_shadow);

		if (facing_left) {
			anim_left->play(position.x, position.y, 45, 1);
		}
		else {
			anim_left->play(position.x, position.y, 45, 2);
		}
	}

private:
	const int speed = 1;
	const int shadow_width = 32;

private:
	IMAGE boss_shadow;
	Animation_new* anim_left;
	POINT position = { 800,360 };

	bool facing_left = false;
};


void generateboar(vector<Enemy*>& enemy_list,int x) {
	static int interval = 80;
	if (0<= x && x< 20) {
		interval = 80;
	}
	if (20 <= x && x<50) {
		interval = 70;
	}
	if (50 <= x  && x < 75) {
		interval = 65;
	}
	if (75 <= x ) {
		interval = 60;
	}
	static int counter = 0;
	if ((++counter) % interval == 0)
		enemy_list.push_back(new Enemy());
}

void updatebullets(vector<Bullet>& bullet_list, const Player& paimon,int x) {
	static double r_speed = 0.0025;
	static double t_speed = 0.0025;
	bullet_list.resize(2);
	if (10 <= x &&x< 25) {
		r_speed = 0.0030;
		t_speed = 0.0025;
		bullet_list.resize(2);
	}
	if (25 <= x &&x< 50) {
		r_speed = 0.0030;
		t_speed = 0.0030;
		bullet_list.resize(3);
	}
	if (50 <= x &&x< 75) {
		r_speed = 0.0035;
		t_speed = 0.0035;
		bullet_list.resize(4);
	}
	if (75 <= x&&x<100 ) {
		r_speed = 0.0040;
		t_speed = 0.0040;
		bullet_list.resize(5);
	}
	if (100 <= x) {
		bullet_list.resize(6);
	}

	double radian_interval = 2 * 3.14159 / bullet_list.size();
	POINT player_position = paimon.GetPosition();
	double radius = 80 + 40 * sin(GetTickCount() * r_speed);
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

	srand(time(NULL));
	initgraph(1280, 720);

	mciSendString(_T("open mus/bgm.mp3 alias bgm"), NULL, 0, NULL);
	mciSendString(_T("open mus/hit.wav alias hit"), NULL, 0, NULL);


	int score = 0;

	ExMessage msg;

	IMAGE img_menu;
	IMAGE image_background;

	Player paimon;
	Boss boss1;

	vector<Enemy*> boar_list;
	vector<Bullet>bullet_list;

	RECT region_btn_start_game, region_btn_quit_game;

	region_btn_start_game.left = (window_width - button_width) / 2;
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
	loadimage(&image_background, _T("scene/scene1.png"));

	BeginBatchDraw();

	while (running) {
		DWORD start_time = GetTickCount();
		while (peekmessage(&msg)) {
			if (is_game_started)
				paimon.pocessevent(msg);
			else {
				btn_start_game.processevent(msg);
				btn_quit_game.processevent(msg);
			}
		}
		if (is_game_started)
		{
			paimon.move();
			paimon.current_hp(score);
			paimon.max_hp(score);
			paimon.new_energy(score);
			paimon.Max_energy(score);

			updatebullets(bullet_list, paimon,score);
			generateboar(boar_list,score);

			for (Enemy* boar : boar_list) {
				boar->speed_up(paimon,score);
				boar->move(paimon);
				boar->current_hp(score);
				boar->max_hp(score);
			}
			bool collision_detected = false;
			for (Enemy* boar : boar_list) {
				if (boar->checkplayercollision(paimon)) {
					collision_detected = true;
				}
			}
			if (collision_detected && current_hp_player <= 0) {
				static TCHAR text[128];
				_stprintf_s(text, _T("最终得分：%d!"), score);
				MessageBox(GetHWnd(), text, _T("游戏结束"), MB_OK);
				running = false;
			}
			for (Enemy* boar : boar_list) {
				for (const Bullet& bullet : bullet_list) {
					if (boar->checkbulletcollision(bullet)) {
						mciSendString(_T("play hit from 0"), NULL, 0, NULL);
						if (boar->current_hp_boar <= 0) {
							score++;
						}
					}
				}
			}
		}
		cleardevice();
		if (is_game_started) {
			putimage(0, 0, &image_background);
			paimon.draw(1000 / 144);
			boss1.draw(1000 / 144);
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
	mciSendString(_T("close bgm"), NULL, 0, NULL);

	mciSendString(_T("close hit"), NULL, 0, NULL);

	EndBatchDraw();
	for (Enemy* boar : boar_list) {
		delete boar;
	}
	return 0;

}