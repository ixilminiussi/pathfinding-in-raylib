#include "node.h"
#include "tile.h"
#include "world.h"
#include <limits>
#include <string>

Node::Node(const Vector2 &position, int x, int y) : neighbourCount(0), G(0), H(0), F(0), position(position), x(x), y(y)
{
}

Node::Node(const Node &other) : Node(other.position, other.x, other.y)
{
    neighbourCount = other.neighbourCount;
    for (int i = 0; i < neighbourCount; i++)
    {
        neighbours[i] = other.neighbours[i];
    }

    G = std::numeric_limits<float>::infinity();
}

Node &Node::operator=(const Node &other)
{
    if (this != &other)
    {
        position = other.position;
        x = other.x;
        y = other.y;
        neighbourCount = other.neighbourCount;

        for (int i = 0; i < neighbourCount; i++)
        {
            neighbours[i] = other.neighbours[i];
        }
    }

    G = std::numeric_limits<float>::infinity();

    return *this;
}

bool Node::operator==(const Node &other) const
{
    return (x == other.x && y == other.y && neighbourCount == other.neighbourCount);
}

bool Node::operator!=(const Node &other) const
{
    return !(*this == other);
}

void Node::render()
{
    for (int i = 0; i < neighbourCount; i++)
    {
        DrawLineEx(position, neighbours[i]->getPosition(), 3.0f, {255, 255, 255, 20});
    }
}

const Vector2 &Node::getPosition() const
{
    return position;
}

void Node::addNeighbour(const Node *node)
{
    neighbours[neighbourCount] = node;
    neighbourCount++;
}

void Node::setG(float newG)
{
    G = newG;
    F = G + H;
}

void Node::setH(float newH)
{
    H = newH;
    F = G + H;
}

void Node::setConnectionBackward(Node *node)
{
    connectionBackward = node;
}

void Node::setConnectionForward(Node *node)
{
    connectionForward = node;
}

float Node::getG() const
{
    return G;
}

float Node::getH() const
{
    return H;
}

float Node::getF() const
{
    return F;
}

const Node *Node::getNeighbour(int index) const
{
    return neighbours[index];
}

int Node::getNeighbourCount() const
{
    return neighbourCount;
}

Node *Node::getConnectionBackward()
{
    return connectionBackward;
}

Node *Node::getConnectionForward()
{
    return connectionForward;
}

bool Node::inWall() const
{
    World *world = World::getInstance();
    Vector2 coords = world->getWorldAddress(position);
    const Tile *tile = world->getTile((int)coords.x, (int)coords.y);

    if (tile == nullptr)
        return false;

    return !tile->isNavigable();
}
