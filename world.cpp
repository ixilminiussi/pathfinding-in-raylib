#include "world.h"
#include "tile.h"
#include "raylib.h"
#include "namespaces.h"
#include <utility>
#include <algorithm>
#include <vector>

World* World::instance = nullptr;

World* World::getInstance() {
	if (instance == nullptr) {
		instance = new World();
	}
	return instance;
}
World::World() {
	typedef Tile* TileP;

	tiles = new TileP[game::WIDTH * game::HEIGHT];

	for (int x = 0; x < game::WIDTH; x++) {
		for (int y = 0; y < game::HEIGHT; y++) {
			tiles[y * game::WIDTH + x] = new Tile(
				Rectangle {
				(float)(x * game::TILE_SIZE),
				(float)(y * game::TILE_SIZE),
				(float)(game::TILE_SIZE),
				(float)(game::TILE_SIZE)
				},
				x, y
			);
		}
	}
}

void World::render() const {
	for (int i = 0; i < game::WIDTH * game::HEIGHT; i++) {
		tiles[i]->render();
	}
}

void World::paintArea(const Vector2& center, const float& radius, Type type) {
	for (int x = 0; x < game::WIDTH; x++) {
		for (int y = 0; y < game::HEIGHT; y++) {
			if (CheckCollisionCircleRec(center, radius, tiles[y * game::WIDTH + x]->getRectangle())) {
				tiles[y * game::WIDTH + x]->setType(type);

				for (int rx = -1; rx <= 1; rx++) {
					for (int ry = -1; ry <= 1; ry++) {
						if (x + rx >= 0 && x + rx < game::WIDTH &&
							y + ry >= 0 && y + ry < game::HEIGHT) {

							tiles[(y + ry) * game::WIDTH + (x + rx)]->updatePosition();
						}
					}
				}
			}
		}
	}
}

Vector2 World::getWorldAddress(const Vector2& P) const {
	return { P.x / game::TILE_SIZE, P.y / game::TILE_SIZE };
}

bool World::lineValidation(const Vector2& A, const Vector2& B) {
	Vector2 Ap = getWorldAddress(A);
	Vector2 Bp = getWorldAddress(B);
	int x1 = Ap.x;
	int x2 = Bp.x;
	int y1 = Ap.y;
	int y2 = Bp.y;
	
	// BRESENHAM algorithm to find all squares in the way
	// find all at : https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm#:~:text=Bresenham's%20line%20algorithm%20is%20a%20line%20drawing%20algorithm
	int dx = x2 - x1;
	int dy = y2 - y1;
	int yi = 1;
	if (dy < 0) {
		yi = -1;
		dy = -dy;
	}
	int D = (2 * dy) - dx;
	int y = y1;

	for (int x = x1; x < x2; x++) {
		setType(x, y, Type::WALL);
		if (D > 0) {
			y = y + yi;
			D = D + (2 * (dy - dx));
		}
		else {
			D += 2 * dy;
		}
	}

	return true;
}

Type World::getType(int x, int y) const {
	if (x < 0) return Type::WALL;
	if (x >= game::WIDTH) return Type::WALL;
	if (y < 0) return Type::WALL;
	if (y >= game::HEIGHT) return Type::WALL;

	return tiles[y * game::WIDTH + x]->getType();
}

void World::setType(int x, int y, Type type) {
	if ((x < 0) || (x > game::WIDTH) || (y < 0) || (y > game::HEIGHT)) return;

	tiles[y * game::WIDTH + x]->setType(type);
}

const Rectangle& World::getRectangle(int x, int y) const {
	if (x < 0) return { 
		-(float)game::TILE_SIZE, 
		(float)(y * game::TILE_SIZE), 
		(float)game::TILE_SIZE, 
		(float)game::TILE_SIZE
	};
	if (x >= game::WIDTH) return { 
		(float)screen::WIDTH, 
		(float)(y * game::TILE_SIZE), 
		(float)game::TILE_SIZE, 
		(float)game::TILE_SIZE
	};
	if (y < 0) return { 
		(float)(x * game::TILE_SIZE), 
		-(float)game::TILE_SIZE, 
		(float)game::TILE_SIZE, 
		(float)game::TILE_SIZE
	};
	if (y >= game::HEIGHT) return { 
		(float)(x * game::TILE_SIZE), 
		(float)game::HEIGHT, 
		(float)game::TILE_SIZE, 
		(float)game::TILE_SIZE
	};

	return tiles[y * game::WIDTH + x]->getRectangle();
}
