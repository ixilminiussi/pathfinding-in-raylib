#include "boundingBox.h"
#include "effects.h"
#include "graph.h"
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

    BBLand *bBLand = BBLand::getInstance();

    for (int i = 0; i < game::SOLDIER_COUNT; i++)
    {
        Soldier::newSoldier(
            {(float)GetRandomValue(10, screen::WIDTH - 10), (float)GetRandomValue(10, screen::HEIGHT - 10)});
    }

    Mouse *mouse = Mouse::getInstance();
    World *world = World::getInstance();
    Graph *graph = Graph::getInstance();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(shoshone::maroon);

        if (mouse->mode == Mode::Editing)
        {
            game::DELTA = 0;
        }
        else
        {
            game::DELTA = std::min(0.01f, GetFrameTime());
            if (world->updateFlag)
            {
                world->updateFlag = false;
                graph->refresh();
                graph = Graph::getInstance();
            }
        }

        graph->render();

        for (std::shared_ptr<Soldier> s : Soldier::army)
        {
            s->update(game::DELTA);
            s->renderBelow();
        }

        mouse->update(game::DELTA);

        for (std::shared_ptr<Soldier> s : Soldier::army)
        {
            s->renderAbove();

            if (mouse->mode == Mode::Playing && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                if (CheckCollisionCircleRec(s->getPosition(), Soldier::radius + 2.0f, mouse->getSelectionArea()))
                {
                    s->select();
                }
            }
        }

        world->render();
        mouse->render();

        Effect::iterate();

        bBLand->update();
        bBLand->render();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
