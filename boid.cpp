#define _USE_MATH_DEFINES

#include "boid.h"
#include "math.h"
#include "raylib.h"
#include "raymath.h"
#include "obstacle.h"
#include "cell.h"
#include <vector>

std::vector<Boid*> Boid::boids = std::vector<Boid*>();

float Boid::alignmentRange = 60.0f;
float Boid::alignmentStrength = 7.0f;
float Boid::separationRange = 20.0f;
float Boid::separationStrength = 10.0f;
float Boid::cohesionRange = 100.0f;
float Boid::cohesionStrength = 5.0f;
float Boid::wallAvoidanceRange = 50.0f;
float Boid::wallAvoidanceStrength = 10.0f;
float Boid::steerStrength = 10.0f;
float Boid::cone = 0.2f;
float Boid::speed = 150.0f;
float Boid::eatingRange = 3.0f;


Boid::Boid(const Vector2& positionP, Team teamP, Texture2D *textureP) :
    texture(textureP),
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
	Rectangle src = Rectangle{ 0, 0, (float)texture->width, (float)texture->height };
    Vector2 size = Vector2 { 10.0f * scale, 10.0f * scale };
    Color color;
    switch (team) {
    case Team::Red:
        color = Color{ 255, (unsigned char)(155 * direction.x), (unsigned char)(155 * direction.y), 255 };
        break;
    case Team::Blue:
        color = Color{ (unsigned char)(155 * direction.x), (unsigned char)(155 * direction.y), 255, 255 };
        break;
    case Team::Green:
        color = Color{ (unsigned char)(155 * direction.x), 255, (unsigned char)(155 * direction.y), 255 };
        break;
    }
	DrawTexturePro(*texture, src, Rectangle{ position.x, position.y, size.x, size.y }, Vector2{ size.x / 2.0f, size.y / 2.0f }, Vector2Angle({ -1.0f, 0.0f }, direction) * -180.0f / M_PI, color);

    if (debug) {
        DrawCircleLines((int)position.x, (int)position.y, 30.0f * scale, color);
        for (Cell* c : parent->getNeighbours()) {
            if (c == parent) {
                c->render(WHITE);
            }
            else {
                c->render(RED);
            }
        }
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
	force = Vector2Add(force, b->getDirection());
}

void Boid::checkAndAdd(Vector2& main, const Vector2& add, const float& strength) {
    // useless for some, but makes sure that an empty force vector (one that wasn't affected at all) doesn't normalize to an inacurrate direction
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

void Boid::update(float dt) {
    scale = Lerp(scale, 1.0f, 0.2f * dt);

    Vector2 alignmentForce{ 0.0f, 0.0f };
    Vector2 separationForce{ 0.0f, 0.0f };
    Vector2 cohesionForce{ 0.0f, 0.0f };
    Vector2 predatorForce{ 0.0f, 0.0f };
    Vector2 preyForce{ 0.0f, 0.0f };

    // calculating neighbours
    /* Alignment, Cohesion, Prey/predator checks are wide and need to look further than their own cell, 
    * however after 8 checks we have enough information to move on to separation/eating checks in our current cell only */
    int alCoPr = 0;
    if (parent != nullptr) {
        for (Cell* c : parent->getNeighbours()) {
            for (Boid* b : c->getChildren()) {
				if (alCoPr >= 8) {
                    if (c != parent) { break; }
                }
                if (Vector2DotProduct(Vector2Normalize(Vector2Subtract(b->getPosition(), position)), direction) <= cone) {
                    if (b->getTeam() == team) {
                        if (Vector2Distance(b->getPosition(), position) <= separationRange * scale) {
                            addSeparation(separationForce, b->getPosition());
                        }
                        if (Vector2Distance(b->getPosition(), position) <= alignmentRange * scale) {
                            alCoPr++;
                            addAlignment(alignmentForce, b);
                        }
                        if (Vector2Distance(b->getPosition(), position) <= cohesionRange * scale) {
                            addCohesion(cohesionForce, b->getPosition());
                        }
                    }
                    if (b->getTeam() == predator) {
                        if (Vector2Distance(b->getPosition(), position) <= predatorRange * scale) {
                            alCoPr++;
                            addSeparation(predatorForce, b->getPosition());
                        }
                    }
                    if (b->getTeam() == prey) {
                        if (Vector2Distance(b->getPosition(), position) <= preyRange * scale) {
                            alCoPr++;
                            addCohesion(preyForce, b->getPosition());
                        }
                        // WIN (change teams) 
                        if (Vector2Distance(b->getPosition(), position) <= eatingRange * (b->getScale() + scale)) {
                            if (scale < 2.5f) scale += 0.1f;
                            if (b->getScale() > 0.7f) b->setScale(b->getScale() - 0.1f);
                            b->setTeam(predator);
                        }

                    }
                }
            }
        }
    }

    // calculating walls
    Vector2 wallForce = { 0.0f, 0.0f };

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

    // Add forces together
    checkAndAdd(force, wallForce, wallAvoidanceStrength * scale);
    checkAndAdd(force, predatorForce, predatorStrength);
    checkAndAdd(force, preyForce, preyStrength);
    checkAndAdd(force, cohesionForce, cohesionStrength);
    checkAndAdd(force, alignmentForce, alignmentStrength);
    checkAndAdd(force, separationForce, separationStrength);

    // Bigger fish go faster and strafe harder (like sharks) however
    float smoothScale = (float)(log(scale) + 1.0f);

    force = Vector2Normalize(force);
    direction = Vector2Lerp(direction, force, steerStrength / smoothScale * dt);
    direction = Vector2Normalize(direction);

    position = Vector2Add(position, Vector2Scale(direction, speed * smoothScale * dt));
    
    // Collisions hard code. If a fish still somehow hits a wall, walks backwards until he no longer does, and flip it's direction
    // To avoid infinite loops, if the fish hasnt gotten out after 100 steps, teleport him elsewhere at random (hopefully not in another box)
    bool hasHitWall = false;
    for (int i = 0; i < 100; i++) {
        if (!isColliding()) {
            break;
        }
        hasHitWall = true;
        position = Vector2Subtract(position, direction);
    }
    if (isColliding()) {
       setPosition({ (float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight()) });
    }
    if (hasHitWall) {
        direction = Vector2({ -direction.x, -direction.y });
    }

    if (parent == nullptr) {
		for (Cell *c : Cell::cells) {
			c->isInOrOut(this);
		}
    }
    else {
		for (Cell *c : parent->getNeighbours()) {
			c->isInOrOut(this);
		}
    }
}

const Vector2& Boid::getPosition() {
    return position;
}

void Boid::setPosition(const Vector2& positionP) {
    position = positionP;
    parent = nullptr;
}

const Vector2& Boid::getDirection() {
    return direction;
}

void Boid::setDirection(const Vector2& directionP) {
    direction = directionP;
}

const float& Boid::getScale() {
    return scale;
}

void Boid::setScale(const float& scaleP) {
    scale = scaleP;
}

const Team& Boid::getTeam() {
    return team;
}

Cell* Boid::getParent() {
    return parent;
}

void Boid::track() {
    debug = true;
}

void Boid::untrack() {
    debug = false;
}

void Boid::setParent(Cell* parentP) {
    parent = parentP;
}
