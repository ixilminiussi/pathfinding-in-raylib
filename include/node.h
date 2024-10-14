#pragma once

#include <raylib.h>

class Node
{
    typedef Node *NodeP;

  public:
    Node() = delete;
    Node(const Vector2 &position, int x, int y);
    ~Node() = default;

    Node(const Node &other);
    Node &operator=(const Node &other);
    bool operator==(const Node &other) const;
    bool operator!=(const Node &other) const;

    void render();
    const int getWeight(const Node *node) const;

    const Vector2 &getPosition() const;
    void addNeighbour(const Node *node);

    float G, H, F;
    Node *connectionBackward;
    Node *connectionForward;

    const Node *neighbours[6];
    int neighbourCount;

  private:
    Vector2 position;
    int x, y;
};

typedef Node *NodeP;
