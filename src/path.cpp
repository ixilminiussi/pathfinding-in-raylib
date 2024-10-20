#include "path.h"
#include "graph.h"
#include "namespaces.h"
#include "node.h"
#include "world.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <raylib.h>
#include <raymath.h>
#include <rayplus.h>
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

std::unique_ptr<Path> Path::newPath(const Vector2 &A, const Vector2 &B)
{
    std::unique_ptr<Path> path = std::unique_ptr<Path>(new Path(A, B));
    return path;
}

void Path::render() const
{
    Color yellow = ColorAlphaBlend(shoshone::maroon, shoshone::yellow, Color{150, 150, 150, 150});

    for (int i = 0; i < segmentCount; i++)
    {
        DrawLineEx(getSegment(i).A, getSegment(i).B, 3.0f, {yellow.r, yellow.g, yellow.b, 100});

        DrawCircleV(getSegment(i).A, 4.0f, yellow);
        if (i == segmentCount - 1)
        {
            DrawCircleV(getSegment(i).B, 6.0f, yellow);
        }
    }
}

bool Path::isValid()
{
    return segmentCount > 0;
}

Force Path::getDirectionFromNearby(const Vector2 &C) const
{
    float closestDistanceSqr = std::numeric_limits<float>::max();
    Force closestPoint = {Vector2Zero(), Vector2Zero()};

    for (int i = 0; i < segmentCount; i++)
    {
        const Segment &currentSegment = getSegment(i);
        Force currentClosestPoint = getProjectedPointOnSegment(currentSegment, C);
        float currentDistanceSqr = Vector2DistanceSqr(currentClosestPoint.origin, C);
        if (currentDistanceSqr <= closestDistanceSqr)
        {
            closestDistanceSqr = currentDistanceSqr;
            closestPoint = currentClosestPoint;
        }
    }

    return closestPoint;
}

bool Path::aStarAlgorithm(const Vector2 &A, const Vector2 &B)
{
    bool pathFound = false;
    float bestF = std::numeric_limits<float>::infinity();
    Node *bestNode;

    Graph *graph = Graph::getInstance();
    World *world = World::getInstance();

    const Node *nAP = graph->getBestNode(A);
    const Node *nBP = graph->getBestNode(B);

    // Ensure both start and end nodes are valid
    if (!nAP || !nBP)
        return false;

    Node *nA = new Node(*nAP);
    // Initialize G, H, and F values for start and end nodes
    nA->setG(0.0f);
    nA->setH(Vector2Distance(nA->getPosition(), B));

    std::vector<Node *> toSearch = {nA};
    std::vector<Node *> processed;

    // ============================
    // MAIN LOOP
    // ============================
    while (!toSearch.empty())
    {
        // if we found a path, only stop searching once there is no potential
        // better candidate left;
        // Find the node with the lowest F value
        Node *current = *std::min_element(toSearch.begin(), toSearch.end(), [](Node *a, Node *b) {
            return (a->getF() < b->getF()) || (a->getF() == b->getF() && a->getH() < b->getH());
        });

        processed.push_back(current);
        toSearch.erase(std::remove(toSearch.begin(), toSearch.end(), current), toSearch.end());

        // Found a valid path to the destination
        // TODO: Just because there is a straightline from there, doesn't mean
        // this is the shortest path.
        if (world->lineValidation(current->getPosition(), B))
        {
            pathFound = true;
            if (current->getF() < bestF)
            {
                bestNode = current;
                bestF = current->getF();
            }
        }

        // if the best F found is still worst than the current best found Path,
        // stop searching
        // ============================
        // Backpropagation through solution
        // ============================
        if (pathFound && bestF <= current->getF() && bestNode != current)
        {
            Node *n = bestNode;

            while (true)
            {
                if (world->lineValidation(n->getPosition(), A))
                {
                    addSegment({A, n->getPosition()});
                    break;
                }

                Node *nSkip = n;
                while (*nSkip != *nA &&
                       world->lineValidation(nSkip->getConnectionBackward()->getPosition(), n->getPosition()))
                {
                    nSkip = nSkip->getConnectionBackward();
                    n->setConnectionBackward(nSkip);
                    nSkip->setConnectionForward(n);
                }

                n = n->getConnectionBackward();
            }

            while (n != bestNode)
            {
                addSegment({n->getPosition(), n->getConnectionForward()->getPosition()});
                n = n->getConnectionForward();
            }
            addSegment({bestNode->getPosition(), B});

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

        // ============================
        // Process all neighbours
        // ============================
        for (int i = 0; i < current->getNeighbourCount(); i++)
        {
            Node *neighbour = new Node(*(current->getNeighbour(i)));

            // Skip if already processed
            if (std::find_if(processed.begin(), processed.end(), [neighbour](Node *n) { return *neighbour == *n; }) !=
                processed.end())
            {
                delete neighbour;
                continue;
            }

            // Changed to instance in search if node is already present
            auto it = std::find_if(toSearch.begin(), toSearch.end(), [neighbour](Node *n) { return *neighbour == *n; });

            if (it != toSearch.end())
            {
                delete neighbour;
                neighbour = *(it);
            }

            // Calculate the current cost to said node from our current path
            float costToNeighbour = current->getG() + graph->outerRadius * 2.0f;
            neighbour->setH(Vector2Distance(neighbour->getPosition(), B));

            if (costToNeighbour < neighbour->getG())
            {
                neighbour->setG(costToNeighbour);
                neighbour->setConnectionBackward(current);
            }

            // Update G, H, F values if not in search or found a better path
            if (it == toSearch.end())
            {
                neighbour->setH(Vector2Distance(neighbour->getPosition(), nBP->getPosition()));
                toSearch.push_back(neighbour);
            }
        }
    }

    // ============================
    // Delete node copies
    // ============================
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
                nSkip->getConnectionForward() = n;
            }
            else
            {
                n->connectionsBackward[index] = nSkip;
                nSkip->getConnectionForward() = n;
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
n->getConnectionForward()->getPosition(), i
+ 1}; n = n->getConnectionForward();
    }
    segments[segmentCount + 1] = {n->getPosition(), B, segmentCount + 2};
    segmentCount += 2;

    return true;
}*/

bool Path::straightSegmentAlgorithm(const Vector2 &A, const Vector2 &B)
{
    if (World::getInstance()->lineValidation(A, B))
    {
        addSegment({A, B, 1});
        return true;
    }
    return false;
}

Force Path::getProjectedPointOnSegment(const Segment &segment, const Vector2 &P) const
{
    const Vector2 &A = segment.A;
    const Vector2 &B = segment.B;
    Vector2 direction = Vector2Normalize(B - A);
    // in case segment is very small
    float segmentLength = Vector2DistanceSqr(A, B);
    if (segmentLength <= 1.0f)
    {
        return {A, P - A};
    }

    float pDotA = Vector2DotProduct(P - A, B - A);
    float param = pDotA / segmentLength;

    if (param <= 0)
    {
        return {A, direction};
    }
    if (param >= 1)
    {
        if (B == end)
        {
            return {B, B - P};
        }
        else
        {
            return getProjectedPointOnSegment(getSegment(segment.index + 1), P);
        }
    }
    return {A + ((B - A) * param), direction};
}

const Segment &Path::getSegment(int index) const
{
    if (index < sizeof(segments) / sizeof(Segment))
    {
        return segments[index];
    }
    else
    {
        return furtherSegments.at(index - (sizeof(segments) / sizeof(Segment))); // TODO: make it retrieve from vector
    }
}

void Path::addSegment(Segment segment)
{
    segment.index = segmentCount;

    if (segmentCount >= sizeof(segments) / sizeof(Segment))
    {
        furtherSegments.push_back(segment);
    }
    else
    {
        segments[segmentCount] = segment;
    }

    segmentCount++;
}
