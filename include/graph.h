#pragma once

#include <raylib.h>

class Path;
class Node;

class Graph
{
  public:
    static Graph *getInstance();
    static void refresh();

    ~Graph();

    Graph(const Graph &g) = delete;
    Graph operator=(const Graph &g) = delete;

    void render();

    Node *getNode(int x, int y) const;
    Node *getBestNode(const Vector2 &P) const;
    int getIndex(int x, int y) const;

    float outerRadius;

  private:
    Graph();
    int resolutionX, resolutionY;
    Node **nodes;
    void generateEdges();
    void generateWeights();
    Path getPath(const Vector2 &A, const Vector2 &B);

    static Graph *instance;
};
