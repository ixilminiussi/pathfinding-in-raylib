#include "land.h"

int Land::IDsource = 0;

Land::Land() :
	rectangle(),
	type(0),
	ID(0)
{}

Land::Land(const Rectangle& rectangle, int type) :
	rectangle(rectangle),
	type(type),
	ID(IDsource++)
{
}

void Land::render() const {
	Color color;

	switch (type) {
	case Type::R:
		color = YELLOW;
		break;
	case Type::H:
		color = RED;
		break;
	case Type::P:
		color = GREEN;
		break;
	case Type::W:
		color = ORANGE;
		break;
	default:
		color = BLACK;
	}

	DrawRectangleRec(rectangle, color);
}
