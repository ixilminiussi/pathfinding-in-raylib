#pragma once
#include "raylib.h"

enum Mode
{
    Editing,
    Playing
};

/**
 * @class Mouse (singleton)
 * @brief class to manage mouse locations and user inputs
 *
 */
class Mouse final
{
  public:
    /**
     * @brief singleton function
     *
     * @return Mouse* - singleton instance of mouse
     */
    static Mouse *getInstance();
    Mouse(const Mouse &) = delete;
    Mouse &operator=(const Mouse &) = delete;
    /**
     * @brief receives and reacts to all inputs
     *
     * @param dt float - time since last frame
     */
    void update(float dt);
    /**
     * @brief draws UI and inputs
     */
    void render();
    /**
     * @brief deselect all soldiers
     */
    void deselectAll() const;
    /**
     * @brief getter for the selection area of the mouse
     *
     * @return const Rectangle - selected area
     */
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
