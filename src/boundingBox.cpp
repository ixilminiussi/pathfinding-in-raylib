#include "boundingBox.h"
#include "namespaces.h"
#include <algorithm>
#include <memory>
#include <raylib.h>

BB::BB(const Rectangle &areaP) : area(areaP), neighbourCount(0)
{
    debugColor = {(u_char)GetRandomValue(0, 255), (u_char)GetRandomValue(0, 255), (u_char)GetRandomValue(0, 255), 255};
}

const std::vector<std::shared_ptr<Soldier>> BB::getSoldiers() const
{
    return soldiers;
}

const std::shared_ptr<BB> *BB::getNeighbours() const
{
    return neighbours;
}

const Rectangle &BB::getRectangle() const
{
    return area;
}

int BB::getNeighbourCount() const
{
    return neighbourCount;
}

bool BB::checkOverlap(const Vector2 &position)
{
    return (CheckCollisionPointRec(position, area));
}

void BB::addSoldier(std::shared_ptr<Soldier> soldier, std::shared_ptr<BB> self)
{
    soldiers.push_back(soldier);
    soldier->setBoundingBox(self);
}

void BB::removeSoldier(std::shared_ptr<Soldier> soldier)
{
    soldiers.erase(std::remove(soldiers.begin(), soldiers.end(), soldier), soldiers.end());
}

void BB::addNeighbour(std::shared_ptr<BB> neighbour)
{
    neighbours[neighbourCount] = neighbour;
    neighbourCount++;
}

void BB::update()
{
    for (auto it = soldiers.begin(); it != soldiers.end();)
    {
        std::shared_ptr<Soldier> soldier = *it;
        bool success = false;

        if (checkOverlap(soldier->getPosition()))
        {
            it++;
            continue;
        }
        for (int i = 0; i < neighbourCount; i++)
        {
            if (neighbours[i]->checkOverlap(soldier->getPosition()))
            {
                removeSoldier(soldier);
                neighbours[i]->addSoldier(soldier, neighbours[i]);
                success = true;
                break;
            }
        }

        if (!success)
        {
            removeSoldier(soldier);
            BBLand::getInstance()->findBB(soldier);
        }
    }
}

void BB::render() const
{
    if (debug::boundingBoxes)
    {
        DrawRectangleLines(area.x + 1, area.y + 1, area.width - 2, area.height - 2, debugColor);

        for (std::shared_ptr<Soldier> soldier : soldiers)
        {
            DrawCircle(soldier->getPosition().x, soldier->getPosition().y, 5.0f, debugColor);
        }
    }
}

BBLand *BBLand::instance = nullptr;

BBLand *BBLand::getInstance()
{
    if (instance == nullptr)
    {
        instance = new BBLand(game::bbSubdivisionsX, game::bbSubdivisionsY);
    }

    return instance;
}

BBLand::BBLand(int subdivisionsXP, int subdivisionsYP) : subdivisionsX(subdivisionsXP), subdivisionsY(subdivisionsYP)
{
    boxes = new std::shared_ptr<BB>[subdivisionsX * subdivisionsY];
    float width = (float)screen::WIDTH / (float)subdivisionsX;
    float height = (float)screen::HEIGHT / (float)subdivisionsY;

    for (int x = 0; x < subdivisionsX; x++)
    {
        for (int y = 0; y < subdivisionsY; y++)
        {
            boxes[y * subdivisionsX + x] = std::make_shared<BB>(Rectangle({x * width, y * height, width, height}));
        }
    }

    for (int x = 0; x < subdivisionsX; x++)
    {
        for (int y = 0; y < subdivisionsY; y++)
        {
            std::shared_ptr<BB> box = getBox(x, y);
            if (x > 0 && y > 0)
                box->addNeighbour(getBox(x - 1, y - 1));
            if (x > 0)
                box->addNeighbour(getBox(x - 1, y));
            if (y > 0)
                box->addNeighbour(getBox(x, y - 1));
            if (x < subdivisionsX - 1 && y > 0)
                box->addNeighbour(getBox(x + 1, y - 1));
            if (x < subdivisionsX - 1)
                box->addNeighbour(getBox(x + 1, y));
            if (x < subdivisionsX - 1 && y < subdivisionsY - 1)
                box->addNeighbour(getBox(x + 1, y + 1));
            if (y < subdivisionsY - 1)
                box->addNeighbour(getBox(x, y + 1));
            if (x > 0 && y < subdivisionsY - 1)
                box->addNeighbour(getBox(x - 1, y + 1));

            box->addNeighbour(box);
        }
    }
}

std::shared_ptr<BB> BBLand::getBox(int x, int y)
{
    if (x < 0 || x >= subdivisionsX || y < 0 || y >= subdivisionsY)
        return nullptr;
    return boxes[y * subdivisionsX + x];
}

void BBLand::update()
{
    for (int i = 0; i < subdivisionsX * subdivisionsY; i++)
    {
        boxes[i]->update();
    }
}

void BBLand::render() const
{
    for (int i = 0; i < subdivisionsX * subdivisionsY; i++)
    {
        boxes[i]->render();
    }
}

bool BBLand::findBB(std::shared_ptr<Soldier> soldier)
{
    for (int i = 0; i < subdivisionsX * subdivisionsY; i++)
    {
        if (CheckCollisionPointRec(soldier->getPosition(), boxes[i]->getRectangle()))
        {
            boxes[i]->addSoldier(soldier, boxes[i]);
            return true;
        }
    }

    return false;
}
