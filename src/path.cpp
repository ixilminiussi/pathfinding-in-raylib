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

    const Node *nAP = graph->getBestNode(A);
    const Node *nBP = graph->getBestNode(B);

    // Ensure both start and end nodes are valid
    if (!nAP || !nBP)
        return false;

    Node *nA = new Node(*nAP);
    // Initialize G, H, and F values for start and end nodes
    float bigDistanceSqr = Vector2DistanceSqr(nA->getPosition(), nBP->getPosition());
    nA->G = 0;
    nA->H = nA->F = bigDistanceSqr;

    std::vector<Node *> toSearch = {nA};
    std::vector<Node *> processed;

    while (!toSearch.empty())
    {
        // Find the node with the lowest F value
        Node *current = *std::min_element(toSearch.begin(), toSearch.end(), [](Node *a, Node *b) {
            return (a->F < b->F) || (a->F == b->F && a->H < b->H);
        });

        processed.push_back(current);
        toSearch.erase(std::remove(toSearch.begin(), toSearch.end(), current), toSearch.end());

        // Found a valid path to the destination
        // TODO: Just because there is a straightline from there, doesn't mean
        // this is the shortest path.
        if (world->lineValidation(current->getPosition(), B))
        {
            Node *n = current;

            while (*n != *nA)
            {
                if (world->lineValidation(n->getPosition(), A))
                    break;

                Node *nSkip = n;
                while (*nSkip != *nA &&
                       world->lineValidation(nSkip->connectionBackward->getPosition(), n->getPosition()))
                {
                    nSkip = nSkip->connectionBackward;
                    n->connectionBackward = nSkip;
                    nSkip->connectionForward = n;
                }

                n = n->connectionBackward;
                segmentCount++;
            }

            // Create path segments
            segments[0] = {A, n->getPosition(), 1};

            for (int i = 1; i <= segmentCount; i++)
            {
                segments[i] = {n->getPosition(), n->connectionForward->getPosition(), i + 1};
                n = n->connectionForward;
            }

            segments[segmentCount + 1] = {n->getPosition(), B, segmentCount + 2};
            segmentCount += 2;

            for (Node *n : toSearch)
            {
                if (n != nullptr)
                    delete n;
            }
            toSearch.clear();

            for (Node *n : processed)
            {
                if (n != nullptr)
                    delete n;
            }
            processed.clear();

            return true;
        }

        // Process all neighbours
        for (int i = 0; i < current->neighbourCount; i++)
        {
            Node *neighbour = new Node(*(current->neighbours[i]));

            // Skip if already processed
            if (std::find_if(processed.begin(), processed.end(), [neighbour](Node *n) { return *neighbour == *n; }) !=
                processed.end())
            {
                delete neighbour;
                continue;
            }

            auto it = std::find_if(toSearch.begin(), toSearch.end(), [neighbour](Node *n) { return *neighbour == *n; });

            if (it != toSearch.end())
            {
                delete neighbour;
                neighbour = *(it);
            }

            float costToNeighbour = current->G + graph->outerRadius * 2.0f;

            // Update G, H, F values if not in search or found a better path
            if (it == toSearch.end() || costToNeighbour < neighbour->G)
            {
                neighbour->G = costToNeighbour;
                neighbour->connectionBackward = current;

                if (it == toSearch.end())
                {
                    neighbour->H = Vector2DistanceSqr(neighbour->getPosition(), nBP->getPosition());
                    neighbour->F = neighbour->G + neighbour->H;
                    toSearch.push_back(neighbour);
                }
            }
        }
    }

    for (Node *n : toSearch)
    {
        if (n != nullptr)
            delete n;
    }
    toSearch.clear();

    for (Node *n : processed)
    {
        if (n != nullptr)
            delete n;
    }
    processed.clear();

    return false;
}

/* bool Path::dijkstraAlgorithm(const Vector2 &A, const Vector2 &B)
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
        segments[i] = {n->getPosition(),
n->connectionForward->getPosition(), i
+ 1}; n = n->connectionForward;
    }
    segments[segmentCount + 1] = {n->getPosition(), B, segmentCount + 2};
    segmentCount += 2;

    return true;
}*/

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
