#include "raylib.h"
#include "boid.h"
#include <iostream>
#include <string>
#include "cell.h"
#include "math.h"
#include "obstacle.h"

#define BOID_COUNT 2000.0f

using namespace std;

void spawnNew(Team team, Texture *texture) {
	Boid* b;
	float x, y;
	x = (float)GetRandomValue(2, (int)GetRenderWidth());
	y = (float)GetRandomValue(2, (int)GetRenderHeight());
	b = new Boid(Vector2{ x, y }, team, texture);
	while (b->isColliding()) {
		x = (float)GetRandomValue(2, (int)GetRenderWidth());
		y = (float)GetRandomValue(2, (int)GetRenderHeight());
		b->setPosition({ x, y });
	}

    for (Cell *c : Cell::cells) {
        c->isInOrOut(b);
    }
}

Vector2 mouseStart;

int main() {
    InitWindow(GetScreenWidth(), GetScreenHeight(), "Boids!");
    SetWindowState(FLAG_FULLSCREEN_MODE);

    Image sprite = LoadImage("WhiteFish.png");
	Texture2D texture = LoadTextureFromImage(sprite);

    float stepX = (float)(GetScreenWidth()) / 10.0f;
    float stepY = (float)(GetScreenHeight()) / 5.0f;
    for (int x = 0; x < 10; x ++) {
        for (int y = 0; y < 10; y ++) {
            new Cell(Rectangle{ x * stepX, y * stepY, stepX, stepY }, x, y);
        }
    }

    for (Cell *c : Cell::cells) {
        c->findNeighbours();
    }

    new Obstacle(Rectangle{ GetRenderWidth() / 2.0f - 250.0f, GetRenderHeight() / 2.0f - 50.0f, 500.0f, 100.0f }, true);
    new Obstacle(Rectangle{ GetRenderWidth() / 2.0f - 100.0f, 0.0f, 200.0f, 100.0f });
    new Obstacle(Rectangle{ GetRenderWidth() / 2.0f - 100.0f, GetRenderHeight() - 100.0f, 200.0f, 100.0f });

    for (int i = 0; i < BOID_COUNT; i++) {
        if (i < BOID_COUNT / 3) {
            spawnNew(Team::Red, &texture);
        }
        else {
            if (i < BOID_COUNT / 3 * 2) {
                spawnNew(Team::Green, &texture);
            }
            else {
				spawnNew(Team::Blue, &texture);
            }
        }
    } 

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        float dt = GetFrameTime();

        // update and renders boids + obstacles
        for (Boid *b : Boid::boids) {
            b -> update(dt);
            b -> render();
        }
        for (Obstacle *o : Obstacle::obstacles) {
            o->render();
        }

        // Mouse interactions
        // Start creating
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { mouseStart = GetMousePosition(); }

        // Overlay 
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 start = { std::min((int)mouseStart.x, GetMouseX()), std::min((int)mouseStart.y, GetMouseY())};
            Vector2 size = { std::abs((float)GetMouseX() - mouseStart.x), std::abs((float)GetMouseY() - mouseStart.y) };
            Rectangle current = { start.x, start.y, size.x, size.y };
            DrawRectangleLinesEx(current, 2.0f, WHITE);
        }

        // Create obstacle
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            Vector2 start = { std::min((int)mouseStart.x, GetMouseX()), std::min((int)mouseStart.y, GetMouseY())};
            Vector2 size = { std::abs((float)GetMouseX() - mouseStart.x), std::abs((float)GetMouseY() - mouseStart.y) };
            new Obstacle(Rectangle{ start.x, start.y, size.x, size.y });
        }

        // Delete osbtacle
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            for (Obstacle* o : Obstacle::obstacles) {
                if (o->overlaps(GetMousePosition(), 1.0f)) {
                    o->kill();
                }
            }
        }

        int blueScore = 0;
        int redScore = 0;
        int greenScore = 0;

        for (Boid *b : Boid::boids) {
            switch (b->getTeam()) {
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

        DrawText(std::to_string(blueScore).c_str(), (int)(GetRenderWidth() / 2 - 195), (int)(GetRenderHeight() / 2 - 10), 20, BLUE);
        DrawText(std::to_string(greenScore).c_str(), (int)(GetRenderWidth() / 2 - 20), (int)(GetRenderHeight() / 2 - 10), 20, GREEN);
        DrawText(std::to_string(redScore).c_str(), (int)(GetRenderWidth() / 2 + 165), (int)(GetRenderHeight() / 2 - 10), 20, RED);

        DrawText(std::to_string(GetFPS()).c_str(), 10, 10, 22, WHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}