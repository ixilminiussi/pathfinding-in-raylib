#include "path.h"
#include "effects.h"
#include "graph.h"
#include "namespaces.h"
#include "node.h"
#include "raylib.h"
#include "raymath.h"
#include "world.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

// std::vector<Path *> Path::bakedPaths = std::vector<Path *>();

Path::Path(const Vector2 &A, const Vector2 &B) : segmentCount(0)
{
    start = A;
    end = B;
    if (!straightSegmentAlgorithm(A, B))
    {
        /*dijkstraAlgorithm(A, B);*/
        aStarAlgorithm(A, B);
    }
}

Path *Path::newPath(const Vector2 &A, const Vector2 &B)
{
    Path *path = new Path(A, B);

    if (path->segmentCount == 0)
    {
        delete path;
        return nullptr;
    }

    return path;
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

bool Path::aStarAlgorithm(const Vector2 &A, const Vector2 &B)
{
    Graph *graph = Graph::getInstance();
    World *world = World::getInstance();

    Node *nA = graph->getBestNode(A);
    Node *nB = graph->getBestNode(B);
    if (nA == nullptr || nB == nullptr)
    {
        return false;
    }

    float bigDistanceSqr = Vector2DistanceSqr(nA->getPosition(), nB->getPosition());

    nA->G = 0;
    nA->H = bigDistanceSqr;
    nA->F = bigDistanceSqr;

    nB->G = bigDistanceSqr;
    nB->H = 0;
    nB->F = bigDistanceSqr;

    std::vector<Node *> toSearch = std::vector<Node *>();
    std::vector<Node *> processed = std::vector<Node *>();
    toSearch.push_back(nA);

    while (toSearch.size() > 0)
    {
        Node *current = toSearch.at(0);

        for (Node *n : toSearch)
        {
            if (n->F < current->F || (n->F == current->F && n->H < current->H))
            {
                current = n;
            }
        }

        processed.push_back(current);
        toSearch.erase(std::remove(toSearch.begin(), toSearch.end(), current), toSearch.end());

        // found a connection
        if (world->lineValidation(current->getPosition(), B))
        {
            // backpropagation
            Node *n = current;
            while (n != nA)
            {
                if (world->lineValidation(n->getPosition(), A))
                {
                    break;
                }
                Node *nSkip = n;
                do
                {
                    if (world->lineValidation(nSkip->connectionBackward->getPosition(), n->getPosition()))
                    {
                        nSkip = nSkip->connectionBackward;
                        n->connectionBackward = nSkip;
                        nSkip->connectionForward = n;
                    }
                    else
                    {
                        n->connectionBackward = nSkip;
                        nSkip->connectionForward = n;
                        break;
                    }

                } while (nSkip != nA);
                n = n->connectionBackward;
                segmentCount++;
            }

            segments[0] = {A, n->getPosition(), 1};

            for (int i = 1; i <= segmentCount; i++)
            {
                segments[i] = {n->getPosition(), n->connectionForward->getPosition(), i + 1};
                n = n->connectionForward;
            }
            segments[segmentCount + 1] = {n->getPosition(), B, segmentCount + 2};
            segmentCount += 2;

            return true;
        }

        for (int i = 0; i < current->neighbourCount; i++)
        {
            Node *n = current->neighbours[i];
            bool inProcessed = std::find(processed.begin(), processed.end(), n) != processed.end();
            if (inProcessed)
                continue;

            bool inSearch = std::find(toSearch.begin(), toSearch.end(), n) != toSearch.end();

            float costToNeighbour = current->G + graph->outerRadius * 2.0f;

            if (!inSearch || costToNeighbour < n->G)
            {
                n->G = costToNeighbour;
                n->connectionBackward = current;

                if (!inSearch)
                {
                    n->H = Vector2DistanceSqr(n->getPosition(), nB->getPosition());
                    n->F = n->H + n->G;
                    toSearch.push_back(n);
                }
            }
        }
    }

    return false;
}

bool Path::dijkstraAlgorithm(const Vector2 &A, const Vector2 &B)
{
    Graph *graph = Graph::getInstance();
    World *world = World::getInstance();

    Node *nA = graph->getBestNode(A);
    Node *nB = graph->getBestNode(B);
    if (nA == nullptr || nB == nullptr)
    {
        return false;
    }
    int index = graph->getIndex(nA->x, nA->y);

    if (nB->connectionsBackward[index] == nullptr)
    {
        return false;
    }

    // backpropagation
    Node *n = nB;

    while (n != nA)
    {
        if (world->lineValidation(n->getPosition(), A))
        {
            break;
        }
        Node *nSkip = n;
        do
        {
            Vector2 P = nSkip->connectionsBackward[index]->getPosition();
            if (world->lineValidation(P, n->getPosition()))
            {
                nSkip = nSkip->connectionsBackward[index];
                n->connectionsBackward[index] = nSkip;
                nSkip->connectionForward = n;
            }
            else
            {
                n->connectionsBackward[index] = nSkip;
                nSkip->connectionForward = n;
                break;
            }

        } while (nSkip != nA);
        n = n->connectionsBackward[index];
        segmentCount++;
    }

    segments[0] = {A, n->getPosition(), 1};

    for (int i = 1; i <= segmentCount; i++)
    {
        segments[i] = {n->getPosition(), n->connectionForward->getPosition(), i + 1};
        n = n->connectionForward;
    }
    segments[segmentCount + 1] = {n->getPosition(), B, segmentCount + 2};
    segmentCount += 2;

    return true;
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
