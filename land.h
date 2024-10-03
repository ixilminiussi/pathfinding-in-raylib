#pragma once
#include "raylib.h"

namespace Type {
	const int R = 1; // Road
	const int P = 2; // Park
	const int H = 3; // House
	const int W = 4; // Wall
};

class Land
{
public:
	Land();
	Land(const Rectangle& rectangle, int type);
	~Land() = default;

	void render() const;

	const Rectangle rectangle;
	const int type;
	const int ID;

private:
	static int IDsource;
};

