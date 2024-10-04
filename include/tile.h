#pragma once
#include "raylib.h"

enum Type {
	WALL,
	FLOOR
};

enum Position {
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_RIGHT,
	OTHER
};

class Tile
{
public:
	Tile(const Rectangle& rectangle, int x, int y, Type type = Type::FLOOR);
	~Tile() = default;

	void render() const;
	void updatePosition();
	void setType(Type type);
	Type getType() const;
	const Rectangle& getRectangle() const;

private:
	Type type;
	Position position;
	int ix, iy;
	Rectangle rectangle;
};

