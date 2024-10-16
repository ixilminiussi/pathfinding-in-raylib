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
    void deselect();
    void target(const Vector2 &target, int unitID);

    void update(float dt);
    void renderBelow() const;
    void renderAbove() const;

    // Vector2 *getObjective();
    const Vector2 &getPosition() const;

    static std::vector<Soldier *> army;
    static std::vector<Soldier *> selected;

    static float radius;

  private:
    bool isInWall() const;

    Vector2 position;
    Vector2 direction;

    Vector2 objective;
    bool isSelected, isTravelling;
    int unitID;

    const Path *path;
    double lastTimeImmobile;

    double timeEntered;

    static float separationRangeSqr;
    static float separationStrength;
    static float obstacleRange;
    static float obstacleStrength;
    static float towardsPathStrength;
    static float alongsidePathStrength;
    static float steeringDelta;
    static float walkingSpeed;
};
