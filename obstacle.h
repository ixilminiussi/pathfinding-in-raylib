#pragma once

#include "raylib.h"
#include <vector>

class Obstacle {
public:
	Obstacle(const Rectangle& rectangleP);
	~Obstacle() = default;

	void render();

	bool overlaps(const Vector2& point, const float& range);

	Vector2 getCenter();

	static std::vector<Obstacle *> obstacles;

private:
	Rectangle rectangle;
};

