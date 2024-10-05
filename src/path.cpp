#include "path.h"
#include "namespaces.h"
#include "raylib.h"
#include "raymath.h"
#include "world.h"
#include <cmath>
#include <limits>

// std::vector<Path *> Path::bakedPaths = std::vector<Path *>();

Path::Path(const Vector2 &A, const Vector2 &B) : users(0)
{
    start = A;
    end = B;
    if (!straightSegmentAlgorithm(A, B))
    {
        segmentCount = 0;
    }
}

Path *Path::newPath(const Vector2 &A, const Vector2 &B)
{
    Path *path;
    // if (!World::getInstance()->lineValidation(A, B))
    // {
    // for (Path *p : bakedPaths)
    // {
    //     if (p->isCloseEnough(A, B))
    //     {
    //         // TODO: move p back to front of queue;
    //         return p;
    //     }
    // }
    //    path = new Path(A, B);
    // }
    // else
    // {
    // }

    path = new Path(A, B);

    if (path->segmentCount == 0)
    {
        delete path;
        return nullptr;
    }

    // bakedPaths.insert(bakedPaths.begin(), path);

    // if (bakedPaths.size() > 1000)
    // {
    //     Path *lastPath = bakedPaths.at(bakedPaths.size() - 1);
    //     bakedPaths.pop_back();
    //     delete lastPath;
    // }
    return path;
}

Path::~Path()
{
}

void Path::hello()
{
    users++;
}

void Path::goodbye()
{
    users--;
}

void Path::render()
{
    Color yellow = ColorAlphaBlend(shoshone::maroon, shoshone::yellow, Color{150, 150, 150, 150});

    for (int i = 0; i < segmentCount; i++)
    {
        DrawLineEx(getSegment(i)->A, getSegment(i)->B, 3.0f, {yellow.r, yellow.g, yellow.b, 100});

        DrawCircleV(getSegment(i)->A, 4.0f, yellow);
        if (i == segmentCount - 1)
        {
            DrawCircleV(getSegment(i)->B, 6.0f, yellow);
        }
    }
}

Force Path::getDirectionFromNearby(const Vector2 &C)
{
    Segment *closestSegment;
    float closestDistanceSqr = std::numeric_limits<float>::max();
    Force closestPoint = {Vector2Zero(), Vector2Zero()};

    for (int i = 0; i < segmentCount; i++)
    {
        Segment *currentSegment = getSegment(i);
        Force currentClosestPoint = getProjectedPointOnSegment(*currentSegment, C);
        float currentDistanceSqr = Vector2DistanceSqr(currentClosestPoint.origin, C);
        if (currentDistanceSqr <= closestDistanceSqr)
        {
            closestSegment = currentSegment;
            closestDistanceSqr = currentDistanceSqr;
            closestPoint = currentClosestPoint;
        }
    }

    return closestPoint;
}

bool Path::straightSegmentAlgorithm(const Vector2 &A, const Vector2 &B)
{
    if (World::getInstance()->lineValidation(A, B))
    {
        segmentCount = 1;
        segments[0] = {A, B, 1};
        return true;
    }
    return false;
}

Force Path::getProjectedPointOnSegment(const Segment &segment, const Vector2 &P)
{
    const Vector2 &A = segment.A;
    const Vector2 &B = segment.B;
    Vector2 direction = Vector2Normalize(Vector2Subtract(B, A));
    // in case segment is very small
    float segmentLength = Vector2DistanceSqr(A, B);
    if (segmentLength <= 1.0f)
    {
        return {A, Vector2Subtract(P, A)};
    }

    float pDotA = Vector2DotProduct(Vector2Subtract(P, A), Vector2Subtract(B, A));
    float param = pDotA / segmentLength;

    if (param <= 0)
    {
        return {A, direction};
    }
    if (param >= 1)
    {
        if (Vector2Equals(B, end))
        {
            return {B, Vector2Subtract(B, P)};
        }
        else
        {
            return getProjectedPointOnSegment(*getSegment(segment.index + 1), P);
        }
    }
    return {Vector2Add(A, Vector2Scale(Vector2Subtract(B, A), param)), direction};
}

bool Path::isCloseEnough(const Vector2 &A, const Vector2 &B)
{
    if (Vector2DistanceSqr(B, end) < 4000.0f)
    {
        if (Vector2DistanceSqr(getDirectionFromNearby(A).origin, A) < 4000.0f)
        {
            return true;
        }
    }
    return false;
}

Segment *Path::getSegment(int index)
{
    if (index < segmentCount)
    {
        return &segments[index];
    }
    else
    {
        return &segments[index]; // TODO: make it retrieve from vector when
                                 // segment
    }
}
