#pragma once
// My own custom functions for raylib (because the default ones are annoying and
// bad and wrong and do unnecessary copies and use floats for everything and are
// allergic to operators

#include <raylib.h>
#include <raymath.h>

// Raylib 5.x+ already defines these operators in raymath.h under __cplusplus.
// Only define them for older versions to avoid redefinition errors.
#if RAYLIB_VERSION_MAJOR < 5

// Vector2Add(Vector2 v1, Vector2 v2)
inline Vector2 operator+(const Vector2 &v1, const Vector2 &v2)
{
    return {v1.x + v2.x, v1.y + v2.y};
}

inline const Vector2 &operator+=(Vector2 &v1, const Vector2 &v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    return v1;
}
// Vector2Subtract(Vector2 v1, Vector2 v2)
inline Vector2 operator-(const Vector2 &v1, const Vector2 &v2)
{
    return {v1.x - v2.x, v1.y - v2.y};
}

inline const Vector2 &operator-=(Vector2 &v1, const Vector2 &v2)
{
    v1.x -= v2.x;
    v1.y -= v2.y;
    return v1;
}
// Vector2Equals(Vector2 p, Vector2 q)
inline bool operator==(const Vector2 &v1, const Vector2 &v2)
{
    return (v1.x == v2.x) && (v1.y == v2.y);
}

inline bool operator!=(const Vector2 &v1, const Vector2 &v2)
{
    return !(v1 == v2);
}
// Vector2Scale(Vector2 v, float scale)
inline Vector2 operator*(const Vector2 &v, float scale)
{
    return {v.x * scale, v.y * scale};
}

inline const Vector2 &operator*=(Vector2 &v, float scale)
{
    v.x *= scale;
    v.y *= scale;
    return v;
}

#endif // RAYLIB_VERSION_MAJOR < 5
