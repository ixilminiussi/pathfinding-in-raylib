#include "effects.h"
#include "namespaces.h"
#include "raylib.h"
#include <algorithm>
#include <memory>
#include <raymath.h>

std::vector<std::shared_ptr<Effect>> Effect::effects = std::vector<std::shared_ptr<Effect>>();

void Effect::iterate()
{
    for (auto it = Effect::effects.begin(); it != Effect::effects.end();)
    {
        (*it)->update(game::DELTA);
        (*it)->render();

        if (!(*it)->checkEndOfLife())
        {
            it++;
        }
    }
}

Effect::Effect(const Vector2 &position, float scale, float lerp, const Color &color)
    : position(position), scale(scale), lerp(lerp), color(color)
{
}

std::shared_ptr<ToggleButton> ToggleButton::newEffect(const Vector2 &position, float scale, float lerp,
                                                      const Color &color, bool &link, const char *prompt)
{
    std::shared_ptr<ToggleButton> button =
        std::shared_ptr<ToggleButton>(new ToggleButton(position, scale, lerp, color, link, prompt));

    Effect::effects.push_back(button);

    return button;
}

ToggleButton::ToggleButton(const Vector2 &position, float scale, float lerp, const Color &color, bool &linkP,
                           const char *promptP)
    : Effect(position, scale, lerp, color), link(linkP),
      area(Rectangle({position.x, position.y, 20.0f * scale, 20.0f * scale})), prompt(promptP)
{
}

void ToggleButton::update(float dt)
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (CheckCollisionPointRec(GetMousePosition(), area))
        {
            link = (link) ? false : true;
        }
    }
}

void ToggleButton::render()
{
    DrawRectangleLinesEx(area, 2.0f, color);
    if (link)
        DrawRectangle(position.x + (4.0f * scale), position.y + (4.0f * scale), area.width - (8.0f * scale),
                      area.height - (8.0f * scale), color);

    DrawText(prompt, position.x + area.width + (10.0f * scale), position.y, (int)area.height, color);
}

bool ToggleButton::checkEndOfLife()
{
    return false;
}

std::shared_ptr<CircleWave> CircleWave::newEffect(const Vector2 &position, float scale, float lerp, const Color &color)
{
    std::shared_ptr<CircleWave> effect = std::shared_ptr<CircleWave>(new CircleWave(position, scale, lerp, color));

    Effect::effects.push_back(effect);

    return effect;
}

CircleWave::CircleWave(const Vector2 &position, float scale, float lerp, const Color &color)
    : Effect(position, scale, lerp, color), mainScale(1.0f * scale), innerScale(0.0f), endScale(10.0f * scale),
      startingAngle((float)GetRandomValue(0, 360))
{
}

void CircleWave::render()
{
    float lineWidth = (endScale - innerScale > mainScale) ? mainScale : endScale - innerScale;
    DrawPolyLinesEx(position, 5, mainScale, startingAngle + (innerScale * 2.0f), lineWidth, color);
}

void CircleWave::update(float dt)
{
    mainScale = Lerp(mainScale, endScale, lerp * dt);
    innerScale = Lerp(innerScale, endScale, lerp / 3.0f * dt);
}

bool CircleWave::checkEndOfLife()
{
    if (std::abs(innerScale - endScale) < 2.0f)
    {
        // by removing the shared pointer from the list, we are essentially
        // deleting it (except it will only be deleted once its caller
        // removes it from its own scope, so after render is called)
        auto it = std::remove_if(Effect::effects.begin(), Effect::effects.end(),
                                 [this](const std::shared_ptr<Effect> &effect) { return effect.get() == this; });
        Effect::effects.erase(it, Effect::effects.end());

        return true;
    }

    return false;
}
