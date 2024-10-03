#include "raylib.h"
#include "land.h"
#include "soldier.h"
#include "city.h"
#include "mouse.h"
#include "namespaces.h"

int main() {
    {
		using namespace screen; 

        WIDTH = 800;
        HEIGHT = 500;
        FULL_SCREEN = false;

        InitWindow(WIDTH, HEIGHT, "Boids!");
        if (FULL_SCREEN) SetWindowState(FLAG_FULLSCREEN_MODE);
    }

    {
        using namespace game;

        SOLDIER_COUNT = 20;
    }

    /* using namespace Type;
    int *worldGrid = new int[10 * 7]
    {
		P, P, P, P, P, P, P, P, P, P,
		P, R, R, R, R, R, R, R, R, P,
		P, P, P, R, P, R, P, P, R, P,
		P, R, R, R, R, R, P, R, R, P,
		P, R, P, P, P, R, P, P, R, P,
		P, R, R, R, R, R, R, R, R, P,
		P, P, P, P, P, P, P, P, P, P
    }; */

    for (int i = 0; i < game::SOLDIER_COUNT; i++) {
        new Soldier({ (float)GetRandomValue(0, screen::WIDTH), (float)GetRandomValue(0, screen::HEIGHT) });
    }

    // City city = City(worldGrid, 7, 10);

    Mouse* mouse = Mouse::getInstance();

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(shoshone::maroon);
        game::DELTA = GetFrameTime();

		if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            mouse->update(game::DELTA);
		}

        // city.render();
        for (Soldier* s : Soldier::army) {
            s->update(game::DELTA);
            s->render();

            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                if (CheckCollisionPointRec(s->getPosition(), mouse->getSelectionArea())) {
                    s->select();
                }
            }
        }

        if (!IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
			mouse->update(game::DELTA);
        }

        // Mouse selections

        EndDrawing();
    }

    CloseWindow();
    return 0;
}