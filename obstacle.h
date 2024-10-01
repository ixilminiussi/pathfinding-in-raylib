#pragma once

#include "raylib.h"
#include <vector>

class Obstacle {
public:
	/* Creates a new obstacle, adds it to the static  list of obstacles
	* @params
	* - rectangle: physical area covered by obstacle
	* @optional
	* - cantDie: whether the use can delete obstacle (false by default)
	*/
	Obstacle(const Rectangle& rectangle, bool cantDie = false);

	/* Removes obstacle from static list of obstacles */
	~Obstacle();

	/* Renders obstacle */
	void render();

	/* Destroys obstacle if he can die */
	void kill();

	/* Checks if obstacle overlaps with point */
	bool overlaps(const Vector2& point, const float& range);

	/* Returns center of rectangle */
	Vector2 getCenter();

	static std::vector<Obstacle *> obstacles; // static list of obstacles

private:
	Rectangle rectangle;
	bool cantDie;
};

