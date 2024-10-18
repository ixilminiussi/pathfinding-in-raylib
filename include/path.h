#pragma once
#include "raylib.h"
#include <memory>
#include <vector>

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

    static std::unique_ptr<Path> newPath(const Vector2 &A, const Vector2 &B);

    void render() const;
    Force getDirectionFromNearby(const Vector2 &C) const;
    bool isValid();

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

    Force getProjectedPointOnSegment(const Segment &segment, const Vector2 &P) const;

    const Segment &getSegment(int index) const;
    void addSegment(Segment segment);

    // bool isCloseEnough(const Vector2 &A, const Vector2 &B);

    Vector2 start;
    Vector2 end;

    Segment segments[10];
    std::vector<Segment> furtherSegments = std::vector<Segment>();
    int segmentCount;
};
