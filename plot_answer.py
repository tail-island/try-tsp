import matplotlib.pyplot as plot
import networkx as nx
import numpy as np
import sys

from funcy import concat, pairwise, take


def plot_answer(cities, route, path):
    n = len(cities)
    cities_dict = dict(enumerate(cities))

    graph = nx.Graph()
    graph.add_nodes_from(cities_dict)

    for (i, j) in take(n, pairwise(concat(route, route))):
        graph.add_edge(i, j)

    plot.figure(figsize=(10, 10))
    nx.draw_networkx(graph, cities_dict)

    plot.savefig(path)


if __name__ == '__main__':
    with open(sys.argv[1]) as f:
        cities = np.array([[float(string) for string in line.strip().split()] for line in f.readlines()])

    with open(sys.argv[2]) as f:
        route = np.array([int(line.strip()) for line in f.readlines()])

    plot_answer(cities, route, sys.argv[3])
