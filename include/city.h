#pragma once
class City
{
public:
	City(int* grid, int width, int height);
	~City() = default;

	void render();

	const int width, height;
private:
	class Land*** grid;
};

