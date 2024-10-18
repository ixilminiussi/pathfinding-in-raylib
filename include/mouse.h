#pragma once
#include "raylib.h"

enum Mode
{
    Editing,
    Playing
};

class Mouse final
{
  public:
    Mouse(const Mouse &) = delete;
    Mouse &operator=(const Mouse &) = delete;

    // Static method to get the single instance of the class
    static Mouse *getInstance();

    void update(float dt);
    void renderBelow();
    void renderTop();

    void deselectAll() const;
    const Rectangle &getSelectionArea() const;

    Mode mode;

  private:
    Mouse();
    static Mouse *instance;
    Color fill;
    Color outline;

    float editingRadius;

    Vector2 mouseStart;
    Rectangle selectionArea;
    int unitID;
};
