import sys
import json
import IPython
import matplotlib.pyplot as plt
import networkx as nx
from networkx.algorithms.bipartite.basic import color
from networkx.drawing.nx_agraph import graphviz_layout


def get_edge_key(v_from, v_to):
    return v_from << 32 | v_to

def get_vertex_keys(edge):
    v_from = edge >> 32
    v_to = edge & ((1 << 32) - 1)
    return (v_from, v_to)

def read_network(path):
    with open(path, 'r') as file:
        network = json.load(file)
        network['capacities'] = dict(network['capacities'])
        return network

def read_solution(path):
    with open(path, 'r') as file:
        solution = json.load(file)
        solution['flow']['values'] = dict(solution['flow']['values'])
        return solution

def node_label(network, node):
    if node == network['source']:
        return 'S'
    elif node == network['sink']:
        return 'T'
    else:
        return str(node)

def node_color(network, node):
    if node == network['source']:
        return (0.4, 0.9, 0.4)
    elif node == network['sink']:
        return (0.9, 0.4, 0.4)
    else:
        return (0.6, 0.6, 0.9)

def edge_color(network, solution, n1, n2):
    edge_key = get_edge_key(n1, n2)
    capacity = network['capacities'][edge_key]
    flow = solution['flow']['values'].get(edge_key, 0)
    if flow == 0:
        return (0, 0, 0)
    else:
        return (0.4, 0.9, 0.4)

def edge_width(network, solution, n1, n2):
    edge_key = get_edge_key(n1, n2)
    capacity = network['capacities'][edge_key]
    flow = solution['flow']['values'].get(edge_key, 0)
    if flow == 0:
        return 1
    else:
        return 2

def edge_label(network, solution, n1, n2):
    edge_key = get_edge_key(n1, n2)
    capacity = network['capacities'][edge_key]
    flow = solution['flow']['values'].get(edge_key, 0)
    return f'{flow}/{capacity}'

def build_graph(network, solution):
    G = nx.DiGraph()
    for edge, capacity in network['capacities'].items():
        (v_from, v_to) = get_vertex_keys(edge)
        G.add_edge(v_from, v_to)
    return G

def draw_graph(network, solution, G):
    node_labels = {node: node_label(network, node) for node in G.nodes()}
    node_colors = [node_color(network, node) for node in G.nodes()]
    edge_labels = {(n1, n2): edge_label(network, solution, n1, n2) for n1, n2 in G.edges()}
    edge_colors = [edge_color(network, solution, n1, n2) for n1, n2 in G.edges()]
    edge_widths = [edge_width(network, solution, n1, n2) for n1, n2 in G.edges()]
    pos = graphviz_layout(G, prog='dot')
    plt.figure(figsize=(19, 10), dpi=80)
    nx.draw_networkx(G, pos, labels=node_labels, node_color=node_colors, edge_color=edge_colors, width=edge_widths)
    nx.draw_networkx_edge_labels(G, pos, edge_labels=edge_labels, font_size=8)
    plt.show()

network_path = sys.argv[1]
solution_path = sys.argv[2]
network = read_network(network_path)
solution = read_solution(solution_path)
G = build_graph(network, solution)
draw_graph(network, solution, G)

