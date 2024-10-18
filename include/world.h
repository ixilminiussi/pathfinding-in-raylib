#pragma once
#include "raylib.h"
#include "tile.h"

class World
{
  public:
    static World *getInstance();

    ~World() = default;

    World(const World &w) = delete;
    World operator=(const World &w) = delete;

    void render() const;
    void paintArea(const Vector2 &center, const float &radius, enum TileCategory category);
    Vector2 getWorldAddress(const Vector2 &P) const;

    bool lineValidation(const Vector2 &A, const Vector2 &B, bool narrow = false) const;

    enum TileCategory getTileCategory(int x, int y) const;
    void setTileCategory(int x, int y, TileCategory category);
    void setNavigable(int x, int y, bool state);

    const Tile *getTile(int x, int y) const;

    class Rectangle getRectangle(int x, int y) const;

    bool updateFlag;

  private:
    World();
    static World *instance;
    class Tile **tiles;
};
