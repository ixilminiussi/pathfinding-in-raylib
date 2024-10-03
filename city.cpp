#include "city.h"
#include "raylib.h"
#include "land.h"

City::City(int* gridP, int width, int height) :
	width(width),
	height(height) 
{
	grid = new Land ** [width];

	float tileWidth = (float)GetRenderWidth() / (float)width;
	float tileHeight = (float)GetRenderHeight() / (float)height;

	for (int x = 0; x < width; x++) {

		grid[x] = new Land * [height];

		for (int y = 0; y < height; y++) {
			Rectangle rectangle = { x * tileWidth, y * tileHeight, tileWidth, tileHeight };
			int type = gridP[x * height + y];
			grid[x][y] = new Land(rectangle, type);
		}
	}
}

void City::render() 
{
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			grid[x][y]->render();
		}
	}
}
