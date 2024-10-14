#include "node.h"
#include <limits>
#include <string>

Node::Node(const Vector2 &position, int x, int y) : position(position), x(x), y(y), neighbourCount(0), G(0), H(0), F(0)
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
    // DrawPolyLinesEx(position, 6, outerRadius, 90.0f, 2.0f, {255, 255, 255,
    // 70});
    DrawText(std::to_string(x).c_str(), (int)position.x - 15, (int)position.y - 8, 20, WHITE);
    DrawText(std::to_string(y).c_str(), (int)position.x + 5, (int)position.y - 8, 20, WHITE);

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
