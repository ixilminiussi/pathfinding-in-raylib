#pragma once

#include <raylib.h>

class Node
{
    typedef Node *NodeP;

  public:
    Node() = delete;
    Node(const Vector2 &position, int x, int y, int length);
    ~Node();

    void render();
    const int getWeight(const Node *node) const;

    const Vector2 &getPosition() const;
    void addNeighbour(Node *node);

    float G, H, F;
    Node *connectionBackward;
    Node *connectionForward;

    Node **connectionsBackward;
    Node *neighbours[6];
    int neighbourCount;
    int x, y;

  private:
    Vector2 position;
};

typedef Node *NodeP;
