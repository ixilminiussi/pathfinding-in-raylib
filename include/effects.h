#pragma once
#include "raylib.h"
#include <memory>
#include <vector>

/**
 * @class Effect
 * @brief virtual class to create and manage special effects
 *
 */
class Effect
{
  public:
    ~Effect() = default;

    /**
     * @brief updates the variables every frame
     *
     * @param dt float - time since last frame
     */
    virtual void update(float dt) = 0;
    /**
     * @brief draws effect on screen
     */
    virtual void render() = 0;
    /**
     * @brief checks whether the effect needs to be deleted according to its own
     * logic
     *
     * @return bool - true if it should be deleted, false otherwise
     */
    virtual bool checkEndOfLife() = 0;

    static std::vector<std::shared_ptr<Effect>> effects;
    /**
     * @brief updates and draws all generated effects, removes those at end of
     * life
     */
    static void iterate();

  protected:
    /**
     * @brief private default constructor for a given effect
     *
     * @param position Vector2 - world position to draw effect
     * @param scale float - world scale of effect
     * @param lerp float - speed of effect, effects are often shapes lerping
     * from A to B
     * @param color Color - color of effect
     */
    Effect(const Vector2 &position, float scale, float lerp, const Color &color);

    Vector2 position;
    float scale;
    float lerp;
    Color color;
};

class ToggleButton final : public Effect
{
  public:
    static std::shared_ptr<ToggleButton> newEffect(const Vector2 &position, float scale, float lerp, const Color &color,
                                                   bool &link, const char *prompt = "");
    /**
     * @brief grows and rotates the effect every frame
     *
     * @param dt float - time since last frame
     */
    void update(float dt) override;
    /**
     * @brief draws effect on screen
     */
    void render() override;
    /**
     * @brief buttons never diiiie
     *
     * @return false always
     */
    bool checkEndOfLife() override;

  private:
    ToggleButton(const Vector2 &position, float scale, float lerp, const Color &color, bool &link, const char *prompt);
    bool &link;
    Rectangle area;
    const char *prompt;
};

class CircleWave final : public Effect
{
  public:
    /**
     * @brief actual constructor is private since shared_ptr cannot be added
     * from the constructor. creates a new Effect and adds it to the list of
     * effects as a shared pointer
     *
     * @param position Vector2 - world position to draw effect
     * @param scale float - world scale of effect
     * @param lerp float - lerp speed to grow/rotate circle effect
     * @param color Color - color of effect
     */
    static std::shared_ptr<CircleWave> newEffect(const Vector2 &position, float scale, float lerp, const Color &color);

    /**
     * @brief grows and rotates the effect every frame
     *
     * @param dt float - time since last frame
     */
    void update(float dt) override;
    /**
     * @brief draws effect on screen
     */
    void render() override;
    /**
     * @brief checks whether the effect is finished
     *
     * @return bool - true if the scale and rotation have reached their final
     * value
     */
    bool checkEndOfLife() override;

  private:
    /**
     * @brief assigns given variables to local variables
     *
     * @param position Vector2 - world position of effect on screen
     * @param scale float - scale of effect on screen
     * @param lerp float - lerp speed to rotate and grow effect
     * @param color Color - color of effect
     */
    CircleWave(const Vector2 &position, float scale, float lerp, const Color &color);

    float mainScale;
    float innerScale;
    float endScale;
    float startingAngle;
};
