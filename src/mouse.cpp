#include "mouse.h"
#include "effects.h"
#include "namespaces.h"
#include "soldier.h"
#include "tile.h"
#include "world.h"
#include <algorithm>
#include <math.h>
#include <raylib.h>
#include <thread>

Mouse *Mouse::instance = nullptr;

using namespace shoshone;
Mouse::Mouse()
    : fill({yellow.r, yellow.g, yellow.b, 100}), outline(yellow), mouseStart({0.0f, 0.0f}),
      selectionArea({0.0f, 0.0f, 0.0f, 0.0f}), mode(Mode::Editing), editingRadius(50.0f), unitID(0)
{
}

Mouse *Mouse::getInstance()
{
    if (instance == nullptr)
    {
        instance = new Mouse();
    }
    return instance;
}

const float GOLDEN_ANGLE = 137.5f * M_PI / 180.0f;

void createTargetThread(Soldier *s, const Vector2 &goal, int unitID)
{
    s->target(goal, unitID);
}

void Mouse::update(float dt)
{
    World *world = World::getInstance();

    switch (mode)
    {
    case Mode::Editing:
        if (IsKeyPressed(KEY_TAB))
        {
            mode = Mode::Playing;
        }

        editingRadius -= GetMouseWheelMove() * 10.0f;
        if (editingRadius < 20.0f)
            editingRadius = 20.0f;
        if (editingRadius > 150.0f)
            editingRadius = 150.0f;
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            world->paintArea(GetMousePosition(), editingRadius, TileCategory::WALL);
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            world->paintArea(GetMousePosition(), editingRadius, TileCategory::FLOOR);
        }

        break;
    case Mode::Playing:

        if (IsKeyPressed(KEY_SPACE))
        {
            deselectAll();
        }

        if (IsKeyPressed(KEY_TAB))
        {
            deselectAll();
            mode = Mode::Editing;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            mouseStart = GetMousePosition();
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Vector2 mousePointer = GetMousePosition();
            Vector2 topLeft = {std::min(mousePointer.x, mouseStart.x), std::min(mousePointer.y, mouseStart.y)};
            Vector2 size = {std::abs(mousePointer.x - mouseStart.x), std::abs(mousePointer.y - mouseStart.y)};

            selectionArea = {topLeft.x, topLeft.y, size.x, size.y};
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            if (IsKeyUp(KEY_LEFT_SHIFT))
            {
                deselectAll();
            }
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            unitID++;
            float scaleFactor = 12.0f; // Dynamically scale
            int numSoldiers = Soldier::selected.size();

            std::vector<std::thread> threads;

            // Sunflower seed algorithm for evenly distributed points in a
            // circle
            for (int n = 1; n <= numSoldiers; n++) // Start from 1 to avoid clustering at the center
            {
                float r = scaleFactor * std::sqrt(n);
                float theta = fmod(n * GOLDEN_ANGLE,
                                   2 * M_PI); // Normalize angle between 0 and 2π

                Vector2 goal = {GetMouseX() + r * cos(theta), GetMouseY() + r * sin(theta)};

                threads.emplace_back(createTargetThread, Soldier::selected.at(n - 1), goal, unitID);
            }

            for (auto &t : threads)
            {
                t.join();
            }
        }
        break;
    }
}

void Mouse::renderTop()
{
    float scaleFactor = 12.0f; // Dynamically scale
    int numSoldiers = Soldier::selected.size();

    switch (mode)
    {
    case Mode::Playing:
        // Sunflower seed algorithm for evenly distributed points in a circle
        for (int n = 1; n <= numSoldiers; n++) // Start from 1 to avoid clustering at the center
        {
            float r = scaleFactor * std::sqrt(n);
            float theta = fmod(n * GOLDEN_ANGLE,
                               2 * M_PI); // Normalize angle between 0 and 2π

            Vector2 goal = {GetMouseX() + r * cos(theta), GetMouseY() + r * sin(theta)};
            DrawCircleV(goal, 6.0f, fill);
        }

        // Menu
        DrawText("<Tab>", 150.0f, 30.0f, 32, outline);
        DrawText("   Editing", 265.0f, 30.0f, 32, fill);
        DrawText("Playing", 425.0f, 30.0f, 32, outline);
        DrawText("<Space>", 115.0f, 100.0f, 32, outline);
        DrawText("   Deselect all", 265.0f, 100.0f, 32, outline);
        DrawText("<Left Click>", 56.0f, 150.0f, 32, outline);
        DrawText("   Select units", 265.0f, 150.0f, 32, outline);
        DrawText("<Right Click>", 44.0f, 200.0f, 32, outline);
        DrawText("   Target location", 264.0f, 200.0f, 32, outline);
        DrawLineEx({266.0f, 30.0f}, {266.0f, 235.0f}, 4.0f, outline);
        break;

    case Mode::Editing:

        // Menu
        DrawText("<Tab>", 150.0f, 30.0f, 32, outline);
        DrawText("   Editing", 265.0f, 30.0f, 32, outline);
        DrawText("Playing", 425.0f, 30.0f, 32, fill);
        DrawText("<Scroll>", 115.0f, 100.0f, 32, outline);
        DrawText("   Change editing area", 265.0f, 100.0f, 32, outline);
        DrawText("<Left Click>", 56.0f, 150.0f, 32, outline);
        DrawText("   Fill wall", 265.0f, 150.0f, 32, outline);
        DrawText("<Right Click>", 44.0f, 200.0f, 32, outline);
        DrawText("   Clear wall", 264.0f, 200.0f, 32, outline);
        DrawLineEx({266.0f, 30.0f}, {266.0f, 235.0f}, 4.0f, outline);
        break;
    }
}

void Mouse::renderBelow()
{
    switch (mode)
    {
    case Mode::Editing:
        DrawCircleV(GetMousePosition(), editingRadius,
                    {shoshone::yellow.r, shoshone::yellow.g, shoshone::yellow.b, 100});
        break;
    case Mode::Playing:
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            DrawRectangleRec(selectionArea, fill);
            DrawRectangleLinesEx(selectionArea, 3.0f, outline);
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))
        {
            new CircleWave(GetMousePosition(), 5.0f, 30.0f, fill);
            new CircleWave(GetMousePosition(), 2.5f, 15.0f, outline);
        }
        break;
    }
}

void Mouse::deselectAll() const
{
    for (Soldier *s : Soldier::army)
    {
        s->deselect();
    }
}

const Rectangle &Mouse::getSelectionArea() const
{
    return selectionArea;
}
