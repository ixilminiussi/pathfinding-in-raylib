#pragma once
#include "raylib.h"
#include "namespaces.h"

class World
{
public:
	static World* getInstance();

	~World() = default;

	World(const World& w) = delete;
	World operator=(const World& w) = delete;

	void render() const;
	void paintArea(const Vector2& center, const float& radius, enum Type type);
	Vector2 getWorldAddress(const Vector2& P) const;

	bool lineValidation(const Vector2& A, const Vector2& B);

	enum Type getType(int x, int y) const;
	void setType(int x, int y, Type type);

	const class Rectangle& getRectangle(int x, int y) const;

private:
	World();
	static World* instance;
	class Tile** tiles;
};

