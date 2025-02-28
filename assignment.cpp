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
#include<random>

using namespace std;

std::random_device rd;
std::mt19937 gen(rd());


const int window_width = 1280;
const int window_height = 720;
const int button_width = 192;
const int button_height = 75;
const double M_PI = 3.14159;

int idx_current_anim = 0;
const int player_anim_num = 6;

int current_hp_player = 500;
int protectivevalue = 0;

bool is_game_started = false;
bool running = true;

bool should_generate_ghost = true;

#pragma comment(lib,"MSIMG32.LIB")
#pragma comment(lib,"Winmm.lib")


inline void putimage_alpha(int x, int y, IMAGE* img) {
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h, GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}//处理不同图片,去除背景


// 设置像素为透明（通过修改Alpha通道实现）
void makeBlackTransparent(DWORD* buffer, int width, int height) {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = y * width + x;
			DWORD color = buffer[index];
			int r = GetRValue(color);
			int g = GetGValue(color);
			int b = GetBValue(color);
			if (r == 0 && g == 0 && b == 0) {
				buffer[index] = 0x00FFFFFF;  // 设置Alpha通道为0，即透明
			}
		}
	}
}

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

		for (size_t i = 0; i < num; i++) {
			_stprintf_s(path_file, path, i);
			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);
			frame_list.push_back(frame);
			
			int width = frame->getwidth();
			int height = frame->getheight();


			IMAGE *frame_opposite = new IMAGE();
			loadimage(frame_opposite, path_file);
			Resize(frame_opposite, width, height);
			frame_list_opposite.push_back(frame_opposite);

			DWORD* color_buffer = GetImageBuffer(frame_list[i]);
			DWORD* color_buffer_opposite = GetImageBuffer(frame_list_opposite[i]);
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					int idx_img = y * width + x;
					int idx_opposite_img = y * width + width - x - 1;
					color_buffer_opposite[idx_opposite_img] = color_buffer[idx_img];
				}
			}

			IMAGE* frame_sketch = new IMAGE();
			IMAGE* frame_sketch_opposite = new IMAGE();

			Resize(frame_sketch, width, height);
			Resize(frame_sketch_opposite, width, height);
			frame_sketch_list.push_back(frame_sketch);
			frame_sketch_opposite_list.push_back(frame_sketch_opposite);

			DWORD* color_buffer_sketch = GetImageBuffer(frame_sketch_list[i]);
			DWORD* color_buffer_sketch_opposite = GetImageBuffer(frame_sketch_opposite_list[i]);

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
			delete frame_list_opposite[i]; // 释放 frame_list_opposite 的内存
			delete frame_sketch_list[i];
			delete frame_sketch_opposite_list[i];
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
			putimage_alpha(x, y, frame_sketch_list[idx_frame]);
		}
		if (dir == 2 && is_sketch == 1) {
			putimage_alpha(x, y, frame_sketch_opposite_list[idx_frame]);
		}
	}
private:
	int interval_ms = 0;
	int idx_frame = 0;
	int timer = 0;
	vector<IMAGE*>frame_list;
	vector<IMAGE*>frame_list_opposite;
	vector<IMAGE*>frame_sketch_list;
	vector<IMAGE*>frame_sketch_opposite_list;
};

class Animation_new {
public:
	Animation_new(LPCTSTR path, int x, int y, int width, int height, int interval,int deal_black){  //参数讲解： x为横向个数，y为纵向个数，width，height分别为单个的宽高，interval为帧率间隔，z是判断是否需要处理黑色背景
		interval_ms = interval;                                                   
		for (int i = 0; i < y; ++i) {
			for (int j = 0; j < x; ++j) {
				IMAGE * frame = new IMAGE();
				loadimage_alpha(frame, path, j * width, i * height, width, height);
				if (deal_black == 1) {
					int width = frame->getwidth();
					int height = frame->getheight();
					DWORD* buffer = GetImageBuffer(frame);

					makeBlackTransparent(buffer, width, height);
				}
				frame_list.push_back(frame);

				IMAGE* frame_opposite = new IMAGE();
				Resize(frame_opposite, width, height);
				frame_list_opposite.push_back(frame_opposite);

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
		for (auto frame_opposite : frame_list_opposite) {
			delete frame_opposite;
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
		if (dir == -1) {
			putimage_alpha(pos_x, pos_y, frame_list_opposite[idx_frame]);
		}
	}
	void reset() {
		idx_frame = 0;
		timer = 0;
	}
	int getFrameCount() const {
		return frame_list.size();
	}

	IMAGE* getFrame(int index) const {
		return frame_list[index];
	}

	DWORD getTotalDuration() const {
		return interval_ms * frame_list.size();
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

	int max_length = 500;
public://处理受伤问题
	//BOSS1
	DWORD lastblood1hit = 0;
	DWORD lastblood2hit = 0;
	DWORD lastblood2hit2 = 0;
	DWORD lastblood3hit = 0;
	DWORD lastbullethit = 0;
	//BOSS2
	DWORD lastredexplodehit = 0;
	DWORD lastredeyehit = 0;
	DWORD lastlightninghit = 0;
	DWORD lastbluehit = 0;
	DWORD lastyellowhit = 0;
	DWORD lastrandomhit = 0;
	

private://处理非重复访问问题
	bool visited[5] = { false };
	bool visited_energy[5] = { false };
	bool visited_protective[4] = { false };

	int blinkTimer = 0;
	bool isBlinking = false;

	int max_energy = 100;
	int current_energy = 100;
	int idx_energy = 0;

	int max_protective = 50;
	int current_protective = 50;
	int idx_protective = 0;

public://处理CD问题与每秒回复问题
	DWORD lastTime = 0;

	DWORD is_shift_end = 0;
	bool is_shift_anim_playing = false;  // 记录 shift 动画是否正在播放
	DWORD shift_anim_start_time = 0;     // 记录 shift 动画开始的时间

	DWORD is_fireball_end = 0;				//记录fireball是否正在播放
	DWORD fireball_anim_start_time = 0;    //记录fireball 开始时间

	DWORD is_explode_end = 0;					 //记录fireball是否正在播放
	DWORD explode_anim_start_time = 0;    //记录fireball 开始时间

	DWORD is_fireflush_end = 0;            //记录fireflush是否结束
	DWORD fireflush_anim_start_time = 0;	//fireflush开始时间
	bool fireflush_animation_ended = false;  //fireflush第一轮动画是否结束


	DWORD lastShieldRecoveryTime = 0;
	const DWORD shieldRecoveryInterval = 1000;
	const int shieldRecoveryAmount = 2;

	DWORD lastEnergyRecoveryTime = 0;
	const DWORD energyRecoveryInterval = 1000;
	const int energyRecoveryAmount = 3;
	
	DWORD lastHpRecoveryTime = 0;
	const DWORD hpRecoveryInterval = 1000;
	const int hpRecoveryAmount = 2;

	bool is_fireflush_anim_playing = false;		//检测对应技能是否还在绘制//只要还在绘制就增加伤害判定
	bool is_explode_anim_playing = false;
	bool is_fireball_anim_playing = false;

public://获取鼠标点击时坐标
	void setLeftClickPosition(int x, int y) {
		leftClickX = x;
		leftClickY = y;
	}
	int getLeftClickX() const {
		return leftClickX;
	}
	int getLeftClickY() const {
		return leftClickY;
	}

	int leftClickX = 0;
	int leftClickY = 0;

	int fireball_x = 0;
	int fireball_y = 0;

	int fireflush_start_x;  // 记录 fireflush 释放时的 x 坐标
	int fireflush_start_y;  // 记录 fireflush 释放时的 y 坐标
	int fireflush_direction; // 记录 fireflush 释放时的方向（1表示右，-1表示左）

	int fireflush_x;
	int fireflush_y;

public:
	Player() {

		loadimage(&player_shadow, _T("images/shadow_player.png"));
		loadimage(&shield, _T("images/bubble.png"));

		anim_left = new Animation(_T("images/paimon_left_%d.png"),6,45);
		fireball = new Animation_new(_T("images/1.png"), 4, 4, 100, 100, 180,0);
		fireflush = new Animation_new(_T("images/3.png"), 5, 5, 70, 70, 45,1);
		firepart = new Animation_new(_T("images/3_part.png"), 5, 2, 70, 70, 45, 1);
		explode = new Animation_new(_T("images/F.png"), 5, 8, 192, 192, 60,1);
		shift_end = new Animation_new(_T("images/shift_end.png"), 5, 7, 192, 192,75,1);

	}
	~Player() {
		delete anim_left;
		delete fireball;
		delete fireflush;
		delete explode;
		delete shift_end;
	}
	void pocessevent(const ExMessage& msg) {
		if (msg.message == WM_KEYDOWN) {
			switch (msg.vkcode) {
			case 'W':
				is_move_up = true;
				break;
			case 'S':
				is_move_down = true;
				break;
			case 'A':
				is_move_left = true;
				break;
			case 'D':
				is_move_right = true;
				break;
			case '1':
				if (GetTickCount() - is_fireball_end > 5000) {  //检查冷却时间
					is_fireball = true;
					skill1_fireball();
					is_fireball_end = GetTickCount();
				}
				break;
			case '2':
				if (GetTickCount() - is_fireflush_end > 5000) {
					is_fireflush = true;
					skill2_fireflush();
					is_fireflush_end = GetTickCount();
				}
				break;
			case VK_SPACE:
				if (GetTickCount() - is_shift_end > 7000) {  // 检查冷却时间
					is_shift = true;
					skillspace_shift();
					is_shift_end = GetTickCount();
				}
				break;
			case 'Q':
				skillq_shield(get_score);
				break;
			case 'R':
				if (GetTickCount() - is_explode_end > 9000) {
					is_explode = true;
					skillr_expolde();
					is_explode_end = GetTickCount();
				}
				break;
			default:
				break;
			}
		}
		else if (msg.message == WM_KEYUP) {
			switch (msg.vkcode) {
			case 'W':
				is_move_up = false;
				break;
			case 'S':
				is_move_down = false;
				break;
			case 'A':
				is_move_left = false;
				break;
			case 'D':
				is_move_right = false;
				break;
			case '1':
				if (!is_fireball_anim_playing) {
					is_fireball = false;
				}
				break;
			case '2':
				if (!is_fireflush_anim_playing) {
				is_fireflush = false;
			}
				break;
			case VK_SPACE:
				is_shift = false;
				break;
			case 'R':
				if (!is_explode_anim_playing) {
					is_explode = false;
				}
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
		if (10 <= x && x < 25) {
			max_length =500;
		}
		if (25 <= x && x < 50) {
			max_length =500;
		}
		if (50 <= x && x < 75) {
			max_length =500;
		}
		if (75 <= x && x < 100) {
			max_length = 500;
		}
		if (x >= 100) {
			max_length =500;
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
		if (x - idx_score >= 10) {
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
	//每秒回复2点生命值：
	void current_hp_recovery() {
		DWORD currentTime = GetTickCount();
		if (currentTime - lastHpRecoveryTime >= hpRecoveryInterval) {
			current_hp_player = min(current_hp_player+ hpRecoveryAmount,max_length );
			lastHpRecoveryTime = currentTime;
		}
	}
	bool canBeHit() {
		DWORD currentTime = GetTickCount();
		if (is_shift_anim_playing) {
			return false;
		}
		return currentTime - lastTime >= 600;
	}
	void setLastTime() {
		lastTime = GetTickCount();
	}
	void Max_protective(int score) {
		if (score >= 25 && score < 50) {
			max_protective = 75;
		}
		else if (score >= 50 && score < 75) {
			max_protective = 100;
		}
		if (score >= 75 && score < 100) {
			max_protective = 125;
		}
		if (score >= 100) {
			max_protective = 150;
		}
	}
	void new_protective(int score) {
		if (score >= 25 && score < 50 && !visited_protective[0]) {
			current_protective +=25;
			visited_protective[0] = true;
		}
		else if (score >= 50 && score < 75 && !visited_protective[1]) {
			current_protective +=25;
			visited_protective[1] = true;
		}
		else if (score >= 75 && score < 100 && !visited_protective[2]) {
			max_protective +=25;
			visited_protective[2] = true;
		}
		else if (score >= 100 && !visited_protective[3]) {
			max_protective +=25;
			visited_protective[3] = true;
		}
		if (score - idx_protective > 0) {
			current_protective += min(1 * (score - idx_protective), max_protective - current_protective);
			idx_protective = score;
		}
	}
	//每1s回复3点护盾值
	void current_protective_recovery() {
		DWORD currentTime = GetTickCount();
		if (currentTime - lastShieldRecoveryTime >= shieldRecoveryInterval) {
			current_protective =min(current_protective + shieldRecoveryAmount, max_protective);
			lastShieldRecoveryTime = currentTime;
		}
	}
	void  Max_energy(int score) {
		int old_max_energy = max_energy;
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
		int energy_increase = max_energy - old_max_energy;
		current_energy = min(current_energy + energy_increase, max_energy);
	}
	void current_energy_recovery() {
		DWORD currentTime = GetTickCount();
		if (currentTime - lastEnergyRecoveryTime >= energyRecoveryInterval) {
			current_energy = min(current_energy + energyRecoveryAmount, max_energy);
			lastEnergyRecoveryTime = currentTime;
		}
	}
	void Getscore(int x) {
		get_score = x;
	}
	void skill1_fireball() {	//技能1  耗蓝25  CD 5S  效果：鼠标决定 击中地点 造成40点伤害
		//函数效果 ：绘制图像+蓝耗解决 CD处理
		if (current_energy >= 20) {
			current_energy -= 20;
			is_fireball_anim_playing = true;
			fireball_anim_start_time = GetTickCount();
			if (getLeftClickX() != 0 && getLeftClickY() != 0) {
				fireball_x = getLeftClickX();
				fireball_y = getLeftClickY();
			}
			fireball->reset();
		}
		else {
			return;
		}
	}
	void draw_fireball() {
		DWORD current_time = GetTickCount();
		if (is_fireball_anim_playing) {
			if (current_time - fireball_anim_start_time < 16*67.5) {
				fireball->play(fireball_x-50, fireball_y-50,45, 1);
			}
			else {
				is_fireball_anim_playing = false;
			}
		}
	}
	void skillspace_shift() {  //space  //  CD 3S   向前位移若干个身位，但位置不超过地图边缘，若超过则恰好卡在地图边缘
		int dir_x = is_move_right - is_move_left;
		int dir_y = is_move_down - is_move_up;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0) {
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;

			int newX = position.x + 4 * frame_width * normalized_x;
			int newY = position.y + 3 * frame_height * normalized_y;

			// 确保位置不超出窗口范围
			newX = max(0, min(newX, window_width - 110));
			newY = max(0, min(newY, window_height - 110));

			position.x = newX;
			position.y = newY;
		}
		is_shift_anim_playing = true;
		shift_anim_start_time = GetTickCount();
		shift_end->reset();
	}
	void skill2_fireflush() {  //技能3	 //耗蓝50  5S	 伤害55点 只可向左右方向进行发射
		if (current_energy >= 25) {
			current_energy -= 25;
			is_fireflush_anim_playing = true;
			fireflush_anim_start_time = GetTickCount();
			fireflush->reset();
			fireflush_animation_ended = false;

			bool facing_left = player_facing_left;
			fireflush_direction = facing_left ? -1 : 1;
			fireflush_start_x = position.x;
			fireflush_start_y = position.y;
		}
		else {
			return;
		}
	}
	void skillr_expolde() {  //技能F  //耗蓝75   CD 15s 在自身周围产生一团爆炸圈 3 次 每次造成50点伤害
		if (current_energy >= 45) {
			current_energy -= 45;
			is_explode_anim_playing = true;
			explode_anim_start_time = GetTickCount();
			explode->reset();
		}
		else {
			return;
		}
	}
	void skillq_shield(int score) { //技能Q  //额外护盾槽 消耗护盾值50  // CD 1S  增加的护盾可以抵消50点伤害  //护盾被破可以对周围范围造成50点伤害
		if (current_protective >= 50 && protectivevalue <=80) {
			current_protective -= 40;
			if (score < 25){
				protectivevalue += 30;
			}
			if (score >= 25 && score < 50) {
				protectivevalue  +=40;
			}
			if (score >= 50 && score < 100) {
				protectivevalue += 50;
			}
			if (score >= 100) {
				protectivevalue +=75;
			}
		}
	}
	void draw_hp(int delta) {
		//绘制左上方血条
		setlinecolor(RGB(0, 0, 0));
		rectangle(0, 30, max_length, 40);

		if (current_hp_player < max_length) {
			setfillcolor(RGB(255, 193, 193));
			fillrectangle(current_hp_player, 30, max_length, 40);
		}
		if (current_hp_player > 0) {
			setfillcolor(RGB(238, 44, 44));
			fillrectangle(0, 30, current_hp_player, 40);
		}

		//绘制paimon头顶
		setlinecolor(RGB(0, 0, 0));
		rectangle(position.x + 0.5 * (frame_width - 0.5 * max_length), position.y - 14, position.x + 0.5 * (frame_width + 0.5 * max_length), position.y - 10);

		if (current_hp_player < max_length) {
			setfillcolor(RGB(255, 193, 193));
			fillrectangle(position.x + 0.5 * (frame_width - 0.5 * max_length) + 0.5 * current_hp_player, position.y - 14, position.x + 0.5 * (frame_width + 0.5 * max_length), position.y - 10);
		}
		if (current_hp_player > 0) {
			setfillcolor(RGB(238, 44, 44));
			fillrectangle(position.x + 0.5 * (frame_width - 0.5 * max_length), position.y - 14, position.x + 0.5 * (frame_width - 0.5 * max_length) + 0.5 * current_hp_player, position.y - 10);
		}
		//血量描述
		static TCHAR text[64];
		swprintf_s(text, _T("当前玩家的血量为:%d / %d"), current_hp_player, max_length);
		setbkmode(TRANSPARENT);
		settextcolor(RGB(255, 85, 185));
		outtextxy(max_length + 10, 27, text);

		if (current_hp_player <= 0) {
			paimon_is_dead = true;
		}
	}
	void draw(int delta) {

		//绘制蓝条
		setlinecolor(RGB(0, 0, 0));
		rectangle(0, 44,0.75* max_energy, 54);

		if (current_energy < max_energy) {
			setfillcolor(RGB(122,197,205));
			fillrectangle(0.75*current_energy, 44,0.75* max_energy, 54);
		}

		setfillcolor(RGB(72,118, 255));
		fillrectangle(0, 44,0.75* current_energy, 54);

		//蓝量描述
		static TCHAR text1[64];
		swprintf_s(text1, _T("当前玩家的蓝量为:%d / %d"), current_energy,max_energy);
		setbkmode(TRANSPARENT);
		settextcolor(RGB(72, 118, 255));
		outtextxy(0.75*max_energy + 10,44, text1);

		//绘制护盾条
		setlinecolor(RGB(0, 0, 0));
		rectangle(0, 58,max_protective, 68);

		if (current_protective < max_protective) {
			setfillcolor(RGB(255,255,224));
			fillrectangle(current_protective, 58,max_protective, 68);
		}

		setfillcolor(RGB(255,140,0));
		fillrectangle(0, 58,current_protective, 68);

		//护盾描述：
		static TCHAR text2[64];
		swprintf_s(text2, _T("当前玩家的护盾值为:%d / %d"), current_protective, max_protective);
		setbkmode(TRANSPARENT);
		settextcolor(RGB(255, 140, 0));
		outtextxy(max_protective + 10, 58, text2);

		//并且在玩家上方血条绘制一定长度的护盾值
		if (protectivevalue > 0) {
			setlinecolor(RGB(0, 0, 0));
			rectangle(position.x + 0.5 * (frame_width - 0.5 * max_length), position.y - 18, position.x + 0.5 * (frame_width - 0.5 * max_length) + protectivevalue, position.y - 14);

			setfillcolor(RGB(248, 248, 255));
			fillrectangle(position.x + 0.5 * (frame_width - 0.5 * max_length), position.y - 18, position.x + 0.5 * (frame_width - 0.5 * max_length) + protectivevalue, position.y - 14);
		}

		//绘制护盾
		if (protectivevalue > 0) {
			putimage_alpha(position.x +0.5*frame_width-60, position.y+0.5*frame_height -60, &shield);
		}
		//绘制explode
		if (is_explode) {
			DWORD current_time_explode = GetTickCount();
			if (is_explode_anim_playing) {
				if (current_time_explode - explode_anim_start_time < 45 * 40 + 1200 + 2000) {
					explode->play(position.x + 0.5 * frame_width - 96, position.y + 0.5 * frame_height - 96, 30, 1);
				}
				else {
					is_explode_anim_playing = false;
				}
			}
		}
		//绘制fireflush
		if (is_fireflush_anim_playing) {
			DWORD current_time_fireflush = GetTickCount();
			DWORD elapsed_time = current_time_fireflush - fireflush_anim_start_time;
			fireflush_x = fireflush_start_x + 0.5 * frame_width + elapsed_time * 0.5 * fireflush_direction;
			fireflush_y = fireflush_start_y + 0.2 * frame_height;

			if (!fireflush_animation_ended) {
				if (elapsed_time < fireflush->getTotalDuration() &&(fireflush_x  <= window_width) &&(fireflush_x  >=0 )) {
					fireflush->play(fireflush_x, fireflush_y, 30, fireflush_direction == 1 ? 1 : -1);
				}
				else {
					// 动画第一轮播放结束，设置标志位
					fireflush_animation_ended = true;
				}
			}
			if (fireflush_animation_ended) {
				elapsed_time = GetTickCount() - fireflush_anim_start_time;
				if((fireflush_x  <= window_width) &&(fireflush_x  >= 0)) {
					firepart->play(fireflush_x , fireflush_y, 30, fireflush_direction == 1 ? 1 : -1);
				}
				else {
					is_fireflush_anim_playing = false;
					fireflush_animation_ended = false;
				}
			}
		}

		//绘制阴影
		int shadow_pos_x = position.x + (frame_width - shadow_width) / 2;
		int shadow_pos_y = position.y + frame_height - 8;

		putimage_alpha(shadow_pos_x, shadow_pos_y, &player_shadow);


		//绘制人物面左还是面右
		
		int dir_x = is_move_right - is_move_left;
		if (dir_x > 0) {
			player_facing_left = false;
		}
		else if (dir_x < 0) {
			player_facing_left = true;
		}

		DWORD currentTime = GetTickCount();
		if (currentTime - lastTime < 1000) { // 0.5 秒内
			blinkTimer += delta;
			if (blinkTimer >= 100) { // 每 100 毫秒切换一次状态
				blinkTimer = 0;
				isBlinking = !isBlinking;
			}
			if (isBlinking) {
				if (player_facing_left) {
					anim_left->play(position.x, position.y, 45, 1, 1);
				}
				else {
					anim_left->play(position.x, position.y, 45, 2, 1);
				}
			}
			else {
				if (player_facing_left) {
					anim_left->play(position.x, position.y, 45, 1, 0);
				}
				else {
					anim_left->play(position.x, position.y, 45, 2, 0);
				}
			}
		}
		else {
			if (player_facing_left) {
				anim_left->play(position.x, position.y, 45, 1, 0);
			}
			else {
				anim_left->play(position.x, position.y, 45, 2, 0);
			}
		}
		//绘制闪现
		DWORD current_time = GetTickCount();
		if (is_shift_anim_playing) {
			if (current_time - shift_anim_start_time < 35 * 30) {
				shift_end->play(position.x + 0.5 * frame_width - 96, position.y + 0.5 * frame_height - 96, 45, 1);
			}
			else {
				is_shift_anim_playing = false;
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
	IMAGE shield;
	Animation_new* fireball;
	Animation_new* fireflush;
	Animation_new* firepart;
	Animation_new* explode;
	Animation_new* shift_end;
	Animation* anim_left;

	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;

	bool is_fireball = false;
	bool is_fireflush = false;
	bool is_explode = false;
	bool is_shift = false;
	bool is_lava = false;

	POINT position = { 500,500 };
	int idx_score = 0;

	int get_score = 0;//用于获取主函数的score又不想该score为全局变量
	bool player_facing_left = false;
public:
	bool paimon_is_dead = false;
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
	DWORD lastexplodehit = 0;
	DWORD lastfireballhit = 0;
	DWORD lastfireflushhit = 0;
	DWORD lastshifthit = 0;

	int max_length = 40;
public:
	int current_hp_boar = 40;

public:
	Enemy() {
		loadimage(&boar_shadow, _T("images/shadow_enemy.png"));
		anim_left = new Animation(_T("images/boar_left_%d.png"), 6, 45);

		std::uniform_int_distribution<> edgeDis(0, 3);
		enum class spawnedge {
			Up = 0,
			Down,
			Left,
			Right
		};
		spawnedge edge = static_cast<spawnedge>(edgeDis(gen));
		switch (edge) {
		case spawnedge::Up: {
			std::uniform_int_distribution<> xDis(0, window_width - 1);
			position.x = xDis(gen);
			position.y = -frame_height;
			break;
		}
		case spawnedge::Down: {
			std::uniform_int_distribution<> xDis(0, window_width - 1);
			position.x = xDis(gen);
			position.y = window_height;
			break;
		}
		case spawnedge::Left: {
			std::uniform_int_distribution<> yDis(0, window_height - 1);
			position.x = -frame_width;
			position.y = yDis(gen);
			break;
		}
		case spawnedge::Right: {
			std::uniform_int_distribution<> yDis(0, window_height - 1);
			position.x = window_width;
			position.y = yDis(gen);
			break;
		}
		default:break;
		}
	}
	bool canBeHit() {
		DWORD currentTime = GetTickCount();
		return currentTime - lastHitTime >= 150;
	}
	void setLastHitTime() {
		lastHitTime = GetTickCount();
	}
	void updateattack(int score) {
		if (score < 25) {
			attack = 30;
		}
		if (25 <= score &&score< 50) {
			attack = 40;
		}
		if (50 <= score && score < 75) {
			attack = 50;
		}
		if (score >= 75 &&score <100) {
			attack = 60;
		}
		if (score > 100) {
			attack = 50;
		}
	}
	void checkbulletcollision(const Bullet& bullet) {
		bool is_overlap_x = bullet.position.x > position.x && bullet.position.x <= position.x + frame_width;
		bool is_overlap_y = bullet.position.y > position.y && bullet.position.y <= position.y + frame_height;

		if (is_overlap_x && is_overlap_y && canBeHit()) {
			current_hp_boar -= bullet.power;
			setLastHitTime();
		}
	}
	void checkplayercollision(Player& paimon) {
		POINT check_position = { position.x + 0.5 * frame_width,position.y + 0.5 * frame_height };
		const POINT& paimon_position = paimon.GetPosition();
		bool is_overlap_x = check_position.x > paimon_position.x && check_position.x < paimon_position.x + paimon.frame_width;
		bool is_overlap_y = check_position.y > paimon_position.y && check_position.y < paimon_position.y + paimon.frame_height;

		if (is_overlap_x && is_overlap_y && paimon.canBeHit()) {
			current_hp_boar += min(0.5*max_length, max_length - current_hp_boar);
			if (protectivevalue == 0) {
				current_hp_player -= attack;
			}
			else if (protectivevalue < attack) {
				current_hp_player -= attack - protectivevalue;
				protectivevalue = 0;
			}
			else {
				protectivevalue -= attack;
			}
			paimon.setLastTime(); 
		}
	}
	void checkexplodecollision(Player& paimon) {
		const POINT& paimon_position = paimon.GetPosition();
		int explosion_radius = 192;
		double distance = sqrt(((position.x + 0.5 * frame_width) - (paimon_position.x + 0.5 * paimon.frame_width)) * ((position.x + 0.5 * frame_width) - (paimon_position.x + 0.5 * paimon.frame_width)) +
			((position.y + 0.5 * frame_height) - (paimon_position.y + 0.5 * paimon.frame_height))* ((position.y + 0.5 * frame_height) - (paimon_position.y + 0.5 * paimon.frame_height)));
		if (paimon.is_explode_anim_playing) {
			DWORD current_time = GetTickCount();
			if (distance <=  0.5*explosion_radius &&current_time-lastexplodehit>=1000) {
					current_hp_boar -= min(30,current_hp_boar);
					current_hp_player += min(5, paimon.max_length - current_hp_player);
					lastexplodehit = current_time;
			}
		}
	}
	void checkfireballcollision(Player& paimon) {
		int fireball_radius = 50;
		double distance = sqrt((position.x + 0.5 * frame_width - paimon.fireball_x + fireball_radius) * (position.x + 0.5 * frame_width - paimon.fireball_x + fireball_radius) +
			(position.y + 0.5 * frame_height - paimon.fireball_y + fireball_radius) * (position.y + 0.5 * frame_height - paimon.fireball_y + fireball_radius));
		if (paimon.is_fireball_anim_playing) {
			DWORD current_time = GetTickCount();
			if (distance <= fireball_radius && current_time - lastfireballhit >= 270) {
				current_hp_boar -= min(20, current_hp_boar);
				lastfireballhit = current_time;
			}
		}
	}
	void checkfireflush(Player& paimon) {
		POINT check_position = { position.x + 0.5 * frame_width,position.y + 0.5 * frame_height };
		int fireflush_width = 70;
		int fireflush_height = 70;
		bool is_overlap_x = check_position.x >= paimon.fireflush_x && check_position.x <= paimon.fireflush_x + fireflush_width;
		bool is_overlap_y = check_position.y >= paimon.fireflush_y && check_position.y <= paimon.fireflush_y + fireflush_height;
		DWORD current_time = GetTickCount();
		if (is_overlap_x && is_overlap_y && current_time-lastfireflushhit >=500) {
			current_hp_boar -= 75;
			lastfireflushhit = current_time;
		}
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
	int attack = 30;

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
	bool detected_explode[3] = { false };
};


class Boss {
public:
	//公用技能CD处理
	DWORD skillReleaseTime_all = 0;
	DWORD skill_interval_all = 2500 + rand() % 500;

	bool IsAnySkillPlaying() const {
		return is_blood1_anim_playing || is_blood2_anim_playing || is_blood3_anim_playing;
	}
	int skillChoice = 0;
private:
	// 与移动有关参数
	int safe_radius_max = 0;
	int safe_radius_min = 0;
	int safecenter_x = 0;
	int safecenter_y = 0;

	bool isSafe = false;
	DWORD lastSafetyCheckTime = 0;

	// 记录当前移动方向
	double current_dir_x = 0.5;
	double current_dir_y = 0.5;
	// 记录方向改变间隔时间（毫秒），可按需调整
	int direction_change_interval = 500;
	// 记录下一次允许改变方向的时间
	DWORD next_direction_change_time = 0;
	//获取共用技能坐标等参数
	int bullet_x_start = 0;
	int bullet_y_start = 0;
	int bullet_direction = 0;
	// 与图片加载有关参数
	Animation_new* boss;
	Animation_new* blood1;
	Animation_new* blood2;
	Animation_new* blood3;
	Animation_new* bullet;
	IMAGE boss_shadow;

	//与受击有关参数
	DWORD lastbullethit = 0;
	DWORD lastfireballhit = 0;
	DWORD lastexplodehit = 0;
	DWORD lastfireflushhit = 0;

	int blood3_x_start_left = -75;
	int blood3_x_start_right = window_width;
	int blood3_y_start = 0;
public:
	int blood1_x_start = 0;
	int blood1_y_start = 0;
	int blood1_direction = 0;

	bool is_blood2_anim_playing = false;
	bool is_blood2 = false;
	DWORD is_Blood2_start = 0;
	DWORD is_Blood2_end = 0;
	bool visited_blood2 = false;

	bool is_bullet_anim_playing = false;
	bool is_bullet = false;
	DWORD is_Bullet_start = 0;
	DWORD is_Bullet_end = 0;
	bool visited_bullet = false;

	bool is_blood1_anim_playing = false;
	bool is_blood1 = false;
	DWORD is_Blood1_start = 0;
	DWORD is_Blood1_end = 0;
	bool visited_blood1 = false;

	bool is_blood3_anim_playing = false;
	bool is_blood3 = false;
	DWORD is_Blood3_start = 0;
	DWORD is_Blood3_end = 0;
	bool visited_blood3 = false;

public:
	Boss(LPCTSTR path, int width, int height, int sequence,int Max_hp,int Current_hp) {
		TCHAR path_file[256];
		// 修正路径处理逻辑
		max_hp = Max_hp;
		current_hp = Current_hp;
		_stprintf_s(path_file, path, sequence,sequence);
		boss = new Animation_new(path_file, 4, 2, width, height, 45, 0);
		blood1 = new Animation_new(_T("boss1/blood1.png"), 5, 1, 150, 150, 200, 1);
		blood2 = new Animation_new(_T("boss1/blood2.png"), 5, 1, 150, 150, 60, 1);
		blood3 = new Animation_new(_T("boss1/blood3.png"), 5, 1, 150, 150, 200, 1);
		bullet = new Animation_new(_T("boss1/bullet.png"), 5, 1, 76,30, 45, 1);

		TCHAR shadow_path[256];
		_stprintf_s(shadow_path, _T("boss%d/shadow_enemy.png"), sequence);
		loadimage(&boss_shadow, shadow_path);
		frame_width = width;
		frame_height = height;

		// 初始化时随机生成一个斜向移动方向
		generateNewDiagonalDirection();
		next_direction_change_time = GetTickCount() + direction_change_interval;
	}
	 virtual ~Boss() {
		delete boss;
		delete blood1;
		delete blood2;
		delete blood3;
		delete bullet;
	}

	void changestage() {
		if (current_hp / max_hp > 0.75) {
			stage = 1;
			speed = 2;
		}
		else if (current_hp / max_hp > 0.5) {
			stage = 2;
			speed = 2.5;
		}
		else {
			stage = 3;
			speed = 3;
		}
	}

	bool isInSafe(Player& paimon) {
		int dx = position.x + 0.5 * frame_width - safecenter_x;
		int dy = position.y + 0.5 * frame_height - safecenter_y;
		isSafe = dx * dx + dy * dy <= safe_radius_max * safe_radius_max &&
			safe_radius_min * safe_radius_min <= dx * dx + dy * dy;
		return isSafe;
	}

	void generateSafe(Player& paimon) {
		safecenter_x = paimon.GetPosition().x + 0.5 * paimon.frame_width;
		safecenter_y = paimon.GetPosition().y + 0.5 * paimon.frame_height;

		std::uniform_int_distribution<> dis(0, 300);
		safecenter_x += dis(gen);
		safecenter_y += dis(gen);

		if (stage == 1) {
			std::uniform_int_distribution<> radiusDis(500, 899);
			safe_radius_max = radiusDis(gen);
			safe_radius_min = 0.5 * safe_radius_max;
		}
		if (stage == 2) {
			std::uniform_int_distribution<> radiusDis(400, 899);
			safe_radius_max = radiusDis(gen);
			safe_radius_min = 0.5 * safe_radius_max;
		}
		if (stage == 3) {
			std::uniform_int_distribution<> radiusDis(300,699);
			safe_radius_max = radiusDis(gen);
			safe_radius_min = 0.5 * safe_radius_max;
		}
	}

	void moveToSafe(Player&paimon) {
		int dx = safecenter_x - position.x - 0.5 * frame_width;
		int dy = safecenter_y - position.y - 0.5 * frame_height;
		double distance = sqrt(dx * dx + dy * dy);
		if (distance != 0) {
			double normalized_x = dx / distance;
			double normalized_y = dy / distance;
			position.x += (int)speed * normalized_x;
			position.y += (int)speed * normalized_y;
		}
		if (paimon.GetPosition().y + 0.5 * paimon.frame_height - position.x - 0.5 * frame_width < 0) {
			facing_left = false;
		}
		else if (paimon.GetPosition().y + 0.5 * paimon.frame_height - position.x - 0.5 * frame_width >= 0) {
			facing_left = true;
		}
	}

	// 生成新的斜向移动方向
	void generateNewDiagonalDirection() {
		std::uniform_real_distribution<> angleDis(0, 2 * M_PI);
		double angle = angleDis(gen);

		// 将角度转换为方向向量
		current_dir_x = cos(angle);
		current_dir_y = sin(angle);
	}
	void randomMove(Player& paimon) {
		DWORD current_time = GetTickCount();
		if (current_time >= next_direction_change_time) {
			// 时间到了，重新生成斜向随机方向
			generateNewDiagonalDirection();
			next_direction_change_time = current_time + direction_change_interval;
		}

		// 计算 Boss 到玩家的方向向量
		POINT playerPos = paimon.GetPosition();
		int dx = playerPos.x + 0.5 * paimon.frame_width - (position.x + 0.5 * frame_width);
		int dy = playerPos.y + 0.5 * paimon.frame_height - (position.y + 0.5 * frame_height);
		double playerDirLen = sqrt(dx * dx + dy * dy);
		if (playerDirLen != 0) {
			double playerDirX = dx / playerDirLen;
			double playerDirY = dy / playerDirLen;

			// 融合随机方向和玩家方向
			const double blendFactor = 0.3; // 融合因子，可调整
			current_dir_x = (double)(current_dir_x * (1 - blendFactor) + playerDirX * blendFactor);
			current_dir_y = (double)(current_dir_y * (1 - blendFactor) + playerDirY * blendFactor);

			// 重新归一化方向向量
			double len = sqrt(current_dir_x * current_dir_x + current_dir_y * current_dir_y);
			if (len != 0) {
				current_dir_x /= len;
				current_dir_y /= len;
			}
		}
		// 检查是否靠近地图边缘
		int new_x = position.x + (int)(speed * current_dir_x);
		int new_y = position.y + (int)(speed * current_dir_y);

		if (new_x <= 0.3*frame_width) {
			current_dir_x = abs(current_dir_x); // 若靠近左边缘，改变 x 方向为右
		}
		else if (new_x + frame_width >= window_width-1.5*frame_width) {
			current_dir_x = -abs(current_dir_x); // 若靠近右边缘，改变 x 方向为左
		}

		if (new_y <= 0.3*frame_height) {
			current_dir_y = abs(current_dir_y); // 若靠近上边缘，改变 y 方向为下
		}
		else if (new_y + frame_height >= window_height-1.5*frame_height) {
			current_dir_y = -abs(current_dir_y); // 若靠近下边缘，改变 y 方向为上
		}

		position.x += (int)(speed * current_dir_x);
		position.y += (int)(speed * current_dir_y);

	}
	void move(Player& paimon) {
		DWORD currentTime = GetTickCount();
		if (currentTime - lastSafetyCheckTime >= 1500) { // 每1.5秒检查一次安全范围
			lastSafetyCheckTime = currentTime;
			if (!isInSafe(paimon)) {
				generateSafe(paimon); // 重新生成安全范围
			}
		}
		if (isInSafe(paimon)) {
			randomMove(paimon); // 在安全范围内随机移动
		}
		else {
			moveToSafe(paimon); // 不在安全范围内，移动到安全范围
		}
		if (position.x + 0.5 * frame_width < paimon.GetPosition().x + 0.5 * paimon.frame_width) {
			facing_left = true;
		}
		else if (position.x + 0.5 * frame_width >= paimon.GetPosition().x + 0.5 * paimon.frame_width) {
			facing_left = false;
		}
	}
	//共有技能
	void Blood1(Player& paimon) {
		DWORD current_time = GetTickCount();
		if (current_time - is_Blood1_end > 8000 + rand() % 3000 && current_time - skillReleaseTime_all >= skill_interval_all) {
			is_blood1_anim_playing = true;
			skillReleaseTime_all = GetTickCount();
		}
		if (is_blood1_anim_playing) {
			if (!visited_blood1 && position.y + 0.5 * frame_height + 175 >= paimon.GetPosition().y + 0.5 * paimon.frame_height && position.y + 0.5 * frame_height - 175 <= paimon.GetPosition().y + 0.5 * paimon.frame_height) {
				is_Blood1_start = GetTickCount();
				blood1_x_start = position.x + 0.5 * frame_width;
				blood1_y_start = position.y + 0.5 * frame_height;
				if (facing_left == false) {
					blood1_direction = -1;
				}
				else {
					blood1_direction = 1;
				}
				visited_blood1 = true;
			}
			DWORD delta_time = GetTickCount() - is_Blood1_start;
			int blood1_x = blood1_x_start + delta_time * 0.4 * blood1_direction;
			if (blood1_direction == 1) {
				blood1->play(blood1_x, blood1_y_start, 20, 1);
			}
			else {
				blood1->play(blood1_x, blood1_y_start, 20, -1);
			}
			bool is_overlap_x = paimon.GetPosition().x + 0.5 * paimon.frame_width >= blood1_x && paimon.GetPosition().x + 0.5 * paimon.frame_width <= blood1_x + 150;
			bool is_overlap_y = paimon.GetPosition().y + 0.5 * paimon.frame_height >= blood1_y_start && paimon.GetPosition().y + 0.5 * paimon.frame_height <= blood1_y_start + 150;
			if (is_overlap_x && is_overlap_y && GetTickCount() - paimon.lastblood1hit >= 750) {
				if (protectivevalue >= 40) {
					protectivevalue -= 40;
				}
				else {
					current_hp_player -= 40 - protectivevalue;
					protectivevalue = 0;
				}
				paimon.lastblood1hit = current_time;
			}
			if (blood1_x + 150 <= 0 || blood1_x >= window_width) {
				is_blood1_anim_playing = false;
				skillReleaseTime_all = GetTickCount();
				is_Blood1_end = GetTickCount();
				visited_blood1 = false;
				blood1->reset();
			}
		}
	}
	void Blood2(Player& paimon) {
		DWORD current_time = GetTickCount();
		if (current_time - is_Blood2_end > 10000+rand()%4000 && current_time - skillReleaseTime_all >= skill_interval_all) {
			is_blood2_anim_playing = true;
			skillReleaseTime_all = GetTickCount();
		}
		if (is_blood2_anim_playing) {
			if (!visited_blood2 && position.y + 0.5 * frame_height + 400 >= paimon.GetPosition().y + 0.5 * paimon.frame_height
				&& position.y + 0.5 * frame_height - 400 <= paimon.GetPosition().y + 0.5 * paimon.frame_height
				&& position.x + 0.5 * frame_width - 400 <= paimon.GetPosition().x + 0.5 * paimon.frame_width
				&& position.x + 0.5 * frame_width + 400 >= paimon.GetPosition().x + 0.5 * paimon.frame_width) {
				is_Blood2_start = GetTickCount();
				visited_blood2 = true;
			}

			DWORD delta_time = GetTickCount() - is_Blood2_start;
			;
			double dx = paimon.GetPosition().x + 0.5 * paimon.frame_width - position.x - 0.5 * frame_width;
			double dy = paimon.GetPosition().y + 0.5 * paimon.frame_height - position.y - 0.5 * frame_height;
			double distance = sqrt(dx * dx + dy * dy);
			if (delta_time < 1000) {
				blood2->play(position.x + 0.5 * frame_width-75, position.y + 0.5 * frame_height-75, 30, 1);
				if (distance <= 150 && GetTickCount() - paimon.lastblood2hit >= 1000) {
					if (protectivevalue >= 45) {
						protectivevalue -= 45;
					}
					else {
						current_hp_player -= 45 - protectivevalue;
						protectivevalue = 0;
					}
					paimon.lastblood2hit = GetTickCount();
				}
			}
			if (4000 >= delta_time && delta_time >= 1000) {
				// 保存当前线条颜色
				COLORREF originalLineColor = getlinecolor();
				// 保存当前线条样式
				LINESTYLE originalLineStyle;
				getlinestyle(&originalLineStyle);

				setlinecolor(RGB(139, 0, 0));
				setlinestyle(PS_SOLID, 3);
				circle(position.x + 0.5 * frame_width, position.y + 0.5 * frame_height, ((delta_time - 1000) % 1000) * 0.4);
				// 恢复原来的线条颜色
				setlinecolor(originalLineColor);
				// 恢复原来的线条样式
				setlinestyle(&originalLineStyle);
				if (distance <= ((delta_time - 1000) % 1000) * 0.4 + 10 && distance >= ((delta_time - 1000) % 1000) * 0.4 - 10 && GetTickCount() - paimon.lastblood2hit2 >= 500) {
					if (protectivevalue >= 20) {
						protectivevalue -= 20;
					}
					else {
						current_hp_player -= 20 - protectivevalue;
						protectivevalue = 0;
					}
					paimon.lastblood2hit2 = GetTickCount();
				}
			}
			if (delta_time > 4000) {
				is_blood2_anim_playing = false;
				is_Blood2_end = GetTickCount();
				visited_blood2 = false;;
				blood2->reset();
			}
		}
	}
	void Blood3(Player& paimon) {
		DWORD current_time = GetTickCount();
		if (current_time - is_Blood3_end > 10000 +rand()%2000 && current_time - skillReleaseTime_all >= skill_interval_all) {
			is_blood3_anim_playing = true;
			skillReleaseTime_all = GetTickCount();
		}
		if (is_blood3_anim_playing) {
			if (!visited_blood3) {
				is_Blood3_start = GetTickCount();
				blood3_x_start_left = -75;
				blood3_x_start_right = window_width;
				blood3_y_start = paimon.GetPosition().y;
				visited_blood3 = true;
			}
			DWORD delta_time = GetTickCount() - is_Blood3_start;
			if (delta_time < 1000) {
				blood2->play(position.x + 0.1 * frame_width, position.y + 0.1 * frame_height, 45, 1);
			}
			if (delta_time >= 1000) {
				int blood3_x_left = blood3_x_start_left +   (delta_time - 1000);
				int blood3_x_right = blood3_x_start_right - (delta_time - 1000);
				if (blood3_x_left <= window_width && blood3_x_right >= -75)
				{
					blood3->play(blood3_x_left, blood3_y_start - 50, 20, 1);
					blood3->play(blood3_x_right, blood3_y_start - 50, 20, -1);

					bool is_overlap_x = blood3_x_left <= paimon.GetPosition().x + 0.5 * paimon.frame_width && blood3_x_left + 100 >= paimon.GetPosition().x + 0.5 * paimon.frame_width;
					bool is_overlap_y = blood3_y_start <= paimon.GetPosition().y + 0.5 * paimon.GetPosition().y && blood3_y_start + 100 >= paimon.GetPosition().y + 0.5 * paimon.frame_height;
					if (is_overlap_x && is_overlap_y && GetTickCount() - paimon.lastblood3hit >= 500) {
						if (protectivevalue >= 35) {
							protectivevalue -= 35;
						}
						else {
							current_hp_player -= 35 - protectivevalue;
							protectivevalue = 0;
						}
						paimon.lastblood3hit = GetTickCount();
					}
				}
				if (blood3_x_left > window_width || blood3_x_right < -75) {
					is_blood3_anim_playing = false;
					is_Blood3_end = GetTickCount();
					skillReleaseTime_all = GetTickCount();
					visited_blood3 = false;
					blood3->reset();
				}
			}
		}
	}
	void Bullet1(Player& paimon) {
		DWORD current_time = GetTickCount();
		if (current_time - is_Bullet_end > 4000 + rand() % 1000 && current_time - skillReleaseTime_all >= skill_interval_all) {
			is_bullet_anim_playing = true;
			skillReleaseTime_all = GetTickCount();
		}
		if (is_bullet_anim_playing) {
			if (!visited_bullet && position.y + 0.5 * frame_height + 150 >= paimon.GetPosition().y + 0.5 * paimon.frame_height && position.y + 0.5 * frame_height - 150 <= paimon.GetPosition().y + 0.5 * paimon.frame_height) {
				is_Bullet_start = GetTickCount();
				bullet_x_start = position.x + 0.5 * frame_width;
				bullet_y_start = position.y + 0.5 * frame_height;
				if (facing_left == false) {
					bullet_direction = -1;
				}
				else {
					bullet_direction = 1;
				}
				visited_bullet = true;
			}
			DWORD delta_time = GetTickCount() - is_Bullet_start;
			int bullet_x = bullet_x_start + delta_time * 0.4 * bullet_direction;
			if (bullet_direction == 1) {
				bullet->play(bullet_x, bullet_y_start, 45, 1);
				if (bullet_x >= bullet_x_start + 150) {
					bullet->play(bullet_x-100, bullet_y_start, 45, 1);
				}
			}
			else {
				bullet->play(bullet_x, bullet_y_start, 45, 2);
				if (bullet_x <= bullet_x_start - 150) {
					bullet->play(bullet_x +100 , bullet_y_start, 45, -1);
				}
			}

			bool is_overlap_x = paimon.GetPosition().x + 0.5 * paimon.frame_width >= bullet_x && paimon.GetPosition().x + 0.5 * paimon.frame_width <= bullet_x + 76;
			bool is_overlap_y = paimon.GetPosition().y + 0.5 * paimon.frame_height >= bullet_y_start && paimon.GetPosition().y + 0.5 * paimon.frame_height <= bullet_y_start + 30;
			bool is_overlap_x1 = 0;
			if (bullet_direction == 1) {
				 is_overlap_x1 = paimon.GetPosition().x + 0.5 * paimon.frame_width >= bullet_x-150 && paimon.GetPosition().x + 0.5 * paimon.frame_width <= bullet_x + 76-150;
			}
			else {
				 is_overlap_x1 = paimon.GetPosition().x + 0.5 * paimon.frame_width >= bullet_x+150 && paimon.GetPosition().x + 0.5 * paimon.frame_width <= bullet_x + 76+150;
			}
			if (is_overlap_x && is_overlap_y && GetTickCount() - paimon.lastbullethit >= 300) {
				if (protectivevalue >= 30) {
					protectivevalue -= 30;
				}
				else {
					current_hp_player -= 30 - protectivevalue;
					protectivevalue = 0;
				}
				paimon.lastbullethit = current_time;
			}
			if (is_overlap_x1 && is_overlap_y && GetTickCount() - paimon.lastbullethit >= 300) {
				if (protectivevalue >= 30) {
					protectivevalue -= 30;
				}
				else {
					current_hp_player -= 30 - protectivevalue;
					protectivevalue = 0;
				}
				paimon.lastbullethit = current_time;
			}
			if (bullet_x + 76+150 <= 0 || bullet_x >= window_width+150) {
				is_bullet_anim_playing = false;
				skillReleaseTime_all = GetTickCount();
				is_Bullet_end = GetTickCount();
				visited_bullet = false;
				bullet->reset();
			}
		}
	}
	bool canBeHit() {
		DWORD currentTime = GetTickCount();
		return currentTime - lastbullethit >= 500;
	}
	void setLastHitTime() {
		lastbullethit = GetTickCount();
	}
	void fireball_hit(Player& paimon) {
		int fireball_radius = 50;
		double distance = sqrt((position.x + 0.5 * frame_width - paimon.fireball_x + fireball_radius) * (position.x + 0.5 * frame_width - paimon.fireball_x + fireball_radius) +
			(position.y + 0.5 * frame_height - paimon.fireball_y + fireball_radius) * (position.y + 0.5 * frame_height - paimon.fireball_y + fireball_radius));
		if (paimon.is_fireball_anim_playing) {
			DWORD current_time = GetTickCount();
			if (distance <= fireball_radius && current_time - lastfireballhit >= 270) {
				current_hp -= min(20, current_hp);
				lastfireballhit = current_time;
			}
		}
	}
	void explode_hit(Player& paimon) {
		const POINT& paimon_position = paimon.GetPosition();
		int explosion_radius = 192;
		double distance = sqrt(((position.x + 0.5 * frame_width) - (paimon_position.x + 0.5 * paimon.frame_width)) * ((position.x + 0.5 * frame_width) - (paimon_position.x + 0.5 * paimon.frame_width)) +
			((position.y + 0.5 * frame_height) - (paimon_position.y + 0.5 * paimon.frame_height)) * ((position.y + 0.5 * frame_height) - (paimon_position.y + 0.5 * paimon.frame_height)));
		if (paimon.is_explode_anim_playing) {
			DWORD current_time = GetTickCount();
			if (distance <= 0.5 * explosion_radius && current_time - lastexplodehit >= 1000) {
				current_hp -= min(15, current_hp);
				current_hp_player += min(15, paimon.max_length - current_hp_player);
				lastexplodehit = GetTickCount();
			}
		}
	}
	void fireflush_hit(Player & paimon) {
		POINT check_position = { position.x + 0.5 * frame_width,position.y + 0.5 * frame_height };
		int fireflush_width = 70;
		int fireflush_height = 70;
		bool is_overlap_x = check_position.x >= paimon.fireflush_x && check_position.x <= paimon.fireflush_x + fireflush_width;
		bool is_overlap_y = check_position.y >= paimon.fireflush_y && check_position.y <= paimon.fireflush_y + fireflush_height;
		DWORD current_time = GetTickCount();
		if (is_overlap_x && is_overlap_y && current_time - lastfireflushhit >= 500) {
			current_hp -= 45;
			lastfireflushhit = current_time;
		}
	}
	void bullet_hit(const Bullet& bullet) {
		bool is_overlap_x = bullet.position.x > position.x && bullet.position.x <= position.x + frame_width;
		bool is_overlap_y = bullet.position.y > position.y && bullet.position.y <= position.y + frame_height;

		if (is_overlap_x && is_overlap_y && canBeHit()) {
			current_hp -= 0.5*bullet.power;
			setLastHitTime();
		}
	}
	void draw(int delta) {
		//boss上方小血条
		setlinecolor(RGB(0, 0, 0));
		rectangle(position.x + 0.5 * (frame_width - 0.07 * max_hp), position.y - 5, position.x + 0.5 * (frame_width + 0.07 * max_hp), position.y - 2);

		if (current_hp < max_hp) {//损失血条
			setfillcolor(RGB(255, 193, 193));
			fillrectangle(position.x + 0.5 * (frame_width - 0.07 * max_hp) + 0.07 * current_hp, position.y - 5, position.x + 0.5 * (frame_width + 0.07 * max_hp), position.y - 2);
		}

		//实时血条
		setfillcolor(RGB(238, 44, 44));
		fillrectangle(position.x + 0.5 * (frame_width - 0.07 * max_hp), position.y - 5, position.x + 0.5 * (frame_width - 0.07 * max_hp) + 0.07 * current_hp, position.y - 2);

		//屏幕上方大血条
		setlinecolor(RGB(0, 0, 0));
		rectangle(0.5 * (window_width - 0.55*max_hp), 90, 0.5 * (window_width + 0.55*max_hp), 100);

		if (current_hp < max_hp) {//损失血条
			setfillcolor(RGB(255, 193, 193));
			fillrectangle(0.5 * (window_width - 0.55*max_hp) + 0.55*current_hp, 90, 0.5 * (window_width +0.55*max_hp), 100);
		}

		//实时血条
		setfillcolor(RGB(238, 44, 44));
		fillrectangle(0.5 * (window_width - 0.55*max_hp), 90, 0.5 * (window_width - 0.55*max_hp) + 0.55*current_hp, 100);

		int shadow_pos_x = position.x + (frame_width - shadow_width) / 2;
		int shadow_pos_y = position.y + frame_height - 10;

		putimage_alpha(shadow_pos_x, shadow_pos_y, &boss_shadow);


		if (facing_left) {
			boss->play(position.x, position.y, 45, 1);
		}
		else {
			boss->play(position.x, position.y, 45, -1);
		}
	}
public:

	bool facing_left = false;
	DWORD lastFacingChangeTime = 0;  // 记录上次改变 facing_left 的时间
	const DWORD facingChangeInterval = 750;

	POINT position = { 800,360 };
	int frame_width;
	int frame_height;
	int shadow_width = 32;

	int current_hp = 1500;
	int max_hp = 1500;
private:
	int stage = 1;
	int speed = 2;
};


class Boss1 : public Boss {
public:
	Boss1(LPCTSTR path, int width, int height, int sequence,int Max_hp,int Current_hp) : Boss(path, width, height, sequence,Max_hp,Current_hp) {
	}
	// 析构函数
	~Boss1() override {
	}
	void RandomlyReleaseSkill(Player& paimon) {

		if (!IsAnySkillPlaying()&&GetTickCount()-skillReleaseTime_all>=skill_interval_all)
		{
			std::uniform_int_distribution<> dis(1, 4);
			skillChoice = dis(gen);
		}
		switch (skillChoice) {
		case 1:
			Blood1(paimon);
			break;
		case 2:case 3:
			Blood2(paimon);
			break;
		case 4:
			Blood3(paimon);
			break;
		}
	}
};

class Boss2 :public Boss {
private:
	Animation_new* redexplode;
	Animation_new* redeye;
	Animation_new* lightning;
	Animation_new* blue;
	Animation_new* yellow;
	Animation_new* whitestar;
	Animation_new* yellowstar;
	Animation_new* shine;
private:
	bool is_redexplode_anim_playing = false;
	bool is_redexplode = false;
	DWORD is_redexplode_start = 0;
	DWORD is_redexplode_end = 0;
	bool visited_redexplode = false;

	bool is_redeye_anim_playing = false;
	bool is_redeye = false;
	DWORD is_redeye_start = 0;
	bool visited_redeye = false;

	bool is_lightning_anim_playing = false;
	bool is_lightning = false;
	DWORD is_lightning_start = 0;
	DWORD is_lightning_end = 0;

	bool visited_lighten = false;//这个指闪电的判断
	bool visited_lightning= false;

	bool is_random_anim_playing = false;
	bool is_random = false;
	DWORD is_random_start = 0;
	DWORD is_random_end = 0;
	bool visited_random = false;

	bool visited_random1 = false;
	bool visited_randmo2 = false;


private:
	//记录技能释放时坐标：
	int redeye_x_start = 0;
	int redeye_y_start = 0;

	int lighten_x = 0;
	int lighten_y = 0;

	int random_x1 = 0;
	int random_x2 = 0;
	int random_y1 = 0;
	int random_y2 = 0;
public:
	//LPCTSTR path, int x, int y, int width, int height, int interval,int z
	Boss2(LPCTSTR path, int width, int height, int sequence,int Max_hp,int Current_hp) : Boss(path, width, height, sequence,Max_hp,Current_hp) {
		frame_width = width;
		frame_height = height;
		redexplode = new Animation_new(_T("boss2/redexplode.png"), 5, 7, 200, 200, 45, 1);
		redeye = new Animation_new(_T("boss2/redeye.png"), 5, 8, 200, 200,180, 1);
		lightning = new Animation_new(_T("boss2/lightening.png"), 5, 6, 192, 192, 100,1);
		blue = new Animation_new(_T("boss2/blue.png"), 6, 2, 100, 175, 45, 1);
		yellow = new Animation_new(_T("boss2/yellow.png"), 6, 2, 100, 175, 45, 1);
		whitestar = new Animation_new(_T("boss2/whitestar.png"), 5, 2, 192, 192, 45, 1);
		yellowstar = new Animation_new(_T("boss2/yellowstar.png"), 5, 2, 192, 192, 45, 1);
		shine = new Animation_new(_T("boss2/shine.png"), 5, 2, 192, 192, 45, 1);
	}
	~Boss2() override {
		delete redexplode;
		delete redeye;
		delete blue;
		delete yellow;
		delete lightning;
		delete whitestar;
		delete yellowstar;
		delete shine;
	}
	void Redexplode(Player& paimon) {
		DWORD current_time = GetTickCount();
		if (current_time - is_redexplode_end > 8000 + rand() % 4000 && current_time - skillReleaseTime_all >= skill_interval_all) {
			is_redexplode_anim_playing = true;
			is_redeye_anim_playing = true;
			skillReleaseTime_all = GetTickCount();
		}
		if (is_redexplode_anim_playing &&is_redeye_anim_playing) {
			if (!visited_redexplode&& position.y + 0.5 * frame_height + 400 >= paimon.GetPosition().y + 0.5 * paimon.frame_height
				&& position.y + 0.5 * frame_height - 400 <= paimon.GetPosition().y + 0.5 * paimon.frame_height
				&& position.x + 0.5 * frame_width - 400 <= paimon.GetPosition().x + 0.5 * paimon.frame_width
				&& position.x + 0.5 * frame_width + 400 >= paimon.GetPosition().x + 0.5 * paimon.frame_width) {
				is_redexplode_start = GetTickCount();
				is_redeye_start = GetTickCount();
				visited_redexplode = true;
			}

			DWORD delta_time = GetTickCount() - is_redexplode_start;
			;
			double dx = paimon.GetPosition().x + 0.5 * paimon.frame_width - position.x - 0.5 * frame_width;
			double dy = paimon.GetPosition().y + 0.5 * paimon.frame_height - position.y - 0.5 * frame_height;
			double distance = sqrt(dx * dx + dy * dy);
			if (delta_time < 2000) {
				redexplode->play(position.x+0.5*frame_width-100 , position.y+0.5*frame_height-100, 45, 1);
				if (distance <= 150 && GetTickCount() - paimon.lastredexplodehit >= 1000) {
					if (protectivevalue >= 30) {
						protectivevalue -= 30;
					}
					else {
						current_hp_player -= 30 - protectivevalue;
						protectivevalue = 0;
					}
					paimon.lastredexplodehit = GetTickCount();
				}
			}

			if (!visited_redeye&&delta_time>=2000) {
				redeye_x_start = paimon.GetPosition().x + 0.5 * paimon.frame_width;//中心点坐标
				redeye_y_start = paimon.GetPosition().y + 0.5 * paimon.frame_height;
				visited_redeye = true;
			}
			double dx1 = paimon.GetPosition().x + 0.5 * paimon.frame_width - redeye_x_start;
			double dy1 = paimon.GetPosition().y + 0.5 * paimon.frame_height - redeye_y_start;

			double distance1 = sqrt(dx1 * dx1 + dy1 * dy1);

			if (2050 <= delta_time && delta_time <= 3050) {
				redeye->play(redeye_x_start-100, redeye_y_start-100,15, 1);
				if (distance1 <= 100 && GetTickCount() - paimon.lastredeyehit >= 500) {
					if (protectivevalue >= 15) {
						protectivevalue -= 15;
					}
					else {
						current_hp_player -= 15 - protectivevalue;
						protectivevalue = 0;
					}
					paimon.lastredeyehit = GetTickCount();
				}
			}
			else if(delta_time >3050){
				redexplode->reset();
				is_redexplode_anim_playing = false;
				skillReleaseTime_all = GetTickCount();
				is_redexplode_end = GetTickCount();
				visited_redexplode = false;

				redeye->reset();
				is_redeye_anim_playing = false;
				visited_redeye = false;
			}
		}
	}
	void Lightning(Player&paimon) {
		DWORD current_time = GetTickCount();
		if (current_time - is_lightning_end > 8000 + rand() % 2000 && current_time - skillReleaseTime_all >= skill_interval_all) {
			is_lightning_anim_playing = true;
			skillReleaseTime_all = GetTickCount();
		}
		if (is_lightning_anim_playing ) {
			if (!visited_lightning && position.y + 0.5 * frame_height + 500 >= paimon.GetPosition().y + 0.5 * paimon.frame_height
				&& position.y + 0.5 * frame_height - 500 <= paimon.GetPosition().y + 0.5 * paimon.frame_height
				&& position.x + 0.5 * frame_width - 500 <= paimon.GetPosition().x + 0.5 * paimon.frame_width
				&& position.x + 0.5 * frame_width + 500 >= paimon.GetPosition().x + 0.5 * paimon.frame_width) {
				is_lightning_start = GetTickCount();
				visited_lightning = true;
			}

			DWORD delta_time = GetTickCount() - is_lightning_start;
			if (delta_time>1000&&delta_time<1100) {
				lighten_x = paimon.GetPosition().x + 0.5 * paimon.frame_width - 50;
				lighten_y = paimon.GetPosition().y + 0.5 * paimon.frame_height - 80;
			}
			else if (delta_time>2000&&delta_time<2100) {
				lighten_x = paimon.GetPosition().x + 0.5 * paimon.frame_width - 50;
				lighten_y = paimon.GetPosition().y + 0.5 * paimon.frame_height - 80;
			}
			else if (delta_time>3000&&delta_time<3100) {
				lighten_x = paimon.GetPosition().x + 0.5 * paimon.frame_width - 50;
				lighten_y = paimon.GetPosition().y + 0.5 * paimon.frame_height - 80;
			}
			else if (delta_time>4000&&delta_time<4100) {
				lighten_x = paimon.GetPosition().x + 0.5 * paimon.frame_width - 50;
				lighten_y = paimon.GetPosition().y + 0.5 * paimon.frame_height - 80;
			}
			double dx = paimon.GetPosition().x + 0.5 * paimon.frame_width - position.x - 0.5 * frame_width;
			double dy = paimon.GetPosition().y + 0.5 * paimon.frame_height - position.y - 0.5 * frame_height;
			double distance = sqrt(dx * dx + dy * dy);

			bool is_overlap_x = paimon.GetPosition().x + 0.5 * paimon.frame_width >= lighten_x && paimon.GetPosition().x + 0.5 * paimon.frame_height <= lighten_x + 100;
			bool is_overlap_y = paimon.GetPosition().y + 0.5 * paimon.frame_height >= lighten_y && paimon.GetPosition().y + 0.5 * paimon.frame_width <= lighten_y+ 175;

			if (delta_time  <= 1000) {
				lightning->play(position.x + 0.5 * frame_width - 96, position.y + 0.5 * frame_height - 96, 20, 1);
				if (delta_time >= 500) {
					if (distance <= 100&&GetTickCount() - paimon.lastlightninghit >= 300) {
						if (protectivevalue >= 15) {
							protectivevalue -= 15;
						}
						else {
							current_hp_player -= 15 - protectivevalue;
							protectivevalue = 0;
						}
						paimon.lastlightninghit = GetTickCount();
					}
				}
			}
			else if (1610>=delta_time&& delta_time >= 1110) {
				blue->play(lighten_x, lighten_y, 30, 1);
				if (is_overlap_x && is_overlap_y&&GetTickCount()-paimon.lastbluehit>=700) {
					if (protectivevalue >= 15) {
						protectivevalue -= 15;
					}
					else {
						current_hp_player -= 15 - protectivevalue;
						protectivevalue = 0;
					}
					paimon.lastbluehit = GetTickCount();
				}
			}
			else if (2610 >= delta_time && delta_time >= 2100) {
				yellow->play(lighten_x, lighten_y, 30, 1);
				if (is_overlap_x && is_overlap_y && GetTickCount() - paimon.lastbluehit >= 700) {
					if (protectivevalue >= 15) {
						protectivevalue -= 15;
					}
					else {
						current_hp_player -= 15 - protectivevalue;
						protectivevalue = 0;
					}
					paimon.lastbluehit = GetTickCount();
				}
			}
			else if (3610 >= delta_time && delta_time >= 3100) {
				blue->play(lighten_x , lighten_y, 30, 1);
				if (is_overlap_x && is_overlap_y && GetTickCount() - paimon.lastbluehit >= 700) {
					if (protectivevalue >= 15) {
						protectivevalue -= 15;
					}
					else {
						current_hp_player -= 15 - protectivevalue;
						protectivevalue = 0;
					}
					paimon.lastbluehit = GetTickCount();
				}
			}
			else if (4610 >= delta_time && delta_time >= 4100) {
				yellow->play(lighten_x,lighten_y, 30, 1);
				if (is_overlap_x && is_overlap_y && GetTickCount() - paimon.lastbluehit >= 700) {
					if (protectivevalue >= 15) {
						protectivevalue -= 15;
					}
					else {
						current_hp_player -= 15 - protectivevalue;
						protectivevalue = 0;
					}
					paimon.lastbluehit = GetTickCount();
				}
			}
			else if (delta_time > 5000) {
				is_lightning_anim_playing = false;
				lightning->reset();
				blue->reset();
				yellow->reset();
				visited_lightning = false;
				is_lightning_end = GetTickCount();
			}
		}
	}
	void Random_explode(Player & paimon) {
		DWORD current_time = GetTickCount();
		if (current_time - is_random_end > 8000 + rand() % 4000 && current_time - skillReleaseTime_all >= skill_interval_all) {
			is_random_anim_playing = true;
			skillReleaseTime_all = GetTickCount();
		}
		if (is_random_anim_playing) {
			if (!visited_random && position.y + 0.5 * frame_height + 400 >= paimon.GetPosition().y + 0.5 * paimon.frame_height
				&& position.y + 0.5 * frame_height - 400 <= paimon.GetPosition().y + 0.5 * paimon.frame_height
				&& position.x + 0.5 * frame_width - 400 <= paimon.GetPosition().x + 0.5 * paimon.frame_width
				&& position.x + 0.5 * frame_width + 400 >= paimon.GetPosition().x + 0.5 * paimon.frame_width) {
				is_random_start = GetTickCount();
				visited_random = true;
			}
			DWORD delta_time = GetTickCount() - is_random_start;//其实可以简化为一个for循环但为了简单易懂
			if (delta_time <= 2000) {
				shine->play(position.x + 0.5 * frame_width - 96, position.y + 0.5 * frame_height - 96, 20, 1);
			}
			if (delta_time >= 1000 && delta_time <= 1100) {
				random_x1 = max(rand() % 192 + paimon.GetPosition().x - 200, 20);
				random_x2 = min(rand() % 192 + paimon.GetPosition().x + 200, window_width - 210);
				random_y1 = max(rand() % 192 + paimon.GetPosition().y - 200, 20);
				random_y2 = min(rand() % 192 + paimon.GetPosition().y + 200, window_height - 210);
			}
			if (delta_time >= 3000 && delta_time <= 3100) {
				random_x1 = max(rand() % 192 + paimon.GetPosition().x - 200, 20);
				random_x2 = min(rand() % 192 + paimon.GetPosition().x + 200, window_width - 210);
				random_y1 = max(rand() % 192 + paimon.GetPosition().y - 200, 20);
				random_y2 = min(rand() % 192 + paimon.GetPosition().y + 200, window_height - 210);
			}
			if (delta_time >= 5000 && delta_time <= 5100) {
				random_x1 = max(rand() % 192 + paimon.GetPosition().x - 200, 20);
				random_x2 = min(rand() % 192 + paimon.GetPosition().x + 200, window_width - 210);
				random_y1 = max(rand() % 192 + paimon.GetPosition().y - 200, 20);
				random_y2 = min(rand() % 192 + paimon.GetPosition().y + 200, window_height - 210);
			}
			if (delta_time >= 7000 && delta_time <= 7100) {
				random_x1 = max(rand() % 192 + paimon.GetPosition().x - 200, 20);
				random_x2 = min(rand() % 192 + paimon.GetPosition().x + 200, window_width - 210);
				random_y1 = max(rand() % 192 + paimon.GetPosition().y - 200, 20);
				random_y2 = min(rand() % 192 + paimon.GetPosition().y + 200, window_height - 210);
			}
			double dx1 = paimon.GetPosition().x + 0.5 * paimon.frame_width - random_x1 - 96;
			double dy1 = paimon.GetPosition().y + 0.5 * paimon.frame_height - random_y1-96;

			double dx2 = paimon.GetPosition().x + 0.5 * paimon.frame_width - random_x2 - 96;
			double dy2 = paimon.GetPosition().y + 0.5 * paimon.frame_height - random_y2 - 96;

			double distance1 = sqrt(dx1 * dx1 + dy1 * dy1);
			double distance2 = sqrt(dx2 * dx2 + dy2 * dy2);
			if (delta_time > 1110&&delta_time<=2110) {
				whitestar->play(random_x1, random_y1, 20, 1);
				yellowstar->play(random_x2, random_y2, 20, 1);
				if ((distance1 <= 100 || distance2 <= 100)&& GetTickCount() - paimon.lastrandomhit >= 500&&delta_time>=1150) {
					if (protectivevalue >= 20) {
						protectivevalue -= 20;
					}
					else {
						current_hp_player -= 20 - protectivevalue;
						protectivevalue = 0;
					}
					paimon.lastrandomhit = GetTickCount();
				}
			}
			else if (delta_time > 3110 && delta_time <= 4110) {
				yellowstar->play(random_x1, random_y1, 20, 1);
				whitestar->play(random_x2, random_y2, 20, 1);
				if ((distance1 <= 100 || distance2 <= 100) && GetTickCount() - paimon.lastrandomhit >= 500&&delta_time>=3150) {
					if (protectivevalue >= 20) {
						protectivevalue -= 20;
					}
					else {
						current_hp_player -= 20 - protectivevalue;
						protectivevalue = 0;
					}
					paimon.lastrandomhit = GetTickCount();
				}
			}
			else if (delta_time > 5100 && delta_time <= 6110) {
				yellowstar->play(random_x2, random_y2, 20, 1);
				whitestar->play(random_x1, random_y1, 20, 1);
				if ((distance1 <= 100 || distance2 <= 100) && GetTickCount() - paimon.lastrandomhit >= 500&&delta_time>=5150) {
					if (protectivevalue >= 20) {
						protectivevalue -= 20;
					}
					else {
						current_hp_player -= 20 - protectivevalue;
						protectivevalue = 0;
					}
					paimon.lastrandomhit = GetTickCount();
				}
			}
			else if (delta_time > 7110 && delta_time <= 8110) {
				whitestar->play(random_x2, random_y2, 20, 1);
				yellowstar->play(random_x1, random_y1, 20, 1);
				if ((distance1 <= 100 || distance2 <= 100) && GetTickCount() - paimon.lastrandomhit >= 500 && delta_time >= 7150) {
					if (protectivevalue >= 20) {
						protectivevalue -= 20;
					}
					else {
						current_hp_player -= 20 - protectivevalue;
						protectivevalue = 0;
					}
					paimon.lastrandomhit = GetTickCount();
				}
			}
			else if (delta_time > 8200) {
				yellowstar->reset();
				whitestar->reset();
				is_random_anim_playing = false;
				shine->reset();
				visited_random = false;
				is_random_end = GetTickCount();
				skillReleaseTime_all = GetTickCount();
			}
		}
	}
	void RandomlyReleaseSkill(Player& paimon) {

		if (!IsAnySkillPlaying()&& GetTickCount() - skillReleaseTime_all>= skill_interval_all)
		{
			std::uniform_int_distribution<> dis(1, 20);
			skillChoice = dis(gen);
		}
		switch (skillChoice) {
		case 1: case 2: case 3:case 4:case 5:
			Redexplode(paimon);
			break;
		case 6:
			Blood1(paimon);
			break;
		case 7: 
			Blood2(paimon);
			break;
		case 8:
			Blood3(paimon);
			break;
		case 9: case 10:case 11:case 12: case 13:case 14:
			Lightning(paimon);
			break;
		case 15:case 16:case 17:case 18:case 19:case 20:
			Random_explode(paimon);
			break;
		default:break;
		}
	}
};
class Boss3:public Boss {
private:
	Animation_new* pull;
	Animation_new* explode;
	Animation_new* explode_purpule;
	Animation_new* colorexplode;
	Animation_new* skill1;
	Animation_new* skill2;
	Animation_new* skill3;
	Animation_new* skill4;
	Animation_new* skill5;
public:
	bool is_purpule_anim_playing = false;
	bool is_purpule = false;
	DWORD is_purpule_start = 0;
	DWORD is_purpule_end = 0;
	bool visited_purpule = false;
private:
	int pos_x = 500;
	int pos_y = 500;
	int direction_ball = 1;
	DWORD purpule_delta_time = 0;
	DWORD current = 0;
	vector<POINT*>ball;
	bool visited_ball = false;
	bool is_over_x[12];
	bool is_over_y[12];
	bool index_ball_delete = true;

	bool is_overlap_x[12];
	bool is_overlap_y[12];
public:
	Boss3(LPCTSTR path, int width, int height, int sequence,int Max_hp,int Current_hp) : Boss(path, width, height, sequence,Max_hp,Current_hp){
		pull = new Animation_new(_T("boss3/pull.png"), 5, 8, 200, 200, 100, 1);
		explode = new Animation_new(_T("boss3/explode.png"), 5, 8, 192, 192, 100, 1);
		explode_purpule = new Animation_new(_T("boss3/explode_purpule.png"), 5, 5, 200, 200, 90, 1);
		colorexplode = new Animation_new(_T("boss3/colorexplode.png"), 5, 8, 192, 192, 45, 1);
		skill1 = new Animation_new(_T("boss3/skill1.png"), 7, 1, 43, 50, 45, 1);
		skill2 = new Animation_new(_T("boss3/skill2.png"), 7, 1, 120, 140, 45, 1);
		skill3 = new Animation_new(_T("boss3/skill3.png"), 7, 1, 120, 140, 45, 1);
		skill4 = new Animation_new(_T("boss3/skill4.png"), 7, 1, 120, 140, 45, 1);
		skill5 = new Animation_new(_T("boss3/skill5.png"), 7, 1, 120, 140, 45, 1);
	}
	~Boss3() override {
		delete pull;
		delete explode;
		delete explode_purpule;
		delete colorexplode;
		delete skill1;
		delete skill2;
		delete skill3;
		delete skill4;
		delete skill5;
	}
	void purpule(Player& paimon) {
		DWORD current_time = GetTickCount();
		if (current_time - is_purpule_end > 8000 + rand() % 2000 && current_time - skillReleaseTime_all >= skill_interval_all) {
			is_purpule_anim_playing = true;
			skillReleaseTime_all = GetTickCount();
		}
		if (is_purpule_anim_playing) {
			if (!visited_purpule && position.y + 0.5 * frame_height + 500 >= paimon.GetPosition().y + 0.5 * paimon.frame_height
				&& position.y + 0.5 * frame_height - 500 <= paimon.GetPosition().y + 0.5 * paimon.frame_height
				&& position.x + 0.5 * frame_width - 500 <= paimon.GetPosition().x + 0.5 * paimon.frame_width
				&& position.x + 0.5 * frame_width + 500 >= paimon.GetPosition().x + 0.5 * paimon.frame_width) {
				is_purpule_start = GetTickCount();
				pos_x = position.x;
				pos_y = position.y;
				direction_ball = paimon.GetPosition().x > position.x ? 1 : -1;
				visited_purpule = true;
			}
			purpule_delta_time = current_time - is_purpule_start;
			if (purpule_delta_time <=3450 ) {
				explode_purpule->play(position.x + 0.5 * frame_width - 100,position.y+0.5*frame_width-100,45,1);
			}
		}
	}
	void Skill1(Player& paimon) {  //三连球  //当进入2阶段之后 变为5连球
		if (is_purpule_anim_playing) {
			if (!visited_ball) {
				for (auto i = 0; i < 12; i++) {
					POINT* point1 = new POINT;   //记得delete！！
					point1->x = pos_x + 0.5 * frame_width;
					point1->y = pos_y + 0.5 * frame_height;
					ball.push_back(point1);
					current = GetTickCount();
					visited_ball = true;
				}
			}
			if (purpule_delta_time >= 1500) {
				int order = (purpule_delta_time - 1500) / 375;
				DWORD delta = GetTickCount() - current;
				if (order > 0) {
					for (int i = 0; i < 3; i++) {
						ball[i]->x += 0.005 * delta * cos(M_PI/6 * (i - 1)) * direction_ball;
						ball[i]->y += 0.005 * delta * sin(M_PI/6 * (i - 1)) * direction_ball;
						skill1->play(ball[i]->x, ball[i]->y, 45, direction_ball);
					}
				}
				if (order > 1) {
					for (int i = 3; i < 6; i++) {
						ball[i]->x += 0.005 * (delta-375) * cos(M_PI/6 * (i - 1)) * direction_ball;
						ball[i]->y += 0.005 * (delta-375) * sin(M_PI/6 * (i - 1)) * direction_ball;
						skill1->play(ball[i]->x, ball[i]->y, 45, direction_ball);
					}
				}
				if (order > 2) {
					for (int i = 6; i < 9; i++) {
						ball[i]->x += 0.005 * (delta-750) * cos(M_PI/6 * (i - 1)) * direction_ball;
						ball[i]->y += 0.005 * (delta-750) * sin(M_PI/6* (i - 1)) * direction_ball;
						skill1->play(ball[i]->x, ball[i]->y, 45, direction_ball);
					}
				}
				if (order > 3) {
					for (int i = 9; i < 12; i++) {
						ball[i]->x += 0.005 * (delta - 1125) * cos(M_PI/6 * (i - 1)) * direction_ball;
						ball[i]->y += 0.005 * (delta - 1125) * sin(M_PI/6 * (i - 1)) * direction_ball;
						skill1->play(ball[i]->x, ball[i]->y, 45, direction_ball);
					}
				}
				for (int i = 0; i < 12; i++) {
					is_overlap_x[i] = paimon.GetPosition().x >= ball[i]->x && paimon.GetPosition().x <= ball[i]->x + 43;
					is_overlap_y[i]=paimon.GetPosition().y>=ball[i]->y&&paimon.GetPosition().y<=ba

				}
				if (delta >= 4000) {
					for (int i = 0; i < 12; i++) {
						delete ball[i];
					}
					ball.clear();
					is_purpule_anim_playing = false;
					is_purpule_end = GetTickCount();
					skillReleaseTime_all = GetTickCount();
					explode_purpule->reset();
					skill1->reset();
					visited_ball = false;
					visited_purpule = false;
				}
			}
		}
	}
	void RandomlyReleaseSkill(Player& paimon) {

		if (!IsAnySkillPlaying() && GetTickCount() - skillReleaseTime_all >= skill_interval_all)
		{
			std::uniform_int_distribution<> dis(1, 5);
			skillChoice = dis(gen);
		}
		switch (skillChoice) {
		case 1: case 2: case 3:case 4:case 5:
			purpule(paimon);
			Skill1(paimon);
		default:break;
		}
	}
};

class Ghost : public Boss{
private:
	Animation_new* ghost;
public:
	DWORD lastGenerate = 0;
	DWORD startGenerate = 0;
public:
	int direction = 1;
	bool visited_position = false;
	bool visited_generate = false;
	bool visited_lastgenerate = false;

	int frame_width = 120;
	int frame_height = 133;

public:
	Ghost(LPCTSTR path, int width, int height, int sequence,int Max_hp,int Current_hp) : Boss(path, width, height, sequence,Max_hp,Current_hp){
		ghost = new Animation_new(_T("boss3/ghost1.png"), 4, 2,120,133,45, 0);
	}
	~Ghost() {
		delete ghost;
	}
	void drawghost(Player& paimon, Boss3* boss3) {
		if (!visited_position) {
			int x1 = 2 * paimon.GetPosition().x + paimon.frame_width - boss3->position.x - 0.5 * boss3->frame_width;
			int y1 = 2 * paimon.GetPosition().y + paimon.frame_height - boss3->position.y - 0.5 * boss3->frame_height;
			bool is_x_bigger = paimon.GetPosition().x > boss3->position.x;
			bool is_y_bigger = paimon.GetPosition().y > boss3->position.y;
			if (is_x_bigger && !is_y_bigger) {
				position.x = min(x1, window_width - 200) - 0.5 * frame_width;
				position.y = max(y1, 200) - 0.5 * frame_height;
			}
			else if (is_x_bigger && is_y_bigger) {
				position.x = min(x1, window_width - 200) - 0.5 * frame_width;
				position.y = min(y1, window_height - 200) - 0.5 * frame_height;
			}
			else if (!is_x_bigger && !is_y_bigger) {
				position.x = max(x1, 200) - 0.5 * frame_width;
				position.y = max(y1, 200) - 0.5 * frame_height;
			}
			else if (!is_x_bigger&&is_y_bigger) {
				position.x = max(x1, 200) - frame_width * 0.5;
				position.y = min(y1, window_height - 200) - frame_height * 0.5;
			}
			startGenerate = GetTickCount();
			visited_position = true;
		}
		DWORD current_time = GetTickCount();
		DWORD delta_time = current_time - startGenerate;
		direction = position.x > paimon.GetPosition().x ? -1 : 1;
		if (current_hp > 0) {
			should_generate_ghost = false;
		}
		else if (current_hp <= 0) {
			if (!visited_lastgenerate) {
				lastGenerate = GetTickCount();
			}
			if (GetTickCount() - lastGenerate >= 1000 + rand() % 2000)
			{
				should_generate_ghost = true;
				visited_lastgenerate = false;
				visited_position = false;
			}
		}
		if (current_hp>0) {
			ghost->play(position.x, position.y, 45, direction);
		}

		//头顶血条
		setlinecolor(RGB(0, 0, 0));
		rectangle(position.x + 0.5 * (frame_width - 0.4*max_hp), position.y - 5, position.x + 0.5 * (frame_width + 0.4 * max_hp), position.y - 2);

		if (current_hp < max_hp) {//损失血条
			setfillcolor(RGB(255, 193, 193));
			fillrectangle(position.x + 0.5 * (frame_width - 0.4 * max_hp) + 0.4 * current_hp, position.y - 5, position.x + 0.5 * (frame_width + 0.4 * max_hp), position.y - 2);
		}

		//实时血条
		setfillcolor(RGB(238, 44, 44));
		fillrectangle(position.x + 0.5 * (frame_width - 0.4 * max_hp), position.y - 5, position.x + 0.5 * (frame_width - 0.4 * max_hp) + 0.4 * current_hp, position.y - 2);
	}
	void RandomlyReleaseSkill(Player& paimon) {

		if (!IsAnySkillPlaying() && GetTickCount() - skillReleaseTime_all >= skill_interval_all+4000)
		{
			std::uniform_int_distribution<> dis(1, 6);
			skillChoice = dis(gen);
		}
		switch (skillChoice) {
		case 1: case 4:
			Blood1(paimon);
			break;
		case 2:
			Blood2(paimon);
			break;
		case 3:case 5:case 6:
			Blood3(paimon);
			break;
		}
	}
};
void generateboar(vector<Enemy*>& enemy_list,int x) {
	static int interval = 80;
	if (0<= x && x< 20) {
		interval = 80;
	}
	if (20 <= x && x<50) {
		interval = 72;
	}
	if (50 <= x  && x < 75) {
		interval = 65;
	}
	if (75 <= x &&x<100) {
		interval = 60;
	}
	if (x >= 100) {
		interval = 150;
	}
	static int counter = 0;
	if ((++counter) % interval == 0)
		enemy_list.push_back(new Enemy());
}
void generateghost(vector<Ghost*>& ghost_list){
	if (should_generate_ghost) {
		Ghost* ghost = new Ghost(_T("boss%d/boss%d.png"), 120,133, 3,200,200);
		ghost_list.push_back(ghost);
	}
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

	initgraph(1280, 720);


	int score = 0;

	ExMessage msg;

	IMAGE img_menu;
	IMAGE image_background;

	Player paimon;
	Boss1* boss1 = new Boss1(_T("boss%d/boss%d.png"), 170, 188, 1,1250,1250);
	Boss2* boss2 = new Boss2(_T("boss%d/boss%d.png"), 180, 162, 2,1500,1500);
	Boss3* boss3 = new Boss3(_T("boss%d/boss%d.png"), 175, 221, 3,1500,1500);

	vector<Enemy*> boar_list;
	vector<Bullet>bullet_list;
	vector<Ghost*>ghost_list;

	//绘制按钮界面
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
			{
				paimon.pocessevent(msg);
				paimon.setLeftClickPosition(msg.x, msg.y);
				if (msg.message == WM_LBUTTONDBLCLK) {
					paimon.setLeftClickPosition(msg.x, msg.y);
				}
			}
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
			paimon.Getscore(score);
			paimon.Max_energy(score);
			paimon.new_protective(score);
			paimon.Max_protective(score);
			paimon.current_hp_recovery();
			paimon.current_energy_recovery();
			paimon.current_protective_recovery();

			if (boss1&&score>=100) {
				boss1->move(paimon);
			}
			if (boss2&&!boss1) {
				boss2->move(paimon);
			}
			if (boss3&&!boss3->is_purpule_anim_playing) {
				boss3->move(paimon);
			}
			if (!ghost_list.empty()) {
				for (Ghost* ghost : ghost_list) {
					ghost->move(paimon);
				}
			}
			updatebullets(bullet_list, paimon, score);
			generateboar(boar_list, score);
			generateghost(ghost_list);

			for (Enemy* boar : boar_list) {
				boar->speed_up(paimon, score);
				boar->move(paimon);
				boar->current_hp(score);
				boar->max_hp(score);
				boar->updateattack(score);
				boar->checkplayercollision(paimon);
			}
			
			bool check_score_index = 0;
			for (Enemy* boar : boar_list) {
				for (const Bullet& bullet : bullet_list) {
					boar->checkbulletcollision(bullet);
				}
				boar->checkfireballcollision(paimon);
				boar->checkexplodecollision(paimon);
				boar->checkfireflush(paimon);
				if (boar->current_hp_boar <= 0) {
					check_score_index = 1;
				}
				if (check_score_index) {
					score++;
					check_score_index = 0;
				}
			}
			if (boss1&&score>=100) {
				for (const Bullet& bullet : bullet_list) {
					boss1->bullet_hit(bullet);
				}
				boss1->explode_hit(paimon);
				boss1->fireball_hit(paimon);
				boss1->fireflush_hit(paimon);

				// 检查 boss1 血量是否为 0，如果是则删除
				if (boss1->current_hp <= 0) {
					delete boss1;
					boss1 = nullptr;
					score += 50;
				}
			}
			if (boss2&&!boss1) {
				for (const Bullet& bullet : bullet_list) {
					boss2->bullet_hit(bullet);
				}
				boss2->explode_hit(paimon);
				boss2->fireball_hit(paimon);
				boss2->fireflush_hit(paimon);
				// 检查 boss2 血量是否为 0，如果是则删除
				if (boss2->current_hp <= 0) {
					delete boss2;
					boss2 = nullptr;
					score += 50;
				}
			}
			if (!ghost_list.empty()) {
				for (Ghost* ghost : ghost_list) {
					for (const Bullet& bullet : bullet_list) {
						ghost->bullet_hit(bullet);
					}
					ghost->explode_hit(paimon);
					ghost->fireball_hit(paimon);
					ghost->fireflush_hit(paimon);
				}
			}
			if (boss3) {
				for (const Bullet& bullet : bullet_list) {
					boss3->bullet_hit(bullet);
				}
				boss3->explode_hit(paimon);
				boss3->fireball_hit(paimon);
				boss3->fireflush_hit(paimon);
				// 检查 boss3 血量是否为 0，如果是则删除
				if (boss3->current_hp <= 0) {
					delete boss3;
					boss3 = nullptr;
					score += 50;
				}
			}

		}
		cleardevice();
		if (is_game_started) {
			putimage(0, 0, &image_background);
			for (Enemy* boar : boar_list) {
				boar->draw(1000 / 144);
			}
			if (!ghost_list.empty()) {
				for (Ghost* ghost : ghost_list) {
					ghost->drawghost(paimon, boss3);
					ghost->RandomlyReleaseSkill(paimon);
				}
			}
			for (const Bullet& bullet : bullet_list) {
				bullet.draw();
			}
			if (boss1&&score>=100) {
				boss1->draw(1000 / 144);
				boss1->RandomlyReleaseSkill(paimon);
				boss1->Bullet1(paimon);
			}
			if (boss2&&!boss1) {
				boss2->draw(1000 / 144);
				boss2->RandomlyReleaseSkill(paimon);
				boss2->Bullet1(paimon);
			}
			if (boss3) {
				boss3->draw(1000 / 144);
				boss3->RandomlyReleaseSkill(paimon);
				boss3->Bullet1(paimon);
			}
			paimon.draw_hp(1000 / 144);
			paimon.draw_fireball();
			paimon.draw(1000 / 144);

			for (size_t i = 0; i < boar_list.size(); i++) {
				Enemy* boar = boar_list[i];
				if (!boar->checkalive()) {
					swap(boar_list[i], boar_list.back());
					boar_list.pop_back();
					delete boar;
				}
			}
			if (!ghost_list.empty()) {
				for (size_t i = 0; i < ghost_list.size(); i++) {
					Ghost* ghost = ghost_list[i];
					if (ghost->current_hp <= 0) {
						ghost_list.pop_back();
						delete ghost;
					}
				}
			}
			drawscore(score);
			if (paimon.paimon_is_dead && current_hp_player <= 0) {
				static TCHAR text[128];
				_stprintf_s(text, _T("最终得分：%d!"), score);
				MessageBox(GetHWnd(), text, _T("游戏结束"), MB_OK);
				running = false;
			}
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
	for (Enemy* boar : boar_list) {
		delete boar;
	}
	if (boss1) {
		delete boss1;
	}
	if (boss2) {
		delete boss2;
	}
	if (boss3) {
		delete boss3;
	}
	for(Ghost* ghost : ghost_list) {
		delete ghost;
	}
	return 0;

}