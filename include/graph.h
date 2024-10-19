#pragma once

#include <raylib.h>

class Path;
class Node;

/**
 * @class Graph (singleton)
 * @brief generates and manages all the nodes to generate paths from
 *
 */
class Graph
{
  public:
    /**
     * @brief singleton function
     *
     * @return Graph* - singleton instance of graph
     */
    static Graph *getInstance();
    Graph(const Graph &g) = delete;
    Graph operator=(const Graph &g) = delete;
    /**
     * @brief regenerates node paths
     */
    static void refresh();
    /**
     * @brief destroys all contained nodes
     */
    ~Graph();
    /**
     * @brief for debugging only: draws nodes and connections
     */
    void render() const;
    /**
     * @brief safe getter
     *
     * @param x int - x index of node
     * @param y int - y index of node
     * @return const Node* - node at corresponding location (or nullptr for
     * incorrect indices)
     */
    const Node *getNode(int x, int y) const;
    /**
     * @brief unsafe getter
     *
     * @param x int - x index of node
     * @param y int - y index of node
     * @return Node* - node at corresponding location (or nullptr for incorrect
     * indices)
     */
    Node *unsafeGetNode(int x, int y);
    /**
     * @brief safe getter for nearest accessible node from a given screen
     * location
     *
     * @param P Vector2 - screen location
     * @return const Node* - nearest accessible node
     */
    const Node *getBestNode(const Vector2 &P) const;

    float outerRadius;

  private:
    Graph();
    int resolutionX, resolutionY;
    Node **nodes;
    /**
     * @brief connects neighbouring nodes to one another if a path exists to
     * connect them
     */
    void generateEdges();

    static Graph *instance;
};
