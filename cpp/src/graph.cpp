#include "graph.h"
#include <queue>
#include <vector>
#include <climits>

Graph::Graph(int vertices) : vertices(vertices) {
    adjList.resize(vertices);
}

void Graph::addEdge(int u, int v, int weight) {
    adjList[u].push_back({v, weight});
    adjList[v].push_back({u, weight}); // Undirected graph
}

std::vector<std::pair<int, int>> Graph::primMST() {
    std::vector<int> key(vertices, INT_MAX);
    std::vector<int> parent(vertices, -1);
    std::vector<bool> inMST(vertices, false);
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;

    key[0] = 0;
    pq.push({0, 0});

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();
        inMST[u] = true;

        for (auto& edge : adjList[u]) {
            int v = edge.first;
            int weight = edge.second;
            if (!inMST[v] && weight < key[v]) {
                parent[v] = u;
                key[v] = weight;
                pq.push({key[v], v});
            }
        }
    }

    std::vector<std::pair<int, int>> mst;
    for (int i = 1; i < vertices; i++) {
        mst.push_back({parent[i], i});
    }
    return mst;
}
