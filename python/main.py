import graph_module

# Create a graph
g = graph_module.Graph(5)

# Add edges
g.addEdge(0, 1, 2)
g.addEdge(0, 3, 6)
g.addEdge(1, 2, 3)
g.addEdge(1, 3, 8)
g.addEdge(1, 4, 5)
g.addEdge(2, 4, 7)
g.addEdge(3, 4, 9)

# Run Prim's algorithm
mst = g.primMST()
print("Minimum Spanning Tree:", mst)
