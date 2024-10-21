#include "soldier.h"
#include "boundingBox.h"
#include "graph.h"
#include "namespaces.h"
#include "node.h"
#include "path.h"
#include "raylib.h"
#include "raymath.h"
#include "rayplus.h"
#include "tile.h"
#include "world.h"
#include <algorithm>
#include <math.h>
#include <memory>

std::vector<std::shared_ptr<Soldier>> Soldier::army = std::vector<std::shared_ptr<Soldier>>();
std::vector<std::shared_ptr<Soldier>> Soldier::selected = std::vector<std::shared_ptr<Soldier>>();

float Soldier::radius = 8.0f;
float Soldier::separationRangeSqr = (Soldier::radius + 10.0f) * (Soldier::radius + 10.0f);
float Soldier::separationStrength = 25.0f;
float Soldier::alignmentRangeSqr = (Soldier::radius + 50.0f) * (Soldier::radius + 50.0f);
float Soldier::alignmentStrength = 2.0f;
float Soldier::obstacleRange = 10.0f;
float Soldier::obstacleStrength = 30.0f;
float Soldier::alongsidePathStrength = 20.0f;
float Soldier::towardsPathStrength = 1.0f;
float Soldier::steeringDelta = 5.0f;
float Soldier::walkingSpeed = 200.0f;

Soldier::Soldier(const Vector2 &position)
    : position(position), direction(), objective({0.0f, 0.0f}), isSelected(false), isTravelling(false), path(nullptr),
      lastTimeImmobile(0.0), lastPosition({0.0f, 0.0f}), lastTimeStuck(0.0)
{
}

std::shared_ptr<Soldier> Soldier::newSoldier(const Vector2 &position)
{
    std::shared_ptr<Soldier> soldier = std::shared_ptr<Soldier>(new Soldier(position));
    Soldier::army.push_back(soldier);

    BBLand *bBLand = BBLand::getInstance();
    bBLand->findBB(soldier);

    return soldier;
}

void Soldier::select()
{
    if (isSelected)
        return;
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

void Soldier::setBoundingBox(std::shared_ptr<BB> boundingBoxP)
{
    boundingBox = boundingBoxP;
}

void Soldier::target(const Vector2 &target, int unitIDP)
{
    // Wait until both functions are not busy
    std::unique_lock<std::mutex> lock(mtx);
    targettingBusy.store(true);

    cv.wait(lock, [this] { return !mainlandBusy.load(); });

    forget();

    objective = target;
    isTravelling = true;
    unitID = unitIDP;

    path = Path::newPath(position, target);
    if (!path->isValid())
    {
        Graph *graph = Graph::getInstance();
        path = Path::newPath(position, graph->getBestNode(position)->getPosition());

        if (!path->isValid())
        {
            forget();
        }
    }

    lastTimeImmobile = GetTime();
    lastTimeStuck = GetTime();
    lastPosition = position;

    targettingBusy.store(false);
}

void Soldier::forget()
{
    isTravelling = false;
    path.reset();
}

void Soldier::followPath(Vector2 &impulse)
{
    Force V;

    if (isTravelling && Vector2DistanceSqr(objective, position) > 4000.0f)
    {
        V = path->getDirectionFromNearby(position);

        impulse += (V.origin - position) * towardsPathStrength;
        impulse += V.direction * alongsidePathStrength;
        lastTimeImmobile = GetTime();
    }
    else
    {
        isTravelling = false;

        if (path != nullptr)
        {
            if (Vector2DistanceSqr(objective, position) > 10.0f)
            {
                impulse += (objective - position) * alongsidePathStrength;
            }
        }
    }
}

void Soldier::avoidAndAlignSoldiers(Vector2 &impulse)
{
    for (int i = 0; i < boundingBox->getNeighbourCount(); i++)
    {
        std::shared_ptr<BB> bB = boundingBox->getNeighbours()[i];
        for (std::shared_ptr<Soldier> s : bB->getSoldiers())
        {
            // separation
            if (!((!isTravelling || !s->isTravelling) && s->unitID == unitID) ||
                (s->isTravelling && s->unitID != unitID) || (isTravelling && s->isTravelling))
            {
                if (Vector2DistanceSqr(position, s->position) <= separationRangeSqr)
                {
                    impulse += (position - s->position) * separationStrength;
                }
            }

            // alignment
            if (s->unitID == unitID && s->isTravelling && isTravelling)
            {
                if (Vector2DistanceSqr(position, s->position) <= alignmentRangeSqr)
                {
                    impulse += s->direction * alignmentStrength;
                }
            }
        }
    }
}

void Soldier::avoidWalls(Vector2 &impulse)
{
    World *world = World::getInstance();
    Vector2 worldIndexMin = world->getWorldAddress(position - Vector2{obstacleRange, obstacleRange});
    Vector2 worldIndexMax = world->getWorldAddress(position + Vector2{obstacleRange, obstacleRange});

    int extra = std::floor(radius / game::TILE_SIZE);
    // iterate only through nearby walls
    for (int x = (int)std::floor(worldIndexMin.x) - 1 - extra; x < (int)std::ceil(worldIndexMax.x) + 1 + extra; x++)
    {
        for (int y = (int)std::floor(worldIndexMin.y) - 1 - extra; y < (int)std::ceil(worldIndexMax.y) + 1 + extra; y++)
        {
            if (world->getTileCategory(x, y) == TileCategory::WALL)
            {
                Rectangle rectangle = world->getRectangle(x, y);
                Vector2 rectangleCenter = {rectangle.x + rectangle.width / 2.0f, rectangle.y + rectangle.height / 2.0f};
                if (CheckCollisionCircleRec(position, obstacleRange, rectangle))
                {
                    impulse += (position - rectangleCenter) * obstacleStrength;
                }
            }
        }
    }
}

void Soldier::checkArrival(const Vector2 &impulse)
{
    // if we are on the path but haven't moved for a while (presumibly we
    // arrived at our destination) forget the path and stop travelling
    if (path != nullptr)
    {
        if (impulse != Vector2Zero())
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
}

void Soldier::selfCorrect()
{
    // if we are travelling but aren't making progress (presumibly stuck)
    // regenerate a different path
    if (isTravelling)
    {
        if (Vector2DistanceSqr(lastPosition, position) >= 2 * game::DELTA * walkingSpeed * walkingSpeed)
        {
            lastPosition = position;
            lastTimeStuck = GetTime();
        }
        else
        {
            if (GetTime() - lastTimeStuck > 2.0)
            {
                path.reset();

                path = Path::newPath(position, objective);
                if (!path->isValid())
                {
                    isTravelling = false;
                }
                lastPosition = position;
                lastTimeStuck = GetTime();
            }
        }
    }
}

void Soldier::applyCollisions()
{
    bool retargetFlag = false;
    if (isInWall())
    {
        for (int i = 0; i < 100; i++)
        {
            position -= Vector2Normalize(direction);
            if (!isInWall())
                break;
        }
        while (isInWall())
        {
            position = Vector2({(float)GetRandomValue(10, screen::WIDTH), (float)GetRandomValue(10, screen::HEIGHT)});
            retargetFlag = true;
        }
    }

    if (retargetFlag && isTravelling)
    {
        path.reset();
        path = Path::newPath(position, objective);
    }
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

    followPath(impulse);
    avoidAndAlignSoldiers(impulse);
    avoidWalls(impulse);

    checkArrival(impulse);

    if (impulse != Vector2Zero())
    {
        impulse = Vector2Normalize(impulse);
    }

    direction = Vector2Lerp(direction, impulse, steeringDelta * dt);
    position += direction * walkingSpeed * dt;

    applyCollisions();

    selfCorrect();

    // unlock the mainland to potential intruders
    mainlandBusy.store(false);
    cv.notify_all(); // Notify any waiting threads
}

bool Soldier::isInWall() const
{
    World *world = World::getInstance();
    Vector2 coords = world->getWorldAddress(position);

    int extra = std::floor(radius / game::TILE_SIZE);
    for (int x = (int)coords.x - 1 - extra; x < (int)coords.x + 1 + extra; x++)
    {
        for (int y = (int)coords.y - 1 - extra; y < (int)coords.y + 1 + extra; y++)
        {
            if (world->getTile(x, y) != nullptr && world->getTileCategory(x, y) == TileCategory::WALL)
            {
                if (CheckCollisionCircleRec(position, radius, world->getRectangle(x, y)))
                {
                    return true;
                }
            }
        }
    }

    return false;
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
