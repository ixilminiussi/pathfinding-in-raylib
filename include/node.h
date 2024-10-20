#pragma once

#include <raylib.h>

/**
 * @class Node
 * @brief Used within a graph to run the A* algorithm.
 *
 */
class Node
{
    typedef Node *NodeP;

  public:
    Node() = delete;
    ~Node() = default;
    /**
     * @brief Nodes chiefly have a screen and graph position,
     *
     * @param position Vector2 - screen coordinates
     * @param x int - graph x index
     * @param y int - graph y index
     */
    Node(const Vector2 &position, int x, int y);
    /**
     * @brief copies a node's data into self and sets G to infinity.
     * is key to multithreading the path finding
     *
     * @param other Node - node to be copied
     */
    Node(const Node &other);
    /**
     * @brief copies a node's data into self and sets G to infinity.
     * is key to multithreading the path finding
     *
     * @param other Node - node to be copied
     * @return self
     */
    Node &operator=(const Node &other);
    /**
     * @brief checks if two nodes have the same relevant information
     *
     * @param other Node - node to compare
     * @return bool - true if indices and neighbourCounts are same
     */
    bool operator==(const Node &other) const;
    /**
     * @brief opposite of ==
     *
     * @param other Node - node to compare
     * @return bool - true if == is false
     */
    bool operator!=(const Node &other) const;
    /**
     * @brief debug mode only - draws the node on screen
     */
    void render();
    /**
     * @brief simple position getter
     *
     * @return Vector2 - screen position
     */
    const Vector2 &getPosition() const;
    /**
     * @brief adds neighbour to array/vector depending on whether array is full.
     *
     * @param node Node - node to be added
     */
    void addNeighbour(const Node *node);

    void setG(float newG); // @brief updates F accordingly
    void setH(float newH); // @brief updates F accordingly
    void setConnectionBackward(Node *node);
    void setConnectionForward(Node *node);

    float getG() const;
    float getF() const;
    float getH() const;
    const Node *getNeighbour(int index) const; // @brief gets neighbour from index
    int getNeighbourCount() const;
    Node *getConnectionBackward();
    Node *getConnectionForward();
    bool inWall() const;

  private:
    float G, H, F;
    Vector2 position;
    int x, y;

    Node *connectionBackward;
    Node *connectionForward;

    const Node *neighbours[6];
    int neighbourCount;
};

typedef Node *NodeP;
