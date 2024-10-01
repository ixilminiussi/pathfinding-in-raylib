#include "raylib.h"
#include "include/boid.h"
#include <iostream>
#include <string>
#include "grid.h"
#include "obstacle.h"

#define BOID_COUNT 1700.0f

using namespace std;

void spawnNew(Team team) {
	Boid* b;
	float x, y;
	x = (float)GetRandomValue(2, (int)GetRenderWidth());
	y = (float)GetRandomValue(2, (int)GetRenderHeight());
	b = new Boid(Vector2{ x, y }, team);
	while (b->isColliding()) {
		x = (float)GetRandomValue(2, (int)GetRenderWidth());
		y = (float)GetRandomValue(2, (int)GetRenderHeight());
		b->setPosition({ x, y });
	}

    for (Grid *g : Grid::grids) {
        g->isInOrOut(b);
    }
}

int main() {
    InitWindow(GetScreenWidth(), GetScreenHeight(), "Boids!");
    // SetWindowState(FLAG_FULLSCREEN_MODE);
    SetTargetFPS(60);

    float stepX = (float)(GetScreenWidth()) / 10.0f;
    float stepY = (float)(GetScreenHeight()) / 5.0f;
    for (int x = 0; x < 10; x ++) {
        for (int y = 0; y < 10; y ++) {
            new Grid(Rectangle{ x * stepX, y * stepY, stepX, stepY }, x, y);
        }
    }

    for (Grid *g : Grid::grids) {
        g->findNeighbours();
    }

    new Obstacle(Rectangle{ GetRenderWidth() / 2.0f - 250.0f, GetRenderHeight() / 2.0f - 50.0f, 500.0f, 100.0f });
    new Obstacle(Rectangle{ GetRenderWidth() / 2.0f - 100.0f, 0.0f, 200.0f, 100.0f });
    new Obstacle(Rectangle{ GetRenderWidth() / 2.0f - 100.0f, GetRenderHeight() - 100.0f, 200.0f, 100.0f });

    for (int i = 0; i < BOID_COUNT; i++) {
        if (i < BOID_COUNT / 3) {
            spawnNew(Team::Red);
        }
        else {
            if (i < BOID_COUNT / 3 * 2) {
				spawnNew(Team::Green);
            }
            else {
				spawnNew(Team::Blue);
            }
        }
    } 

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        int blueScore = 0;
        int redScore = 0;
        int greenScore = 0;

        for (Boid *b : Boid::boids) {
            switch (b->team) {
            case Red:
                redScore++;
                break;
            case Blue:
                blueScore++;
                break;
            case Green:
                greenScore++;
                break;
            }
        }

        for (Boid *b : Boid::boids) {
            b -> update();
            b -> render();
        }
        for (Obstacle *o : Obstacle::obstacles) {
            o->render();
        }

        DrawText(std::to_string(blueScore).c_str(), (int)(GetRenderWidth() / 2 - 195), (int)(GetRenderHeight() / 2 - 10), 20, BLUE);
        DrawText(std::to_string(greenScore).c_str(), (int)(GetRenderWidth() / 2 - 20), (int)(GetRenderHeight() / 2 - 10), 20, GREEN);
        DrawText(std::to_string(redScore).c_str(), (int)(GetRenderWidth() / 2 + 165), (int)(GetRenderHeight() / 2 - 10), 20, RED);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}