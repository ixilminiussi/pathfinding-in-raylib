#pragma once
#include "raylib.h"
#include <vector>

class Effect
{
  public:
    Effect() = default;
    Effect(const Vector2 &position, const float &scale, const float &lerp, const Color &color);
    ~Effect();

    virtual void update(const float &dt) = 0;
    virtual void render() = 0;

    static std::vector<Effect *> effects;

  protected:
    Vector2 position;
    float scale;
    float lerp;
    Color color;
};

class CircleWave : public Effect
{
  public:
    CircleWave() = default;
    CircleWave(const Vector2 &position, const float &scale, const float &lerp, const Color &color);

    void update(const float &dt) override;
    void render() override;

  private:
    float mainScale;
    float innerScale;
    float endScale;
    float startingAngle;
};
