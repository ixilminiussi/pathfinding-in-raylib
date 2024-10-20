#pragma once
#include "path.h"
#include "raylib.h"
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <vector>

/**
 * @class Soldier
 * @brief Soldiers are controlled units on the map, they follow paths to their
 * assigned goals and avoid running into one another in the process
 */
class Soldier : public std::enable_shared_from_this<Soldier>
{
  public:
    /**
     * @brief create a new soldier and adds it to the static "army" list
     *
     * @param position Vector2 - starting position of the soldier
     */
    static std::shared_ptr<Soldier> newSoldier(const Vector2 &position);
    ~Soldier() = default;
    /**
     * @brief highlights the soldier
     */
    void select();
    /**
     * @brief de-highlights the soldier
     */
    void deselect();
    /**
     * @brief assigns a target and generates a path to said target. if the path
     * is valid, the soldier starts following it to its destination
     * @param Vector2 - target Vector2 position of the goal
     * @param int - unitID ID used to identify other soldiers with the same goal
     */
    void target(const Vector2 &target, int unitID);
    /**
     * @brief forgets the current path and stops travelling until a new order is
     * assigned
     */
    void forget();
    /**
     * @brief ran every frame, updates the position of the soldier depending on
     * its surroundings/path
     *
     * @param dt float - time since last update
     */
    void update(float dt);
    /**
     * @brief draws target and path on screen
     */
    void renderBelow();
    /**
     * @brief draws the soldier itself
     */
    void renderAbove() const;
    /**
     * @brief simple getter
     *
     * @return Vector2 - position
     */
    const Vector2 &getPosition() const;
    void setBoundingBox(std::shared_ptr<class BB> boundingBox);

    static std::vector<std::shared_ptr<Soldier>> army;
    static std::vector<std::shared_ptr<Soldier>> selected;

    static float radius;

  private:
    Soldier(const Vector2 &position);
    /**
     * @brief influences impulse towards following the path
     *
     * @param impulse Vector2& - impulse to be influenced
     */
    void followPath(Vector2 &impulse);
    /**
     * @brief influences impulse away from other soldiers while following a
     * similar direction (for better flow and boid-like movement)
     *
     * @param impulse Vector2& - impulse to be influenced
     */
    void avoidAndAlignSoldiers(Vector2 &impulse);
    /**
     * @brief influences impulse away from walls
     *
     * @param impulse Vector2& - impulse to be influenced
     */
    void avoidWalls(Vector2 &impulse);
    /**
     * @brief once we have arrived and stop moving, forget path and stop
     * travelling
     *
     * @param impulse Vector2& - current applied force
     */
    void checkArrival(const Vector2 &impulse);
    /**
     * @brief if we are stuck, generate a new path
     */
    void selfCorrect();
    /**
     * @brief if we are inside a wall, get out of said wall by back tracking. If
     * we cannot, simply teleport to random location
     */
    void applyCollisions();
    /**
     * @brief checks if soldier is inside a wall
     *
     * @return Vector2 - {0.0f, 0.0f} if soldier NOT inside a wall, otherwise
     * the average center of all overlapping walls
     */
    bool isInWall() const;

    Vector2 position, direction, lastPosition;
    Vector2 objective;
    bool isSelected, isTravelling;

    std::atomic<bool> targettingBusy{false}, mainlandBusy{false};
    std::mutex mtx;

    std::condition_variable cv;

    int unitID;
    std::shared_ptr<BB> boundingBox;

    std::unique_ptr<Path> path;
    double lastTimeImmobile, lastTimeStuck;

    double timeEntered;

    static float separationRangeSqr;
    static float separationStrength;
    static float alignmentRangeSqr;
    static float alignmentStrength;
    static float obstacleRange;
    static float obstacleStrength;
    static float towardsPathStrength;
    static float alongsidePathStrength;
    static float steeringDelta;
    static float walkingSpeed;
};
