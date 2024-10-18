#include "soldier.h"
#include "namespaces.h"
#include "path.h"
#include "raylib.h"
#include "raymath.h"
#include "tile.h"
#include "world.h"
#include <algorithm>
#include <math.h>
#include <memory>

std::vector<std::shared_ptr<Soldier>> Soldier::army = std::vector<std::shared_ptr<Soldier>>();
std::vector<std::shared_ptr<Soldier>> Soldier::selected = std::vector<std::shared_ptr<Soldier>>();

float Soldier::radius = 8.0f;
float Soldier::separationRangeSqr = (Soldier::radius + 9.0f) * (Soldier::radius + 9.0f);
float Soldier::separationStrength = 15.0f;
float Soldier::obstacleRange = 10.0f;
float Soldier::obstacleStrength = 15.0f;
float Soldier::alongsidePathStrength = 20.0f;
float Soldier::towardsPathStrength = 1.0f;
float Soldier::steeringDelta = 5.0f;
float Soldier::walkingSpeed = 200.0f;

Soldier::Soldier(const Vector2 &position)
    : position(position), direction(), objective({0.0f, 0.0f}), isSelected(false), isTravelling(false), path(nullptr),
      lastTimeImmobile(0.0f)
{
}

std::shared_ptr<Soldier> Soldier::newSoldier(const Vector2 &position)
{
    std::shared_ptr<Soldier> soldier = std::shared_ptr<Soldier>(new Soldier(position));
    Soldier::army.push_back(soldier);
    return soldier;
}

void Soldier::select()
{
    isSelected = true;
    selected.push_back(shared_from_this());
}

void Soldier::deselect()
{
    isSelected = false;
    auto it = std::remove_if(selected.begin(), selected.end(),
                             [this](const std::shared_ptr<Soldier> &soldier) { return soldier.get() == this; });
    selected.erase(it, selected.end());
}

const Vector2 &Soldier::getPosition() const
{
    return position;
}

void Soldier::target(const Vector2 &target, int unitIDP)
{
    // Wait until both functions are not busy
    std::unique_lock<std::mutex> lock(mtx);
    targettingBusy.store(true);

    cv.wait(lock, [this] { return !mainlandBusy.load(); });

    forget();

    path = Path::newPath(position, target);
    if (path->isValid())
    {
        objective = target;
        isTravelling = true;
        unitID = unitIDP;
    }
    else
    {
        forget();
    }

    targettingBusy.store(false);
}

void Soldier::forget()
{
    isTravelling = false;
    path.reset();
}

void Soldier::update(float dt)
{
    if (targettingBusy.load())
    {
        return;
    }

    // Lock the mainland until further notice
    {
        std::lock_guard<std::mutex> lock(mtx);
        mainlandBusy.store(true);
    }

    Vector2 impulse = {0.0f, 0.0f};
    Force V;

    // follow path
    if (isTravelling && (Vector2DistanceSqr(objective, position) > 4000.0f))
    {
        V = path->getDirectionFromNearby(position);

        impulse = Vector2Add(impulse, Vector2Scale(Vector2Subtract(V.origin, position), towardsPathStrength));
        impulse = Vector2Add(impulse, Vector2Scale(V.direction, alongsidePathStrength));
        lastTimeImmobile = GetTime();
    }
    else
    {
        isTravelling = false;

        if (path != nullptr)
        {
            if (Vector2DistanceSqr(objective, position) > 10.0f)
            {
                impulse =
                    Vector2Add(impulse, Vector2Scale(Vector2Subtract(objective, position), alongsidePathStrength));
            }
        }
    }

    for (std::shared_ptr<Soldier> s : army)
    {
        if (!((!isTravelling || !s->isTravelling) && s->unitID == unitID) || (s->isTravelling && s->unitID != unitID) ||
            (isTravelling && s->isTravelling))
        {
            if (Vector2DistanceSqr(position, s->position) <= separationRangeSqr)
            {
                impulse = Vector2Add(impulse, Vector2Scale(Vector2Subtract(position, s->position), separationStrength));
                if (isTravelling)
                {
                    impulse = Vector2Add(impulse, Vector2Scale(V.direction, 2.0f * alongsidePathStrength));
                }
            }
        }
    }

    // avoid walls
    World *world = World::getInstance();
    Vector2 worldIndexMin = world->getWorldAddress(Vector2Subtract(position, Vector2{obstacleRange, obstacleRange}));
    Vector2 worldIndexMax = world->getWorldAddress(Vector2Add(position, Vector2{obstacleRange, obstacleRange}));

    // iterate only through nearby walls
    for (int x = (int)std::floor(worldIndexMin.x) - 1; x < (int)std::ceil(worldIndexMax.x) + 1; x++)
    {
        for (int y = (int)std::floor(worldIndexMin.y) - 1; y < (int)std::ceil(worldIndexMax.y) + 1; y++)
        {
            if (world->getTileCategory(x, y) == TileCategory::WALL)
            {
                Rectangle rectangle = world->getRectangle(x, y);
                Vector2 rectangleCenter = {rectangle.x + rectangle.width / 2.0f, rectangle.y + rectangle.height / 2.0f};
                if (CheckCollisionCircleRec(position, obstacleRange, rectangle))
                {
                    impulse =
                        Vector2Add(impulse, Vector2Scale(Vector2Subtract(position, rectangleCenter), obstacleStrength));
                }
            }
        }
    }

    // if we are on the path but haven't moved for a while (presumibly we
    // arrived at our destination) forget the path and stop travelling
    if (path != nullptr)
    {
        if (!Vector2Equals(impulse, Vector2Zero()))
        {
            lastTimeImmobile = GetTime();
        }
        else
        {
            if (GetTime() - lastTimeImmobile > 1.5)
            {
                forget();
            }
        }
    }

    if (!Vector2Equals(impulse, Vector2Zero()))
    {
        impulse = Vector2Normalize(impulse);
    }

    direction = Vector2Lerp(direction, impulse, steeringDelta * dt);
    position = Vector2Add(position, Vector2Scale(direction, walkingSpeed * dt));

    if (isInWall())
    {
        if (!Vector2Equals(direction, Vector2Zero()))
        {
            for (int i = 0; i < 100; i++)
            {
                position = Vector2Subtract(position, direction);
                if (!isInWall())
                    break;
            }
        }
        while (isInWall())
        {
            position = Vector2({(float)GetRandomValue(10, screen::WIDTH), (float)GetRandomValue(10, screen::HEIGHT)});
        }
    }

    // unlock the mainland to potential intruders
    mainlandBusy.store(false);
    cv.notify_all(); // Notify any waiting threads
}

bool Soldier::isInWall() const
{
    World *world = World::getInstance();
    Vector2 coords = world->getWorldAddress(position);

    return world->getTileCategory((int)coords.x, (int)coords.y) == TileCategory::WALL;
}

void Soldier::renderBelow()
{
    if (targettingBusy.load())
    {
        return;
    }
    // Lock the mainland until further notice
    {
        std::lock_guard<std::mutex> lock(mtx);
        mainlandBusy.store(true);
    }

    if (isTravelling && path != nullptr)
    {
        path->render();
    }

    // unlock the mainland to potential intruders
    mainlandBusy.store(false);
    cv.notify_all(); // Notify any waiting threads
}

void Soldier::renderAbove() const
{
    if (isSelected)
    {
        DrawCircle((int)position.x, (int)position.y, (int)radius, shoshone::yellow);
    }
    else
    {
        DrawCircle((int)position.x, (int)position.y, (int)radius, shoshone::red);
    }
}
