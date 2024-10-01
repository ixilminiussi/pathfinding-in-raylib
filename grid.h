#pragma once 

#include "raylib.h"
#include "include/boid.h"
#include <vector>

class Grid
{
public:
	Grid() = default;
	Grid(const Rectangle& rectangle, int x, int y);
	~Grid();

	void render();
	void findNeighbours();
    void isInOrOut(Boid* boid);

	void removeChild(Boid* b);

	static std::vector<Grid*> grids;
	std::vector<Grid*> neighbours = std::vector<Grid*>();
	std::vector<Boid*> children = std::vector<Boid*>();

	int x, y;

private:
	Rectangle rectangle;
};

