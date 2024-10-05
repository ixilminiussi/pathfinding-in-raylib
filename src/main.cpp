#include "effects.h"
#include "mouse.h"
#include "namespaces.h"
#include "raylib.h"
#include "soldier.h"
#include "world.h"

int main()
{
    InitWindow(screen::WIDTH, screen::HEIGHT, "Troops");
    if (screen::FULL_SCREEN)
        SetWindowState(FLAG_FULLSCREEN_MODE);

    for (int i = 0; i < game::SOLDIER_COUNT; i++)
    {
        new Soldier({(float)GetRandomValue(0, screen::WIDTH), (float)GetRandomValue(0, screen::HEIGHT)});
    }

    Mouse *mouse = Mouse::getInstance();
    World *world = World::getInstance();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(shoshone::maroon);
        game::DELTA = GetFrameTime();

        for (Soldier *s : Soldier::army)
        {
            s->update(game::DELTA);
            s->renderBelow();
        }

        mouse->update(game::DELTA);
        mouse->renderBelow();

        for (Soldier *s : Soldier::army)
        {
            s->renderAbove();

            if (mouse->mode == Mode::Moving && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                if (CheckCollisionPointRec(s->getPosition(), mouse->getSelectionArea()))
                {
                    s->select();
                }
            }
        }

        world->render();
        mouse->renderTop();

        for (Effect *e : Effect::effects)
        {
            e->update(game::DELTA);
            e->render();
        }

        // Mouse selections

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
