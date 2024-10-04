#include "tile.h"
#include "world.h"
#include "raylib.h"
#include "world.h"
#include "namespaces.h"

Tile::Tile(const Rectangle& rectangle, int x, int y, Type type) :
	rectangle(rectangle),
	type(type),
	position(Position::OTHER),
	ix(x),
	iy(y)
{
}

void Tile::render() const {
	if (type == Type::FLOOR) return;

	switch (position) {
	case TOP_LEFT:
		DrawTriangle(
			{ rectangle.x + rectangle.width, rectangle.y },
			{ rectangle.x, rectangle.y + rectangle.height },
			{ rectangle.x + rectangle.width, rectangle.y + rectangle.height },
			shoshone::orange
		);
		break;
	case TOP_RIGHT:
		DrawTriangle(
			{ rectangle.x, rectangle.y },
			{ rectangle.x, rectangle.y + rectangle.height },
			{ rectangle.x + rectangle.width, rectangle.y + rectangle.height },
			shoshone::orange
		);
		break;
	case BOTTOM_LEFT:
		DrawTriangle(
			{ rectangle.x, rectangle.y },
			{ rectangle.x + rectangle.width, rectangle.y + rectangle.height },
			{ rectangle.x + rectangle.width, rectangle.y },
			shoshone::orange
		);
		break;
	case BOTTOM_RIGHT:
		DrawTriangle(
			{ rectangle.x, rectangle.y },
			{ rectangle.x, rectangle.y + rectangle.height },
			{ rectangle.x + rectangle.width, rectangle.y },
			shoshone::orange
		);
		break;
	case OTHER:
		DrawRectangleRec(rectangle, shoshone::orange);
		break;
	}
}

void Tile::updatePosition() {
	Type types[3][3] = {};
	World* world = World::getInstance();
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			types[x + 1][y + 1] = world->getType(ix + x, iy + y);
		}
	}

	Type W = Type::WALL;
	Type F = Type::FLOOR;

	// TOP_LEFT
	if (types[2][1] == W && types[1][2] == W &&
		types[1][0] == F && types[0][1] == F && 
		types[0][0] == F) {
		position = Position::TOP_LEFT;
		return;
	}
	// TOP_RIGHT
	if (types[0][1] == W && types[1][2] == W &&
		types[1][0] == F && types[2][1] == F &&
		types[2][0] == F) {
		position = Position::TOP_RIGHT;
		return;
	}
	// BOTTOM_LEFT
	if (types[1][0] == W && types[2][1] == W &&
		types[0][1] == F && types[1][2] == F &&
		types[0][2] == F) {
		position = Position::BOTTOM_LEFT;
		return;
	}
	// BOTTOM_RIGHT
	if (types[1][0] == W && types[0][1] == W &&
		types[2][1] == F && types[1][2] == F &&
		types[2][2] == F) {
		position = Position::BOTTOM_RIGHT;
		return;
	}

	position = Position::OTHER;
}

void Tile::setType(Type typeP) {
	type = typeP;

	updatePosition();
}

Type Tile::getType() const {
	return type;
}

const Rectangle& Tile::getRectangle() const {
	return rectangle;
}
