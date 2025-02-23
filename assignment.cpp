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
int protectivevalue = 0;

bool is_game_started = false;
bool running = true;



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
	Animation_new(LPCTSTR path, int x, int y, int width, int height, int interval,int z){//int leftToRight) {  //参数讲解： x为横向个数，y为纵向个数，width，height分别为单个的宽高，interval为帧率间隔，z是判断是否需要处理黑色背景
		interval_ms = interval;                                                   //leftToRight 代表图片是从左到右处理还是从上倒下处理，若为1则从左到右，若为2则为从上到下

		for (int i = 0; i < y; ++i) {
			for (int j = 0; j < x; ++j) {
				IMAGE * frame = new IMAGE();
				loadimage_alpha(frame, path, j * width, i * height, width, height);
				if (z == 1) {
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
		if (dir == 2) {
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

private://处理CD问题与每秒回复问题
	DWORD lastTime = 0;

	DWORD is_shift_end = 0;
	bool is_shift_anim_playing = false;  // 记录 shift 动画是否正在播放
	DWORD shift_anim_start_time = 0;     // 记录 shift 动画开始的时间

	DWORD is_fireball_end = 0;
	bool is_fireball_anim_playing = false;  //记录fireball是否正在播放
	DWORD fireball_anim_start_time = 0;    //记录fireball 开始时间

	DWORD is_explode_end = 0;
	bool is_explode_anim_playing = false;  //记录fireball是否正在播放
	DWORD explode_anim_start_time = 0;    //记录fireball 开始时间

	DWORD is_fireflush_end = 0;
	bool is_fireflush_anim_playing = false;
	DWORD fireflush_anim_start_time = 0;
	bool fireflush_animation_ended = false;

	DWORD lastShieldRecoveryTime = 0;
	const DWORD shieldRecoveryInterval = 1000;
	const int shieldRecoveryAmount = 2;

	DWORD lastEnergyRecoveryTime = 0;
	const DWORD energyRecoveryInterval = 1000;
	const int energyRecoveryAmount = 2;
	
	DWORD lastHpRecoveryTime = 0;
	const DWORD hpRecoveryInterval = 1000;
	const int hpRecoveryAmount = 2;

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
private:
	int leftClickX = 0;
	int leftClickY = 0;

	int fireball_x = 0;
	int fireball_y = 0;

public:
	Player() {

		loadimage(&player_shadow, _T("images/shadow_player.png"));
		loadimage(&shield, _T("images/bubble.png"));

		anim_left = new Animation(_T("images/paimon_left_%d.png"),6,45);
		fireball = new Animation_new(_T("images/1.png"), 4, 4, 100, 100, 180,0);
		fireflush = new Animation_new(_T("images/3.png"), 5, 5, 70, 70, 45,1);
		firepart = new Animation_new(_T("images/3_part.png"), 5, 2, 70, 70, 45, 1);

		explode = new Animation_new(_T("images/F.png"), 5, 8, 192, 192, 60,1);
		shift = new Animation_new(_T("images/space.png"), 10, 10, 110, 146, 30,0);

	}
	~Player() {
		delete anim_left;
		delete fireball;
		delete fireflush;
		delete explode;
		delete lava;
		delete shift;
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
				if (GetTickCount() - is_fireflush_end > 1000) {
					is_fireflush = true;
					skill2_fireflush();
					is_fireflush_end = GetTickCount();
				}
				break;
			case VK_SPACE:
				if (GetTickCount() - is_shift_end > 3000) {  // 检查冷却时间
					is_shift = true;
					skillspace_shift();
					is_shift_end = GetTickCount();
				}
				break;
			case 'Q':
				skillq_shield(get_score);
				break;
			case 'R':
				if (GetTickCount() - is_explode_end > 10000) {
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
	//每1s回复2点护盾值
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
		if (current_energy >= 25) {
			current_energy -= 25;
		}
		is_fireball_anim_playing = true;
		fireball_anim_start_time = GetTickCount();
		if (getLeftClickX() != 0 && getLeftClickY() != 0) {
			fireball_x = getLeftClickX();
			fireball_y = getLeftClickY();
		}
		fireball->reset();
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
	}
	void skill2_fireflush() {  //技能3	 //耗蓝50  10S	 伤害75点 只可向上下左右方向进行发射
		if (current_energy > 50) {
			current_energy -= 50;
		}	
		is_fireflush_anim_playing = true;
		fireflush_anim_start_time = GetTickCount();
		fireflush->reset();
		fireflush_animation_ended = false;
	}
	void skillr_expolde() {  //技能F  //耗蓝75   CD 15s 在自身周围产生一团爆炸圈 3 次 每次造成50点伤害
		if (current_energy >= 75) {
			current_energy -= 75;
		}
		is_explode_anim_playing = true;
		explode_anim_start_time = GetTickCount();
		explode->reset();
	}
	void skillq_shield(int score) { //技能Q  //额外护盾槽 消耗护盾值50  // CD 1S  增加的护盾可以抵消50点伤害
		if (current_protective >= 50 && protectivevalue <=80) {
			current_protective -= 50;
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
	void draw(int delta) {
		//绘制左上方血条
		setlinecolor(RGB(0, 0, 0));
		rectangle(0, 30, max_length, 40);

		if (current_hp_player < max_length) {
			setfillcolor(RGB(255, 193, 193));
			fillrectangle(current_hp_player, 30, max_length, 40);
		}

		setfillcolor(RGB(238, 44, 44));
		fillrectangle(0, 30, current_hp_player, 40);

		//绘制paimon头顶
		setlinecolor(RGB(0, 0, 0));
		rectangle(position.x+0.5*(frame_width-0.5*max_length), position.y - 14, position.x + 0.5 *(frame_width+0.5* max_length), position.y - 10);

		if (current_hp_player < max_length) {
			setfillcolor(RGB(255, 193, 193));
			fillrectangle(position.x + 0.5 * (frame_width -0.5*max_length)+0.5*current_hp_player, position.y - 14, position.x + 0.5 *(frame_width+0.5* max_length), position.y - 10);
		}

		setfillcolor(RGB(238, 44, 44));
		fillrectangle(position.x+0.5*(frame_width -0.5*max_length), position.y - 14, position.x + 0.5 *(frame_width-0.5*max_length)+0.5*current_hp_player, position.y - 10);
		//血量描述
		static TCHAR text[64];
		swprintf_s(text, _T("当前玩家的血量为:%d / %d"), current_hp_player,max_length);
		setbkmode(TRANSPARENT);
		settextcolor(RGB(255, 85, 185));
		outtextxy(max_length + 10, 27, text);

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
			}
		}
		//绘制fireflush
		if (is_fireflush) {
			DWORD current_time_fireflush = GetTickCount();
			DWORD elapsed_time = current_time_fireflush - fireflush_anim_start_time;
			if(!fireflush_animation_ended) {
				if (elapsed_time < fireflush->getTotalDuration()&&(200 + (current_time_fireflush - fireflush_anim_start_time) * 0.3) < window_width) {
					fireflush->play((200 + (current_time_fireflush - fireflush_anim_start_time) * 0.3), 200, 30, 1);
				}
				else {
					// 动画第一轮播放结束，设置标志位
					fireflush_animation_ended = true;
				}
			}
			if (fireflush_animation_ended) {
				current_time_fireflush = GetTickCount();
				firepart->play((200 + (current_time_fireflush - fireflush_anim_start_time) * 0.3), 200, 30, 1);
			}
			if((200 + (current_time_fireflush - fireflush_anim_start_time) * 0.3) >= window_width) {
				fireflush_animation_ended = false;
			}
		}

		//绘制阴影
		int shadow_pos_x = position.x + (frame_width - shadow_width) / 2;
		int shadow_pos_y = position.y + frame_height - 8;

		putimage_alpha(shadow_pos_x, shadow_pos_y, &player_shadow);


		//绘制人物面左还是面右
		static bool facing_left = false;
		int dir_x = is_move_right - is_move_left;
		if (dir_x > 0) {
			facing_left = false;
		}
		else if (dir_x < 0) {
			facing_left = true;
		}

		DWORD currentTime = GetTickCount();
		if (currentTime - lastTime < 1000) { // 0.5 秒内
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
		//绘制闪现

		DWORD current_time = GetTickCount();
		if (is_shift_anim_playing) {
			if (current_time - shift_anim_start_time < 100 * 6) {
				shift->play(position.x + 0.5 * frame_width - 50, position.y + 0.5 * frame_height - 70, 150, 1);
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
	Animation_new* lava;
	Animation_new* shift;
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
	int max_length = 100;
	int idx_score = 0;

	int get_score = 0;//用于获取主函数的score又不想该score为全局变量

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
		anim_left = new Animation_new(_T("boss1/boss1.png"), 4, 2, frame_width, frame_height, 45,0);
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

			updatebullets(bullet_list, paimon,score);
			generateboar(boar_list,score);

			for (Enemy* boar : boar_list) {
				boar->speed_up(paimon,score);
				boar->move(paimon);
				boar->current_hp(score);
				boar->max_hp(score);
				boar->updateattack(score);
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

			for (Enemy* boar : boar_list) {
				boar->draw(1000 / 144);
			}
			for (const Bullet& bullet : bullet_list) {
				bullet.draw();
			}
			boss1.draw(1000 / 144);

			paimon.draw_fireball();

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