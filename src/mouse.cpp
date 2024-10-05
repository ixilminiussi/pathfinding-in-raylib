#include "mouse.h"
#include "effects.h"
#include "namespaces.h"
#include "soldier.h"
#include "tile.h"
#include "world.h"
#include <algorithm>
#include <math.h>
#include <raylib.h>

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

void Mouse::update(float dt)
{
    World *world = World::getInstance();

    switch (mode)
    {
    case Mode::Editing:
        if (IsKeyPressed(KEY_TAB))
        {
            mode = Mode::Moving;
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
    case Mode::Moving:

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

            // Sunflower seed algorithm for evenly distributed points in a
            // circle
            for (int n = 1; n <= numSoldiers; n++) // Start from 1 to avoid clustering at the center
            {
                float r = scaleFactor * std::sqrt(n);
                float theta = fmod(n * GOLDEN_ANGLE,
                                   2 * M_PI); // Normalize angle between 0 and 2π

                Vector2 goal = {GetMouseX() + r * cos(theta), GetMouseY() + r * sin(theta)};
                Soldier::selected.at(n - 1)->target(goal, unitID); // Adjust index accordingly
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
    case Mode::Moving:
        // Sunflower seed algorithm for evenly distributed points in a circle
        for (int n = 1; n <= numSoldiers; n++) // Start from 1 to avoid clustering at the center
        {
            float r = scaleFactor * std::sqrt(n);
            float theta = fmod(n * GOLDEN_ANGLE,
                               2 * M_PI); // Normalize angle between 0 and 2π

            Vector2 goal = {GetMouseX() + r * cos(theta), GetMouseY() + r * sin(theta)};
            DrawCircleV(goal, 6.0f, fill);
        }
        break;
    case Mode::Editing:
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
    case Mode::Moving:
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
