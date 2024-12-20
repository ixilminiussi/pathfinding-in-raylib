#include "tile.h"
#include "namespaces.h"
#include "raylib.h"
#include "world.h"

Tile::Tile(const Rectangle &rectangle, int x, int y, TileCategory category)
    : category(category), placement(Placement::OTHER), ix(x), iy(y), rectangle(rectangle), navigable(true)
{
}

void Tile::render() const
{
    if (category == TileCategory::FLOOR)
        return;

    switch (placement)
    {
    case TOP_LEFT:
        DrawTriangle({rectangle.x + rectangle.width, rectangle.y}, {rectangle.x, rectangle.y + rectangle.height},
                     {rectangle.x + rectangle.width, rectangle.y + rectangle.height}, shoshone::orange);
        break;
    case TOP_RIGHT:
        DrawTriangle({rectangle.x, rectangle.y}, {rectangle.x, rectangle.y + rectangle.height},
                     {rectangle.x + rectangle.width, rectangle.y + rectangle.height}, shoshone::orange);
        break;
    case BOTTOM_LEFT:
        DrawTriangle({rectangle.x, rectangle.y}, {rectangle.x + rectangle.width, rectangle.y + rectangle.height},
                     {rectangle.x + rectangle.width, rectangle.y}, shoshone::orange);
        break;
    case BOTTOM_RIGHT:
        DrawTriangle({rectangle.x, rectangle.y}, {rectangle.x, rectangle.y + rectangle.height},
                     {rectangle.x + rectangle.width, rectangle.y}, shoshone::orange);
        break;
    case OTHER:
        DrawRectangleRec(rectangle, shoshone::orange);
        break;
    }
}

void Tile::updatePlacement()
{
    TileCategory categories[3][3] = {};
    World *world = World::getInstance();

    if (category == TileCategory::WALL)
    {
        navigable = false;

        for (int x = -1; x <= 1; x++)
        {
            for (int y = -1; y <= 1; y++)
            {
                categories[x + 1][y + 1] = world->getTileCategory(ix + x, iy + y);
                if (categories[x + 1][y + 1] == TileCategory::WALL)
                {
                    navigable = false;
                }
                else
                {
                    world->setNavigable(ix + x, iy + y, false);
                }
            }
        }

        TileCategory W = TileCategory::WALL;
        TileCategory F = TileCategory::FLOOR;

        // TOP_LEFT
        if (categories[2][1] == W && categories[1][2] == W && categories[1][0] == F && categories[0][1] == F &&
            categories[0][0] == F)
        {
            placement = Placement::TOP_LEFT;
            return;
        }
        // TOP_RIGHT
        if (categories[0][1] == W && categories[1][2] == W && categories[1][0] == F && categories[2][1] == F &&
            categories[2][0] == F)
        {
            placement = Placement::TOP_RIGHT;
            return;
        }
        // BOTTOM_LEFT
        if (categories[1][0] == W && categories[2][1] == W && categories[0][1] == F && categories[1][2] == F &&
            categories[0][2] == F)
        {
            placement = Placement::BOTTOM_LEFT;
            return;
        }
        // BOTTOM_RIGHT
        if (categories[1][0] == W && categories[0][1] == W && categories[2][1] == F && categories[1][2] == F &&
            categories[2][2] == F)
        {
            placement = Placement::BOTTOM_RIGHT;
            return;
        }

        placement = Placement::OTHER;
    }
    else
    {
        navigable = true;

        for (int x = -1; x <= 1; x++)
        {
            for (int y = -1; y <= 1; y++)
            {
                if (world->getTileCategory(ix + x, iy + y) == TileCategory::WALL)
                {
                    navigable = false;
                }
            }
        }
    }
}

void Tile::setTileCategory(TileCategory categoryP)
{
    category = categoryP;

    updatePlacement();
}

TileCategory Tile::getTileCategory() const
{
    return category;
}

const Rectangle &Tile::getRectangle() const
{
    return rectangle;
}

const bool Tile::isNavigable() const
{
    return navigable;
}

void Tile::setNavigable(bool state)
{
    navigable = state;
}
