#pragma once
#include "raylib.h"
#include "tile.h"

/**
 * @class World (singleton)
 * @brief manages the tiles (floor/wall) from the navigable world
 *
 */
class World
{
  public:
    /**
     * @brief singleton function
     *
     * @return World* - singleton instance of world
     */
    static World *getInstance();
    ~World() = default;
    World(const World &w) = delete;
    World operator=(const World &w) = delete;
    /**
     * @brief draws tiles on screen
     */
    void render() const;
    /**
     * @brief sets tile categories in a given area to floor or wall
     *
     * @param center Vector2 - center of area
     * @param radius float - radius of area
     * @param category TileCategory - new category
     */
    void paintArea(const Vector2 &center, float radius, enum TileCategory category);
    /**
     * @brief converts screen to world coordinate system
     *
     * @param P Vector2 - screen coordinates
     * @return Vector2 - world coordinates
     */
    Vector2 getWorldAddress(const Vector2 &P) const;
    /**
     * @brief uses bresenham algorithm to check if a given line overlaps a wall
     * tile
     *
     * @param A Vector2 - point A of line (world coordinates)
     * @param B Vector2 - point B of line (world coordinates)
     * @param narrow bool (false) - whether line check should check for walls,
     * or any non navigable tiles
     * @return bool - true if it doesn't hit
     */
    bool lineValidation(const Vector2 &A, const Vector2 &B, bool narrow = false) const;
    /**
     * @brief category of given tile
     *
     * @param x int - x index in world coordinates
     * @param y int - y index in world coordinates
     */
    enum TileCategory getTileCategory(int x, int y) const;
    /**
     * @brief changes category of given tile
     *
     * @param x int - x index in world coordinates
     * @param y int - y index in world coordinates
     * @param category TileCategory - new category
     */
    void setTileCategory(int x, int y, TileCategory category);
    /**
     * @brief changes whether tile is navigable
     *
     * @param x int - x index in world coordinates
     * @param y int - y index in world coordinates
     * @param state bool - new navigable state
     */
    void setNavigable(int x, int y, bool state);
    /**
     * @brief rectangle covered by given tile
     *
     * @param x int - x index in world coordinates
     * @param y int - y index in world coordinates
     */
    class Rectangle getRectangle(int x, int y) const;
    const Tile *getTile(int x, int y) const;

    bool updateFlag;

  private:
    World();
    static World *instance;
    class Tile **tiles;
};
