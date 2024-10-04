#include "path.h"
#include "raylib.h"
#include "raymath.h"
#include "world.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>

std::vector<Path *> Path::bakedPaths = std::vector<Path *>();

Path::Path(const Vector2 &A, const Vector2 &B)
{
    start = A;
    end = B;
    if (World::getInstance()->lineValidation(A, B))
        straightSegmentAlgorithm(A, B);
}

Path *Path::newPath(const Vector2 &A, const Vector2 &B)
{
    for (Path *p : bakedPaths)
    {
        if (p->isCloseEnough(A, B))
        {
            // TODO: move p back to front of queue;
            return p;
        }
    }
    Path *p = new Path(A, B);

    bakedPaths.insert(bakedPaths.begin(), p);

    if (bakedPaths.size() > 1000)
    {
        Path *lastPath = bakedPaths.at(bakedPaths.size());
        bakedPaths.pop_back();
        delete lastPath;
    }

    return p;
}

Path::~Path()
{
}

void Path::render()
{
    for (int i = 0; i < segmentCount; i++)
    {
        DrawLineEx(getSegment(i)->A, getSegment(i)->B, 1.0f, WHITE);
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

void Path::straightSegmentAlgorithm(const Vector2 &A, const Vector2 &B)
{
    segmentCount = 1;
    segments[0] = {A, B, 1};
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
    if (Vector2DistanceSqr(B, end) < 400.0f)
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
