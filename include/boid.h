#pragma once 

#include <vector>
#include "raylib.h"

class Grid;

enum Team {
    Red, Blue, Green
};

class Boid {
public:
    Boid(const Vector2& position, Team team);
    ~Boid();

    void update();
    void render();

    static std::vector<Boid*> boids;

    Vector2 position;
    Vector2 direction;

    void setTeam(Team team);

    Grid* parent = nullptr;

    Team team;
    bool isColliding();
    void setPosition(const Vector2& position);

private:
    void addSeparation(Vector2& force, const Vector2& position);
    void addAlignment(Vector2& force, Boid *);
    void addCohesion(Vector2& force, const Vector2& position);
    void checkAndAdd(Vector2& main, const Vector2& add, const float& strength);

    Team predator;
    Team prey;

    float preyRange;
    float preyStrength;
    float predatorRange;
    float predatorStrength;
    float scale;
    static float alignmentRange;
    static float alignmentStrength;
    static float separationRange;
    static float separationStrength;
    static float cohesionRange;
    static float cohesionStrength;
    static float wallAvoidanceRange;
    static float wallAvoidanceStrength;
    static float steerStrength;
    static float cone;
    static float speed;
    static float eatingRange;
};