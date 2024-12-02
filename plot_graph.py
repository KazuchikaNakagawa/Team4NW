import networkx as nx
import matplotlib.pyplot as plt

# Create a directed graph
G = nx.DiGraph()

# Add edges with bandwidth as weights
edges = [
    ("node2", "node1", 49.2),
    ("node2", "node4", 1.48),
    ("node2", "node5", 9.58),
    ("node3", "node1", 9.73),
    ("node3", "node2", 49.2),
    ("node3", "node4", 49.2),
    ("node3", "node5", 1.24),
    ("node4", "node1", 10.1),
    ("node4", "node2", 1.25),
    ("node4", "node5", 9.54),
    ("node5", "node1", 47.8),
    ("node5", "node2", 9.54),
    ("node5", "node4", 47.8),
]

G.add_weighted_edges_from(edges)

# Draw the graph
pos = nx.spring_layout(G, seed=42)
plt.figure(figsize=(10, 8))
nx.draw(
    G, pos, with_labels=True, node_color="lightblue", node_size=2000, font_size=10, font_weight="bold", edge_color="gray"
)
edge_labels = {(u, v): f"{d['weight']}Mbits/sec" for u, v, d in G.edges(data=True)}
nx.draw_networkx_edge_labels(G, pos, edge_labels=edge_labels, font_size=8)

plt.title("Network Bandwidth Visualization")
plt.savefig("network_bandwidth.png")
