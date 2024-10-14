#include "node.h"
#include <string>

typedef Node *NodeP;

Node::Node(const Vector2 &position, int x, int y, int length)
    : position(position), x(x), y(y), neighbourCount(0), G(0), H(0), F(0)
{
    connectionsBackward = new NodeP[length];
}

Node::~Node()
{
    delete[] connectionsBackward;
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

void Node::addNeighbour(Node *node)
{
    neighbours[neighbourCount] = node;
    neighbourCount++;
}
