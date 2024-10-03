#include "soldier.h"
#include "path.h"
#include "raylib.h"
#include "raymath.h"
#include "namespaces.h"
#include <iostream>
#include <algorithm>

std::vector<Soldier *> Soldier::army = std::vector<Soldier *>();
std::vector<Soldier *> Soldier::selected = std::vector<Soldier *>();

float Soldier::separationRangeSqr = 300.0f;
float Soldier::separationStrength = 15.0f;
float Soldier::alongsidePathStrength = 20.0f;
float Soldier::towardsPathStrength = 1.0f;
float Soldier::steeringDelta = 5.0f;
float Soldier::walkingSpeed = 100;

Soldier::Soldier(const Vector2 &position)
    : position(position), direction(), isSelected(false), isTravelling(false), path(nullptr), objective({ 0.0f, 0.0f }), acceptableDistanceSqr(0.0f)
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

void Soldier::deSelect()
{
    isSelected = false;
    selected.erase(std::remove(selected.begin(), selected.end(), this), selected.end());
}

const Vector2& Soldier::getPosition() const {
    return position;
}

void Soldier::target(const Vector2 &target, const float& acceptableDistanceSqrP)
{
    acceptableDistanceSqr = acceptableDistanceSqrP;

    path = Path::newPath(position, target);
    objective = target;
    isTravelling = true;
}

void Soldier::update(float dt)
{
    Vector2 impulse = {0.0f, 0.0f};

    if (isTravelling && path != nullptr && Vector2DistanceSqr(objective, position) > acceptableDistanceSqr)
    {
        Force V = path->getDirectionFromNearby(position);

        impulse = Vector2Add(impulse, Vector2Scale(Vector2Subtract(V.origin, position), towardsPathStrength * Vector2DistanceSqr(V.origin, position) / 2000.0f));
        impulse = Vector2Add(impulse, Vector2Scale(V.direction, alongsidePathStrength));
    }

    for (Soldier *s : army)
    {
        if (Vector2DistanceSqr(position, s->position) < separationRangeSqr)
        {
            impulse = Vector2Add(impulse, Vector2Scale(Vector2Subtract(position, s->position), separationStrength));
        }
    }

    if (!Vector2Equals(impulse, Vector2Zero())) impulse = Vector2Normalize(impulse);

	direction = Vector2Lerp(direction, impulse, steeringDelta * dt);
	position = Vector2Add(position, Vector2Scale(direction, walkingSpeed * dt));
}

void Soldier::render() const
{
    if (isSelected)
    {
		DrawCircle((int)position.x, (int)position.y, (int)6.0f, shoshone::yellow);
		if (path != nullptr && isTravelling && Vector2DistanceSqr(objective, position) > acceptableDistanceSqr)
		{
			path->render();
		}
    }
    else {
		DrawCircle((int)position.x, (int)position.y, (int)6.0f, shoshone::red);
    }
}
