
#pragma once

#include"vector2.h"
#include"collision_layer.h"

#include<functional>

class CollisionManager;

class CollisionBox {

	friend class CollisionManager;

private:
	vector2 size;
	vector2 position;  //碰撞箱中心点所处位置
	bool enabled = true;
	std::function<void()>on_collide;
	CollisionLayer layer_src = CollisionLayer::None;    //src代表源 source
	CollisionLayer layer_dst = CollisionLayer::None;	//dst代表目的

private:
	CollisionBox() = default;
	~CollisionBox() = default;
public:
	void set_enabled(bool flag) {
		enabled = flag;
	}
	bool get_enabled()const {
		return enabled;
	}
	void set_layer_src(CollisionLayer layer) {
		layer_src = layer;
	}
	void set_layer_dst(CollisionLayer layer) {
		layer_dst = layer;
	}
	void set_on_collide(std::function<void()> on_collide) {
		this->on_collide = on_collide;
	}
	void set_size(const vector2& size) {
		this->size = size;
	}
	const vector2& get_size()const {
		return size;
	}
	void set_position(const vector2& position) {
		this->position = position;
	}
};
