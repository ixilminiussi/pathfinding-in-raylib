#pragma once
#include "raylib.h"

class Mouse
{
public:
    Mouse(const Mouse&) = delete;
    Mouse& operator=(const Mouse&) = delete;

    // Static method to get the single instance of the class
    static Mouse* getInstance();

    void update(float dt);
    void render();

    void deselectAll() const;
    const Rectangle &getSelectionArea() const;

private:
    Mouse();
    static Mouse* instance;
    Color fill;
    Color outline;

    Vector2 mouseStart;
    Rectangle selectionArea;
};

