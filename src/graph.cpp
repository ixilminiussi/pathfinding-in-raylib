#include "graph.h"
#include "namespaces.h"
#include "node.h"
#include "raylib.h"
#include "world.h"
#include <cmath>
#include <limits>
#include <raymath.h>

Graph *Graph::instance = nullptr;

Graph *Graph::getInstance()
{
    if (instance == nullptr)
    {
        instance = new Graph();
    }

    return instance;
}

void Graph::refresh()
{
    delete instance;
    instance = new Graph();
}

Graph::Graph()
{
    resolutionX = game::GRAPH_RESOLUTION;

    float width = (float)screen::WIDTH / ((float)resolutionX - 1.0f);
    float innerRadius = width / 2.0f;
    outerRadius = width / std::sqrt(3.0f);

    // FIX: incorrect, leads to overflowing grid (but not really)
    resolutionY = std::floor(screen::HEIGHT / (1.5f * innerRadius));

    typedef Node *NodeP;
    nodes = new NodeP[resolutionY * resolutionX];

    Vector2 offset = {innerRadius, 0.25f * outerRadius};
    for (int x = 0; x < resolutionX; x++)
    {
        for (int y = 0; y < resolutionY; y++)
        {
            if (y % 2 == 0)
            {
                nodes[y * resolutionX + x] =
                    new Node(Vector2Add(offset, {innerRadius * 2.0f * x, outerRadius * 1.5f * y}), x, y);
            }
            else
            {
                nodes[y * resolutionX + x] =
                    new Node(Vector2Add(offset, {innerRadius * (2.0f * x - 1), outerRadius * 1.5f * y}), x, y);
            }
        }
    }

    generateEdges();
    // generateWeights();
}

Graph::~Graph()
{
    for (int x = 0; x < resolutionX; x++)
    {
        for (int y = 0; y < resolutionY; y++)
        {
            delete nodes[y * resolutionX + x];
        }
    }
    delete[] nodes;
}

void Graph::generateEdges()
{
    World *world = World::getInstance();

    for (int x = 0; x < resolutionX - 1; x++)
    {
        for (int y = 0; y < resolutionY - 1; y++)
        {
            const Node *potentialNeighbours[6] = {(y % 2 == 0) ? getNode(x, y - 1) : getNode(x - 1, y - 1),
                                                  getNode(x - 1, y),
                                                  (y % 2 == 0) ? getNode(x, y + 1) : getNode(x - 1, y + 1),
                                                  (y % 2 == 0) ? getNode(x + 1, y - 1) : getNode(x, y - 1),
                                                  getNode(x + 1, y),
                                                  (y % 2 == 0) ? getNode(x + 1, y + 1) : getNode(x, y + 1)};

            for (int i = 0; i < 6; i++)
            {
                if (potentialNeighbours[i] != nullptr)
                {
                    if (world->lineValidation(getNode(x, y)->getPosition(), potentialNeighbours[i]->getPosition()))
                    {
                        unsafeGetNode(x, y)->addNeighbour(potentialNeighbours[i]);
                    }
                }
            }
        }
    }
}

const Node *Graph::getNode(int x, int y) const
{
    return (x >= 0 && x < resolutionX && y >= 0 && y < resolutionY) ? nodes[y * resolutionX + x] : nullptr;
}

Node *Graph::unsafeGetNode(int x, int y)
{
    return (x >= 0 && x <= resolutionX && y >= 0 && y <= resolutionY) ? nodes[y * resolutionX + x] : nullptr;
}

const Node *Graph::getBestNode(const Vector2 &P) const
{
    const Node *bestNode = nullptr;
    float bestDistanceSqr = std::numeric_limits<float>::infinity();

    World *world = World::getInstance();

    for (int x = 0; x < resolutionX; x++)
    {
        for (int y = 0; y < resolutionY; y++)
        {
            const Node *currentNode = getNode(x, y);
            if (Vector2DistanceSqr(currentNode->getPosition(), P) >
                std::min(2.0f * (outerRadius * outerRadius), bestDistanceSqr))
            {
                continue;
            }

            float currentDistanceSqr =
                (world->lineValidation(currentNode->getPosition(), P, false))
                    ? Vector2DistanceSqr(currentNode->getPosition(), P)
                    : std::numeric_limits<float>::max(); // (just an inch lower than infinity, so will
                                                         // still pass unless there is another node
                                                         // within the range but with a valid line)
            if (currentDistanceSqr <= bestDistanceSqr)
            {
                bestNode = currentNode;
                bestDistanceSqr = currentDistanceSqr;
            }
        }
    }

    return bestNode;
}

void Graph::render() const
{
    return;
    for (int x = 0; x < resolutionX; x++)
    {
        for (int y = 0; y < resolutionY; y++)
        {
            nodes[y * resolutionX + x]->render();
        }
    }
}

int Graph::getIndex(int x, int y) const
{
    return y * resolutionX + x;
}

/** Uses Dijkstra's algorithm to generate paths from all nodes to all nodes
 */
/* void Graph::generateWeights()
{
    World *world = World::getInstance();

    for (int x = 0; x < resolutionX; x++)
    {
        for (int y = 0; y < resolutionY; y++)
        {
            int index = y * resolutionX + x;

            Node *nA = getNode(x, y);
            if (nA == nullptr)
                continue;

            nA->G = 0;

            std::vector<Node *> toSearch = std::vector<Node *>();
            std::vector<Node *> processed = std::vector<Node *>();
            toSearch.push_back(nA);

            while (toSearch.size() > 0)
            {
                Node *current = toSearch.at(0);

                for (Node *n : toSearch)
                {
                    if (n->G < current->G)
                    {
                        current = n;
                    }
                }

                processed.push_back(current);
                toSearch.erase(std::remove(toSearch.begin(), toSearch.end(),
current), toSearch.end());

                for (int i = 0; i < current->neighbourCount; i++)
                {
                    Node *n = current->neighbours[i];
                    bool inProcessed = std::find(processed.begin(),
processed.end(), n) != processed.end(); if (inProcessed) continue;

                    bool inSearch = std::find(toSearch.begin(), toSearch.end(),
n) != toSearch.end();

                    float costToNeighbour = current->G + outerRadius * 2;

                    if (!inSearch || costToNeighbour < n->G)
                    {
                        n->G = costToNeighbour;
                        n->connectionsBackward[index] = current;

                        if (!inSearch)
                        {
                            toSearch.push_back(n);
                        }
                    }
                }
            }
        }
    }
}*/
