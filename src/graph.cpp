#include "graph.h"
#include "namespaces.h"
#include "node.h"
#include "raylib.h"
#include "world.h"
#include <cmath>
#include <limits>
#include <raymath.h>

Graph *Graph::instance = nullptr;

Graph *Graph::getInstance() {
    if (instance == nullptr) {
        instance = new Graph();
    }

    return instance;
}

void Graph::refresh() {
    delete instance;
    instance = new Graph();
}

Graph::Graph() {
    resolutionX = game::GRAPH_RESOLUTION;

    float width = screen::WIDTH / ((float)resolutionX - 1);
    float innerRadius = width / 2.0f;
    outerRadius = width / std::sqrt(3.0f);

    resolutionY = std::floor(screen::HEIGHT / (1.25f * innerRadius));

    typedef Node *NodeP;
    nodes = new NodeP[resolutionY * resolutionX];

    Vector2 offset = {innerRadius, 0.25f * outerRadius};
    for (int x = 0; x < resolutionX; x++) {
        for (int y = 0; y < resolutionY; y++) {
            nodes[y * resolutionX + x] =
                (y % 2 == 0)
                    ? new Node(Vector2Add(offset, {innerRadius * 2.0f * x,
                                                   outerRadius * 1.5f * y}),
                               x, y)
                    : new Node(Vector2Add(offset, {innerRadius * (2.0f * x - 1),
                                                   outerRadius * 1.5f * y}),
                               x, y);
        }
    }

    generateEdges();
    /*generateWeights();*/
}

Graph::~Graph() { delete[] nodes; }

void Graph::generateEdges() {
    World *world = World::getInstance();

    for (int x = 0; x < resolutionX; x++) {
        for (int y = 0; y < resolutionY; y++) {
            Node *potentialNeighbours[6] = {
                (y % 2 == 0) ? getNode(x, y - 1) : getNode(x - 1, y - 1),
                getNode(x - 1, y),
                (y % 2 == 0) ? getNode(x, y + 1) : getNode(x - 1, y + 1),
                (y % 2 == 0) ? getNode(x + 1, y - 1) : getNode(x, y - 1),
                getNode(x + 1, y),
                (y % 2 == 0) ? getNode(x + 1, y + 1) : getNode(x, y + 1)};

            for (int i = 0; i < 6; i++) {
                if (potentialNeighbours[i] != nullptr) {
                    if (world->lineValidation(
                            getNode(x, y)->getPosition(),
                            potentialNeighbours[i]->getPosition())) {
                        getNode(x, y)->addNeighbour(potentialNeighbours[i]);
                    }
                }
            }
        }
    }
}

Node *Graph::getNode(int x, int y) const {
    return (x >= 0 && x < resolutionX && y >= 0 && y < resolutionY)
               ? nodes[y * resolutionX + x]
               : nullptr;
}

Node *Graph::getBestNode(const Vector2 &P) const {
    Node *bestNode;
    float bestDistanceSqr = std::numeric_limits<float>::infinity();

    World *world = World::getInstance();

    for (int x = 0; x < resolutionX; x++) {
        for (int y = 0; y < resolutionY; y++) {
            Node *currentNode = getNode(x, y);
            if (Vector2DistanceSqr(currentNode->getPosition(), P) >
                bestDistanceSqr) {
                continue;
            }

            float currentDistanceSqr;
            if (world->lineValidation(currentNode->getPosition(), P)) {
                currentDistanceSqr =
                    Vector2DistanceSqr(currentNode->getPosition(), P);
            } else {
                currentDistanceSqr = std::numeric_limits<float>::infinity();
            }
            if (currentDistanceSqr < bestDistanceSqr) {
                bestNode = currentNode;
            }

            if (currentDistanceSqr <= outerRadius * outerRadius) {
                return bestNode;
            }
        }
    }

    return bestNode;
}

void Graph::render() {
    return;
    for (int x = 0; x < resolutionX; x++) {
        for (int y = 0; y < resolutionY; y++) {
            nodes[y * resolutionX + x]->render();
        }
    }
}
