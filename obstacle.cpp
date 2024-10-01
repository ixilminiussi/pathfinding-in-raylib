#include "obstacle.h"

std::vector<Obstacle*> Obstacle::obstacles = std::vector<Obstacle*>();

Obstacle::Obstacle(const Rectangle& rectangleP, bool cantDieP) : rectangle(rectangleP), cantDie(cantDieP) {
	obstacles.push_back(this);
}


Obstacle::~Obstacle() {
    obstacles.erase(std::remove(obstacles.begin(), obstacles.end(), this), obstacles.end());
}

void Obstacle::render() {
	DrawRectangleRec(rectangle, BLACK);
	DrawRectangleLinesEx(rectangle, 1.0f, WHITE);
}

void Obstacle::kill() {
	if (!cantDie) {
		delete this;
	}
}

bool Obstacle::overlaps(const Vector2& point, const float& range) {
	return CheckCollisionCircleRec(point, range, rectangle);
}

Vector2 Obstacle::getCenter() {
	return Vector2{ rectangle.x + rectangle.width / 2.0f, rectangle.y + rectangle.height / 2.0f };
}
