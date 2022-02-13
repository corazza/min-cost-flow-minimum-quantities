import matplotlib.pyplot as plt
import networkx as nx
from networkx.algorithms.bipartite.basic import color
print("unesite ime datoteke")
ime = input()
G = nx.DiGraph()
edges = []

with open(ime) as file:
    for line in file:
        index = -1
        for num in line.strip().split(' '):
            index += 1
            if(index != 2):
                edges.append(int(num))
            else:
                edges.append(float(num))


i = 0
while i < (len(edges) - 2):
    v1 = edges[i]
    v2 = edges[i+1]
    w = edges[i+2]

    G.add_edge(v1, v2, weight=w)
    if(i == len(edges) - 3): break

    i += 3


color_map=[]
for node in G:
    if node==1:
        color_map.append('green')
    else:
        color_map.append('blue')

color_map.pop()
color_map.append('red')

pos=nx.shell_layout(G) 
labels = nx.get_edge_attributes(G,'weight')

nx.draw_networkx(G,pos,node_color=color_map)
nx.draw_networkx_edge_labels(G,pos,edge_labels=labels,font_size=8)
plt.show()