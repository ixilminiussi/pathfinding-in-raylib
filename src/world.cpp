#include "world.h"
#include "namespaces.h"
#include "tile.h"
#include <cmath>
#include <cstdlib>

World *World::instance = nullptr;

World *World::getInstance()
{
    if (instance == nullptr)
    {
        instance = new World();
    }
    return instance;
}

World::World()
{
    typedef Tile *TileP;

    tiles = new TileP[game::WIDTH * game::HEIGHT];

    for (int x = 0; x < game::WIDTH; x++)
    {
        for (int y = 0; y < game::HEIGHT; y++)
        {
            tiles[y * game::WIDTH + x] = new Tile(Rectangle{(float)(x * game::TILE_SIZE), (float)(y * game::TILE_SIZE),
                                                            (float)(game::TILE_SIZE), (float)(game::TILE_SIZE)},
                                                  x, y);
        }
    }
}

void World::render() const
{
    for (int i = 0; i < game::WIDTH * game::HEIGHT; i++)
    {
        tiles[i]->render();
    }
}

void World::paintArea(const Vector2 &center, const float &radius, TileCategory category)
{
    for (int x = 0; x < game::WIDTH; x++)
    {
        for (int y = 0; y < game::HEIGHT; y++)
        {
            if (CheckCollisionCircleRec(center, radius, tiles[y * game::WIDTH + x]->getRectangle()))
            {
                tiles[y * game::WIDTH + x]->setTileCategory(category);

                for (int rx = -1; rx <= 1; rx++)
                {
                    for (int ry = -1; ry <= 1; ry++)
                    {
                        if (x + rx >= 0 && x + rx < game::WIDTH && y + ry >= 0 && y + ry < game::HEIGHT)
                        {

                            tiles[(y + ry) * game::WIDTH + (x + rx)]->updatePosition();
                        }
                    }
                }
            }
        }
    }

    updateFlag = true;
}

Vector2 World::getWorldAddress(const Vector2 &P) const
{
    return {std::floor(P.x / (float)game::TILE_SIZE), std::floor(P.y / (float)game::TILE_SIZE)};
}

bool World::lineValidation(const Vector2 &A, const Vector2 &B, bool narrow) const
{
    Vector2 Ap = getWorldAddress(A);
    Vector2 Bp = getWorldAddress(B);

    // BRESENHAM algorithm to find all squares in the way
    // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm#:~:text=Bresenham's%20line%20algorithm%20is%20a%20line%20drawing%20algorithm
    int x1 = (int)Ap.x;
    int x2 = (int)Bp.x;
    int y1 = (int)Ap.y;
    int y2 = (int)Bp.y;

    int dx = std::abs(x2 - x1);
    int sx = (x1 < x2) ? 1 : -1;
    int dy = -std::abs(y2 - y1);
    int sy = (y1 < y2) ? 1 : -1;
    int error = dx + dy;

    while (true)
    {
        const Tile *tile = getTile(x1, y1);
        if (tile != nullptr)
        {
            if (narrow && !tile->isNavigable())
            {
                return false;
            }
            if (!narrow && tile->getTileCategory() == TileCategory::WALL)
            {
                return false;
            }
        }
        else
        {
            return false;
        }
        if (x1 == x2 && y1 == y2)
        {
            return true;
        }
        int e2 = 2 * error;
        if (e2 >= dy)
        {
            error += dy;
            x1 += sx;
        }
        if (e2 <= dx)
        {
            error += dx;
            y1 += sy;
        }
    }

    return true;
}

TileCategory World::getTileCategory(int x, int y) const
{
    if (x < 0)
        return TileCategory::WALL;
    if (x >= game::WIDTH)
        return TileCategory::WALL;
    if (y < 0)
        return TileCategory::WALL;
    if (y >= game::HEIGHT)
        return TileCategory::WALL;

    return tiles[y * game::WIDTH + x]->getTileCategory();
}

void World::setTileCategory(int x, int y, TileCategory category)
{
    if ((x < 0) || (x >= game::WIDTH) || (y < 0) || (y >= game::HEIGHT))
        return;

    tiles[y * game::WIDTH + x]->setTileCategory(category);
}

Rectangle World::getRectangle(int x, int y) const
{
    if (x < 0)
        return {-(float)game::TILE_SIZE, (float)(y * game::TILE_SIZE), (float)game::TILE_SIZE, (float)game::TILE_SIZE};
    if (x >= game::WIDTH)
        return {(float)screen::WIDTH, (float)(y * game::TILE_SIZE), (float)game::TILE_SIZE, (float)game::TILE_SIZE};
    if (y < 0)
        return {(float)(x * game::TILE_SIZE), -(float)game::TILE_SIZE, (float)game::TILE_SIZE, (float)game::TILE_SIZE};
    if (y >= game::HEIGHT)
        return {(float)(x * game::TILE_SIZE), (float)game::HEIGHT, (float)game::TILE_SIZE, (float)game::TILE_SIZE};

    return tiles[y * game::WIDTH + x]->getRectangle();
}

const Tile *World::getTile(int x, int y) const
{
    if ((x < 0) || (x >= game::WIDTH) || (y < 0) || (y >= game::HEIGHT))
        return nullptr;

    return tiles[y * game::WIDTH + x];
}
