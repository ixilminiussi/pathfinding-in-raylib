#pragma once
#include "raylib.h"

enum TileCategory
{
    WALL,
    FLOOR
};

enum Position
{
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
    OTHER
};

class Tile
{
  public:
    Tile(const Rectangle &rectangle, int x, int y, TileCategory category = TileCategory::FLOOR);
    ~Tile() = default;

    void render() const;
    void updatePosition();
    void setTileCategory(TileCategory category);
    TileCategory getTileCategory() const;
    const Rectangle &getRectangle() const;
    bool isNavigable() const;
    void setNavigable(bool state);

  private:
    TileCategory category;
    Position position;
    int ix, iy;
    Rectangle rectangle;
    bool navigable;
};
