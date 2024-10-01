#include "include/boid.h"
#include "raylib.h"
#include "raymath.h"
#include "obstacle.h"
#include "grid.h"
#include <vector>

std::vector<Boid*> Boid::boids = std::vector<Boid*>();

float Boid::alignmentRange = 60.0f;
float Boid::alignmentStrength = 6.0f;
float Boid::separationRange = 20.0f;
float Boid::separationStrength = 10.0f;
float Boid::cohesionRange = 100.0f;
float Boid::cohesionStrength = 2.0f;
float Boid::wallAvoidanceRange = 50.0f;
float Boid::wallAvoidanceStrength = 10.0f;
float Boid::steerStrength = 0.5f;
float Boid::cone = 0.2f;
float Boid::speed = 7.0f;
float Boid::eatingRange = 3.0f;


Boid::Boid(const Vector2& positionP, Team teamP) :
    position(positionP),
    team(teamP),
    direction(Vector2{ 1.0f, 1.0f }),
    preyRange(0.0f),
    preyStrength(0.0f),
    predatorRange(0.0f),
    predatorStrength(0.0f),
    scale(1.0f)
{
    boids.push_back(this);
    setTeam(team);
}

Boid::~Boid() {
    // remover from vector
    boids.erase(std::remove(boids.begin(), boids.end(), this), boids.end());
}

void Boid::render() {
    switch (team) {
    case Team::Red:
        DrawCircle((int)position.x, (int)position.y, 3.0f * scale, Color{ 255, (unsigned char)(155 * direction.x), (unsigned char)(155 * direction.y), 255 });
        break;
    case Team::Blue:
        DrawCircle((int)position.x, (int)position.y, 3.0f * scale, Color{ (unsigned char)(155 * direction.x), (unsigned char)(155 * direction.y), 255, 255 });
        break;
    case Team::Green:
        DrawCircle((int)position.x, (int)position.y, 3.0f * scale, Color{ (unsigned char)(155 * direction.x), 255, (unsigned char)(155 * direction.y), 255 });
        break;
    }
}

void Boid::setTeam(Team teamP) {
    team = teamP;

    switch (team) {
    case Team::Red:
        prey = Team::Blue;
        predator = Team::Green;

        preyRange = 100.0f;
        preyStrength = 3.0f;

        predatorRange = 300.0f;
        predatorStrength = 10.0f;
        break;
    case Team::Blue:
        prey = Team::Green;
        predator = Team::Red;

        preyRange = 300.0f;
        preyStrength = 10.0f;

        predatorRange = 100.0f;
        predatorStrength = 3.0f;
        break;
    case Team::Green:
        prey = Team::Red;
        predator = Team::Blue;

        preyRange = 150.0f;
        preyStrength = 5.0f;

        predatorRange = 150.0f;
        predatorStrength = 5.0f;
        break;
    }
}

void Boid::addSeparation(Vector2& force, const Vector2& positionP) {
	force = Vector2Add(force, Vector2Subtract(position, positionP));
}

void Boid::addCohesion(Vector2& force, const Vector2& positionP) {
	force = Vector2Add(force, Vector2Subtract(positionP, position));
}

void Boid::addAlignment(Vector2& force, Boid* b) {
	force = Vector2Add(force, b->direction);
}

void Boid::checkAndAdd(Vector2& main, const Vector2& add, const float& strength) {
    if (add.x != 0.0f || add.y != 0.0f) {
		main = Vector2Add(main, Vector2Scale(Vector2Normalize(add), strength));
    }
}

bool Boid::isColliding() {
    if (position.x - scale <= 0.0f) {
        return true;
    }
    if (GetRenderWidth() - position.x + scale <= 0.0f) {
        return true;
    }
    if (position.y <= 0.0f) {
        return true;
    }
    if (GetRenderHeight() - position.y + scale <= 0.0f) {
        return true;
    }

    for (auto o : Obstacle::obstacles) {
        if (o->overlaps(position, scale)) {
            return true;
        }
    }

    return false;
}

void Boid::setPosition(const Vector2& positionP) {
    position = positionP;
}

void Boid::update() {
    scale = Lerp(scale, 1.0f, 0.001f);

    Vector2 alignmentForce{ 0.0f, 0.0f };
    Vector2 separationForce{ 0.0f, 0.0f };
    Vector2 cohesionForce{ 0.0f, 0.0f };
    Vector2 predatorForce{ 0.0f, 0.0f };
    Vector2 preyForce{ 0.0f, 0.0f };

    // calculating neighbours
    if (parent != nullptr) {
        for (Grid* g : parent->neighbours) {
            for (Boid* b : g->children) {
                if (Vector2DotProduct(Vector2Normalize(Vector2Subtract(b->position, position)), direction) <= cone) {
                    if (b->team == team) {
                        if (Vector2Distance(b->position, position) <= separationRange * scale) {
                            addSeparation(separationForce, b->position);
                        }
                        if (Vector2Distance(b->position, position) <= alignmentRange * scale) {
                            addAlignment(alignmentForce, b);
                        }
                        if (Vector2Distance(b->position, position) <= cohesionRange * scale) {
                            addCohesion(cohesionForce, b->position);
                        }
                    }
                    else {
                        if (b->team == predator) {
                            if (Vector2Distance(b->position, position) <= predatorRange * scale) {
                                addSeparation(predatorForce, b->position);
                            }
                            // LOST (change teams) 
                            if (Vector2Distance(b->position, position) <= eatingRange * b->scale) {
                                if (scale > 0.7f) scale -= 0.1f;
                                if (b->scale < 2.0f) b->scale += 0.1f;
                                setTeam(predator);
                            }
                        }
                        if (b->team == prey) {
                            if (Vector2Distance(b->position, position) <= preyRange * scale) {
                                addCohesion(preyForce, b->position);
                            }
                        }
                    }
                }
            }
        }
    }

    Vector2 wallForce = { 0.0f, 0.0f };

    // walls
    if (position.x <= wallAvoidanceRange * scale) {
        wallForce.x += 1.0f;
    }
    if (GetRenderWidth() - position.x <= wallAvoidanceRange * scale) {
        wallForce.x += -1.0f;
    }
    if (position.y <= wallAvoidanceRange) {
        wallForce.y += 1.0f;
    }
    if (GetRenderHeight() - position.y <= wallAvoidanceRange * scale) {
        wallForce.y += -1.0f;
    }

    for (auto o : Obstacle::obstacles) {
        if (o->overlaps(position, wallAvoidanceRange * scale)) {
            addSeparation(wallForce, o -> getCenter());
        }
    }

    Vector2 force{ 0.0f, 0.0f };

    // Add wall
    checkAndAdd(force, wallForce, wallAvoidanceStrength * scale);
    checkAndAdd(force, predatorForce, predatorStrength);
    checkAndAdd(force, preyForce, preyStrength);
    checkAndAdd(force, cohesionForce, cohesionStrength);
    checkAndAdd(force, alignmentForce, alignmentStrength);
    checkAndAdd(force, separationForce, separationStrength);

    float smoothScale = (float)(log(scale) + 1.0f);

    force = Vector2Normalize(force);
    direction = Vector2Add(direction, Vector2Scale(force, steerStrength / smoothScale));
    direction = Vector2Normalize(direction);

    position = Vector2Add(position, Vector2Scale(direction, speed * smoothScale));
    
    bool hasHitWall = false;
    while (isColliding()) {
        hasHitWall = true;
        position = Vector2Subtract(position, direction);
    }
    if (hasHitWall) {
        direction = Vector2({ -direction.x, -direction.y });
    }

    if (parent == nullptr) {
		for (Grid *g : Grid::grids) {
			g->isInOrOut(this);
		}
    }
    else {
		for (Grid *g : parent->neighbours) {
			g->isInOrOut(this);
		}
    }
}