#pragma once
#include "raylib.h"

enum TileCategory
{
    WALL,
    FLOOR
};

enum Placement
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

    /**
     * @brief draws tile on screen
     */
    void render() const;
    /**
     * @brief updates the placement of tile relative to its neighbours
     */
    void updatePlacement();

    void setNavigable(bool state);
    void setTileCategory(TileCategory category); //@brief updates the placement
    bool isNavigable() const;
    TileCategory getTileCategory() const;
    const Rectangle &getRectangle() const;

  private:
    TileCategory category;
    Placement placement;
    int ix, iy;
    Rectangle rectangle;
    bool navigable;
};
