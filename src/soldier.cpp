#include "soldier.h"
#include "namespaces.h"
#include "path.h"
#include "raylib.h"
#include "raymath.h"
#include "tile.h"
#include "world.h"
#include <algorithm>
#include <math.h>

std::vector<Soldier *> Soldier::army = std::vector<Soldier *>();
std::vector<Soldier *> Soldier::selected = std::vector<Soldier *>();

float Soldier::radius = 8.0f;
float Soldier::separationRangeSqr = (Soldier::radius + 9.0f) * (Soldier::radius + 9.0f);
float Soldier::separationStrength = 15.0f;
float Soldier::obstacleRange = 10.0f;
float Soldier::obstacleStrength = 15.0f;
float Soldier::alongsidePathStrength = 20.0f;
float Soldier::towardsPathStrength = 1.0f;
float Soldier::steeringDelta = 5.0f;
float Soldier::walkingSpeed = 100.0f;

Soldier::Soldier(const Vector2 &position)
    : position(position), direction(), isSelected(false), isTravelling(false), path(nullptr), objective({0.0f, 0.0f}),
      lastTimeImmobile(0.0f)
{
    army.push_back(this);
}

Soldier::~Soldier()
{
    army.erase(std::remove(army.begin(), army.end(), this), army.end());
}

void Soldier::select()
{
    isSelected = true;
    selected.push_back(this);
}

void Soldier::deselect()
{
    isSelected = false;
    selected.erase(std::remove(selected.begin(), selected.end(), this), selected.end());
}

const Vector2 &Soldier::getPosition() const
{
    return position;
}

void Soldier::target(const Vector2 &target, int unitIDP)
{
    if (path != nullptr)
    {
        path->goodbye();
    }
    path = Path::newPath(position, target);
    if (path != nullptr)
    {
        path->hello();
        objective = target;
        isTravelling = true;
        unitID = unitIDP;
    }
}

void Soldier::update(float dt)
{
    Vector2 impulse = {0.0f, 0.0f};
    Force V;
    float ratio;

    // follow path
    if ((path != nullptr) && isTravelling && (Vector2DistanceSqr(objective, position) > 4000.0f))
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

    for (Soldier *s : army)
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
    for (int x = (int)std::floor(worldIndexMin.x); x < (int)std::ceil(worldIndexMax.x); x++)
    {
        for (int y = (int)std::floor(worldIndexMin.y); y < (int)std::ceil(worldIndexMax.y); y++)
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

    if (path != nullptr)
    {
        if (!Vector2Equals(impulse, Vector2Zero()))
        {
            lastTimeImmobile = GetTime();
        }
        else
        {
            if (GetTime() - lastTimeImmobile > 1.5)
            { // after being immobile for
              // long enough, lose your path
                path->goodbye();
                path = nullptr;
            }
        }
    }

    if (!Vector2Equals(impulse, Vector2Zero()))
    {
        impulse = Vector2Normalize(impulse);
    }

    direction = Vector2Lerp(direction, impulse, steeringDelta * dt);
    position = Vector2Add(position, Vector2Scale(direction, walkingSpeed * dt));
}

void Soldier::renderBelow() const
{
    if (path != nullptr)
    {
        path->render();
    }
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
