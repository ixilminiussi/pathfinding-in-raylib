#pragma once
#include "path.h"
#include "raylib.h"
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <vector>

class Soldier : public std::enable_shared_from_this<Soldier>
{
  public:
    static std::shared_ptr<Soldier> newSoldier(const Vector2 &position);
    ~Soldier() = default;

    void select();
    void deselect();
    void target(const Vector2 &target, int unitID);
    void forget();

    void update(float dt);
    void renderBelow();
    void renderAbove() const;

    const Vector2 &getPosition() const;

    static std::vector<std::shared_ptr<Soldier>> army;
    static std::vector<std::shared_ptr<Soldier>> selected;

    static float radius;

  private:
    Soldier(const Vector2 &position);
    bool isInWall() const;

    Vector2 position, direction;
    Vector2 objective;
    bool isSelected, isTravelling;

    std::atomic<bool> targettingBusy{false}, mainlandBusy{false};
    std::mutex mtx;

    std::condition_variable cv;

    int unitID;

    std::unique_ptr<Path> path;
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
