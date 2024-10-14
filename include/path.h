#pragma once
#include "raylib.h"

struct Segment
{
    Vector2 A;
    Vector2 B;
    int index;
};

struct Force
{
    Vector2 origin;
    Vector2 direction;
};

class Path
{
  public:
    ~Path() = default;
    Path(const Path &) = delete;
    Path operator=(const Path &) = delete;

    static Path *newPath(const Vector2 &A, const Vector2 &B);

    void render();
    Force getDirectionFromNearby(const Vector2 &C);

  private:
    Path() = delete;
    Path(const Vector2 &A, const Vector2 &B);

    // algorithms
    bool aStarAlgorithm(const Vector2 &A, const Vector2 &B);
    bool straightSegmentAlgorithm(const Vector2 &A, const Vector2 &B);
    /**
     * NOT THE ACTUAL IMPLEMENTATION OF DIJKSTRA
     * the actual implementation is in Graph, where we bake in all the necessary
     * into the nodes. This function simply reads those values
     */
    // bool dijkstraAlgorithm(const Vector2 &A, const Vector2 &B);

    Force getProjectedPointOnSegment(const Segment &segment, const Vector2 &P);

    Segment *getSegment(int index);

    // bool isCloseEnough(const Vector2 &A, const Vector2 &B);

    Vector2 start;
    Vector2 end;

    Segment segments[100];
    int segmentCount;
};
