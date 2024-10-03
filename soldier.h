#pragma once
#include "path.h"
#include "raylib.h"
#include <vector>

class Soldier
{
  public:
    Soldier(const Vector2 &position);
    ~Soldier();

    void select();
    void deSelect();
    void target(const Vector2 &target, const float& acceptableDistanceSqr);

    void update(float dt);
    void render() const;

    // Vector2 *getObjective();
    const Vector2& getPosition() const;

    static std::vector<Soldier *> army;
    static std::vector<Soldier *> selected;

  private:
    Vector2 position;
    Vector2 direction;
    Vector2 objective;

    bool isSelected, isTravelling;
    float acceptableDistanceSqr;

    Path *path;

    static float separationRangeSqr;
    static float separationStrength;
    static float towardsPathStrength;
    static float alongsidePathStrength;
    static float steeringDelta;
    static float walkingSpeed;
};
