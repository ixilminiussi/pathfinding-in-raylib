#pragma once

#include "soldier.h"
#include <memory>
#include <raylib.h>

class BB
{
  public:
    BB() = delete;
    BB(const Rectangle &area);
    ~BB() = default;

    const std::vector<std::shared_ptr<Soldier>> getSoldiers() const;
    void update();
    void render() const;
    const Rectangle &getRectangle() const;
    const std::shared_ptr<BB> *getNeighbours() const;
    int getNeighbourCount() const;
    void addSoldier(std::shared_ptr<Soldier> soldier, std::shared_ptr<BB> self);
    void removeSoldier(std::shared_ptr<Soldier> soldier);
    bool checkOverlap(const Vector2 &position);
    void addNeighbour(std::shared_ptr<BB> neighbour);

  private:
    std::vector<std::shared_ptr<Soldier>> soldiers;
    std::shared_ptr<BB> neighbours[9];
    int neighbourCount;
    Rectangle area;
    Color debugColor;
};

class BBLand final
{
  public:
    static BBLand *getInstance();
    BBLand(const BBLand &bbl) = delete;
    BBLand operator=(const BBLand &bbl) = delete;
    ~BBLand() = default;

    std::shared_ptr<BB> getBox(int x, int y);
    void update();
    void render() const;
    bool findBB(std::shared_ptr<Soldier> soldier);

  private:
    BBLand(int subDivisionsX, int subDivisionsY);
    static BBLand *instance;
    std::shared_ptr<BB> *boxes;

    int subdivisionsX;
    int subdivisionsY;
};
