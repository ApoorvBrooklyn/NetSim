#pragma once
#include <vector>
#include <utility>

class Graph {
public:
    Graph(int vertices);
    void addEdge(int u, int v, int weight);
    std::vector<std::pair<int, int>> primMST();

private:
    int vertices;
    std::vector<std::vector<std::pair<int, int>>> adjList;
};
