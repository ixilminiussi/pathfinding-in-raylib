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

float Soldier::separationRangeSqr = 200.0f;
float Soldier::separationStrength = 15.0f;
float Soldier::obstacleRange = 10.0f;
float Soldier::obstacleStrength = 15.0f;
float Soldier::alongsidePathStrength = 20.0f;
float Soldier::towardsPathStrength = 1.0f;
float Soldier::steeringDelta = 5.0f;
float Soldier::walkingSpeed = 100.0f;
float Soldier::radius = 6.0f;

Soldier::Soldier(const Vector2 &position)
    : position(position), direction(), isSelected(false), isTravelling(false), path(nullptr), objective({0.0f, 0.0f}),
      acceptableDistanceSqr(0.0f), lastTimeImmobile(0.0f)
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

void Soldier::target(const Vector2 &target, const float &acceptableDistanceSqrP)
{
    acceptableDistanceSqr = acceptableDistanceSqrP;

    path = Path::newPath(position, target);
    objective = target;
}

void Soldier::update(float dt)
{
    Vector2 impulse = {0.0f, 0.0f};

    // follow path
    if (path != nullptr && Vector2DistanceSqr(objective, position) > acceptableDistanceSqr)
    {
        isTravelling = true;
        Force V = path->getDirectionFromNearby(position);

        impulse =
            Vector2Add(impulse, Vector2Scale(Vector2Subtract(V.origin, position),
                                             towardsPathStrength * Vector2DistanceSqr(V.origin, position) / 2000.0f));
        impulse = Vector2Add(impulse, Vector2Scale(V.direction, alongsidePathStrength));
    }
    else
    {
        isTravelling = false;
    }

    // avoid other soldiers
    for (Soldier *s : army)
    {
        if (!(isTravelling && !s->isTravelling))
        {
            if (Vector2DistanceSqr(position, s->position) < separationRangeSqr)
            {
                impulse = Vector2Add(impulse, Vector2Scale(Vector2Subtract(position, s->position), separationStrength));
            }
        }
    }

    // avoid walls
    /*World *world = World::getInstance();*/
    /*Vector2 worldIndexMin = world->getWorldAddress(Vector2Subtract(position,
     * Vector2{obstacleRange, obstacleRange}));*/
    /*Vector2 worldIndexMax = world->getWorldAddress(Vector2Add(position,
     * Vector2{obstacleRange, obstacleRange}));*/
    /**/
    /*// iterate only through nearby walls*/
    /*for (int x = (int)std::floor(worldIndexMin.x); x <
     * (int)std::ceil(worldIndexMax.x); x++)*/
    /*{*/
    /*    for (int y = (int)std::floor(worldIndexMin.y); y <
     * (int)std::ceil(worldIndexMax.y); y++)*/
    /*    {*/
    /*        if (world->getType(x, y) == Type::WALL)*/
    /*        {*/
    /*            Rectangle rectangle = world->getRectangle(x, y);*/
    /*            Vector2 rectangleCenter = {rectangle.x + rectangle.width
     * / 2.0f, rectangle.y + rectangle.height / 2.0f};*/
    /*            if (CheckCollisionCircleRec(position, obstacleRange,
     * rectangle))*/
    /*            {*/
    /*                impulse =*/
    /*                    Vector2Add(impulse,
     * Vector2Scale(Vector2Subtract(position, rectangleCenter),
     * obstacleStrength));*/
    /*            }*/
    /*        }*/
    /*    }*/
    /*}*/

    if (!Vector2Equals(impulse, Vector2Zero()))
    {
        impulse = Vector2Normalize(impulse);
        lastTimeImmobile = GetTime();
    }
    else
    {
        if (GetTime() - lastTimeImmobile > 2.0)
        { // after being immobile for long enough, lose your path
            path = nullptr;
        }
    }

    direction = Vector2Lerp(direction, impulse, steeringDelta * dt);
    position = Vector2Add(position, Vector2Scale(direction, walkingSpeed * dt));
}

void Soldier::render() const
{
    if (isSelected)
    {
        DrawCircle((int)position.x, (int)position.y, (int)radius, shoshone::yellow);
        if (path != nullptr && isTravelling && Vector2DistanceSqr(objective, position) > acceptableDistanceSqr)
        {
            path->render();
        }
    }
    else
    {
        DrawCircle((int)position.x, (int)position.y, (int)6.0f, shoshone::red);
    }
}
