#pragma once 

#include "raylib.h"
#include <vector>

class Boid;

class Cell
{
public:
	Cell() = default;
	/* Creates a new cell, adds it to the static list of cells
	* @params
	* - rectangle: physical area covered by cell
	* - x, y: positional id of given cell, used to find neighbours
	*/
	Cell(const Rectangle& rectangle, int x, int y);

	/* Removes cell from static list of cells */
	~Cell();

	/* Renders cell, for debugging purposes only */
	void render(const Color& color);

	void update();

	/* Finds neighbouring cells (including self) and adds to neighbour list */
	void findNeighbours();

	/* Check if a given boid is in, if so, adds it to its 
	* children list and removes it from previous cell's */
    void isInOrOut(Boid* boid);

	/* Removes boid from children list */
	void removeChild(Boid* b);

	static std::vector<Cell*> cells; // list of all cells

	// Getters
	const std::vector<Cell*>& getNeighbours();
	const std::vector<Boid*>& getChildren();
	const int getX();
	const int getY();

private:
	Rectangle rectangle;

	std::vector<Cell*> neighbours = std::vector<Cell*>();
	std::vector<Boid*> children = std::vector<Boid*>();

	int x, y;
};

