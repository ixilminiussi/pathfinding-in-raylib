#include "obstacle.h"

std::vector<Obstacle*> Obstacle::obstacles = std::vector<Obstacle*>();

Obstacle::Obstacle(const Rectangle& rectangleP) : rectangle(rectangleP) {
	obstacles.push_back(this);
}

void Obstacle::render() {
	DrawRectangleRoundedLines(rectangle, 0.1f, 5, 1.0f, WHITE);
	DrawRectangleRounded(rectangle, 0.1f, 5, BLACK);
}

bool Obstacle::overlaps(const Vector2& point, const float& range) {
	return CheckCollisionCircleRec(point, range, rectangle);
}

Vector2 Obstacle::getCenter() {
	return Vector2{ rectangle.x + rectangle.width / 2.0f, rectangle.y + rectangle.height / 2.0f };
}
