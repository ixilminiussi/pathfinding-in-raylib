#include "cell.h"
#include "boid.h"

std::vector<Cell*> Cell::cells = std::vector<Cell*>();

Cell::Cell(const Rectangle& rectangleP, int xP, int yP) :
	rectangle(rectangleP), 
	x(xP), 
	y(yP)
{
    cells.push_back(this);
}

Cell::~Cell() {
    cells.erase(std::remove(cells.begin(), cells.end(), this), cells.end());
}

void Cell::findNeighbours() {
	neighbours = std::vector<Cell*>();
	for (Cell *c : cells) {
		int relX, relY;
		relX = x - c->getX();
		relY = y - c->getY();
		if ((relX == 1 || relX == 0 || relX == -1) &&
			(relY == 1 || relY == 0 || relY == -1)) {
			neighbours.push_back(c);
		}
	}
}

void Cell::render(const Color& color) {
	DrawRectangleLinesEx(rectangle, 1.0f, color);
}

void Cell::isInOrOut(Boid *boid) {
 	if (CheckCollisionPointRec(boid->getPosition(), rectangle)) {
		if (boid->getParent() != this) {
			children.push_back(boid);
			if (boid->getParent() != nullptr) {
				boid->getParent()->removeChild(boid);
			}
			boid->setParent(this);
		}
	}
}

void Cell::removeChild(Boid *b) {
   children.erase(std::remove(children.begin(), children.end(), b), children.end());
}

const std::vector<Cell*>& Cell::getNeighbours() {
	return neighbours;
}

const std::vector<Boid*>& Cell::getChildren() {
	return children;
}

const int Cell::getX() {
	return x;
}

const int Cell::getY() {
	return y;
}