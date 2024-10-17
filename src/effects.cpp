#include "effects.h"
#include "raylib.h"
#include <algorithm>
#include <raymath.h>

std::vector<Effect *> Effect::effects = std::vector<Effect *>();

Effect::Effect(const Vector2 &position, const float &scale, const float &lerp, const Color &color)
    : position(position), scale(scale), lerp(lerp), color(color)
{
    effects.push_back(this);
}

Effect::~Effect()
{
    effects.erase(std::remove(effects.begin(), effects.end(), this), effects.end());
}

CircleWave::CircleWave(const Vector2 &position, const float &scale, const float &lerp, const Color &color)
    : Effect(position, scale, lerp, color), mainScale(1.0f * scale), innerScale(0.0f), endScale(10.0f * scale),
      startingAngle((float)GetRandomValue(0, 360))
{
}

void CircleWave::render()
{
    float lineWidth = (endScale - innerScale > mainScale) ? mainScale : endScale - innerScale;
    DrawPolyLinesEx(position, 5, mainScale, startingAngle + (innerScale * 2.0f), lineWidth, color);

    if (std::abs(innerScale - endScale) < 2.0f)
    {
        delete this;
    }
}

void CircleWave::update(const float &dt)
{
    mainScale = Lerp(mainScale, endScale, lerp * dt);
    innerScale = Lerp(innerScale, endScale, lerp / 3.0f * dt);
}
