#include "world.h"
#include "tile.h"
#include "raylib.h"
#include "namespaces.h"

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

Vector2 World::getWorldAddress(const Vector2& P) {
	return { P.x / game::TILE_SIZE, P.y / game::TILE_SIZE };
}

Type World::getType(int x, int y) {
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
