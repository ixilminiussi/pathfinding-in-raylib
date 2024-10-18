#pragma once
#include "raylib.h"
#include <memory>
#include <vector>

class Effect
{
  public:
    ~Effect() = default;

    virtual void update(const float &dt) = 0;
    virtual void render() = 0;
    virtual bool checkEndOfLife() = 0;

    static std::vector<std::shared_ptr<Effect>> effects;
    static void iterate();

  protected:
    Effect(const Vector2 &position, const float &scale, const float &lerp, const Color &color);

    Vector2 position;
    float scale;
    float lerp;
    Color color;
};

class CircleWave final : public Effect
{
  public:
    static std::shared_ptr<CircleWave> newEffect(const Vector2 &position, const float &scale, const float &lerp,
                                                 const Color &color);

    void update(const float &dt) override;
    void render() override;
    bool checkEndOfLife() override;

  private:
    CircleWave(const Vector2 &position, const float &scale, const float &lerp, const Color &color);

    float mainScale;
    float innerScale;
    float endScale;
    float startingAngle;
};
