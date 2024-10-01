#include "grid.h"

std::vector<Grid*> Grid::grids = std::vector<Grid*>();

Grid::Grid(const Rectangle& rectangleP, int xP, int yP) :
	rectangle(rectangleP), 
	x(xP), 
	y(yP)
{
    grids.push_back(this);
}

Grid::~Grid() {
    grids.erase(std::remove(grids.begin(), grids.end(), this), grids.end());
}

void Grid::findNeighbours() {
	neighbours = std::vector<Grid*>();
	for (Grid *g : grids) {
		int relX, relY;
		relX = x - g->x;
		relY = y - g->y;
		if ((relX == 1 || relX == 0 || relX == -1) &&
			(relY == 1 || relY == 0 || relY == -1)) {
			neighbours.push_back(g);
		}
	}
}

void Grid::render() {
	DrawRectangleLinesEx(rectangle, 1.0f, WHITE);
}

void Grid::isInOrOut(Boid *boid) {
 	if (CheckCollisionPointRec(boid->position, rectangle)) {
		if (boid->parent != this) {
			children.push_back(boid);
			if (boid->parent != nullptr) {
				boid->parent->removeChild(boid);
			}
			boid->parent = this;
		}
	}
}

void Grid::removeChild(Boid *b) {
   children.erase(std::remove(children.begin(), children.end(), b), children.end());
}