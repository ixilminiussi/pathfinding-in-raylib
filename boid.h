#pragma once 

#include <vector>
#include "raylib.h"

static bool DEBUGGING = false;

// used for boundingBox optimisation
class Cell;

enum Team {
    Red, Blue, Green
};

class Boid {
public:

    /* Creates a new boid, adds it to the static list of boids
    * @params
    * - position: spawn position of Boid
    * - team: team of boid (one of 3) */
    Boid(const Vector2& position, Team team, Texture2D *textureP);

    /* Removes boid from static list of boids */
    ~Boid();

    /* Updates the boids position based on all neighbouring boids and obstacles */
    void update(float dt);

    /* Renders the boid according to its scale, team and direction */
    void render();

    /* Check if the boid is colliding with ANYTHING */
    bool isColliding();

    static std::vector<Boid*> boids; //Static list of all existing boids, auto generating

    /* Getters and Setters */
    const Vector2& getPosition();
    const Vector2& getDirection();
    const float& getScale();
    Cell* getParent();
    const Team& getTeam();
    void setPosition(const Vector2& position);
    void setDirection(const Vector2& direction);
    void setScale(const float& scale);
    void setParent(Cell* parent);
    void track(); // debugging
    void untrack();

    /* Change team and adopt team's properties */
    void setTeam(Team team);

private:
    Vector2 direction;
    Vector2 position;

    Team team;
    bool debug = false;

    Cell* parent = nullptr;

    Texture2D *texture;

    /* Applies separation onto a given vector according to a given point
    * @params
    * - force: the vector onto which force will be added
    * - position: the position we are moving away from */
    void addSeparation(Vector2& force, const Vector2& position);

    /* Applies alignment onto a given vector according to a given point
    * @params
    * - force: the vector onto which force will be added
    * - boid: the boid we are aligning with */
    void addAlignment(Vector2& force, Boid *boid);

    /* Applies cohesion onto a given vector according to a given point
    * @params
    * - force: the vector onto which force will be added
    * - position: the position we are moving towards */
    void addCohesion(Vector2& force, const Vector2& position);

    /* Normalizes and adds a given vector onto another according to a given strength.
    * Checks that the first vector isn't empty before normalizing
    * @params:
    * - main: the vector receiving the other
    * - add: the vector being normalized, scaled and added
    * - strength: the factor we are scaling the add vector by
    */
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