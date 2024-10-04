#include "raylib.h"
#include "land.h"
#include "soldier.h"
#include "city.h"
#include "world.h"
#include "mouse.h"
#include "namespaces.h"

int main() {
	InitWindow(screen::WIDTH, screen::HEIGHT, "Boids!");
	if (screen::FULL_SCREEN) SetWindowState(FLAG_FULLSCREEN_MODE);

    for (int i = 0; i < game::SOLDIER_COUNT; i++) {
        new Soldier({ (float)GetRandomValue(0, screen::WIDTH), (float)GetRandomValue(0, screen::HEIGHT) });
    }

    Mouse* mouse = Mouse::getInstance();
    World* world = World::getInstance();

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(shoshone::maroon);
        game::DELTA = GetFrameTime();

		mouse->update(game::DELTA);

        // city.render();
        for (Soldier* s : Soldier::army) {
            s->update(game::DELTA);
            s->render();

            if (mouse->mode == Mode::Moving && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                if (CheckCollisionPointRec(s->getPosition(), mouse->getSelectionArea())) {
                    s->select();
                }
            }
        }

        world->render();

        mouse->render();

        // Mouse selections

        EndDrawing();
    }

    CloseWindow();
    return 0;
}