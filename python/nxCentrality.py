"""
category: Network structure
name: Centrality measures
description: uses NetworkX to make centrality measurements
icon: nodedges.png
menu: yes
"""

from tinkercell import *
import networkx as nx

def getIndex(item,array):
	n = array.length
	for i in range(0,n):
			if tc_getItem(array,i) == item:
				return i;
	return null;

tc_print("generating adjacency matrix...");

nodes = tc_itemsOfFamily("node");
nodeNames = tc_getUniqueNames(nodes);
connections = tc_itemsOfFamily("connection");

numNodes = nodes.length;
numConnections = connections.length;

M = [];

#make the adjacency matrix
for  i in range(0,numConnections):
	connected_nodes = tc_getConnectedNodes( tc_getItem(connections,i) );
	n = connected_nodes.length;
	for j1 in range(0, n-1):
		k1 = getIndex( tc_getItem(connected_nodes,j1) , nodes);
		for j2 in range(j1, n):
			k2 = getIndex( tc_getItem(connected_nodes,j2) , nodes);
			M.append(  (k1,k2) );   #nodes k1 and k2 are connected via connection i

#make the graph
G = nx.Graph();
G.add_nodes_from( range( 0, numNodes ) );
G.add_edges_from(M);

#user interface
strList = toTC( ("degree centrality", "betweenness centrality", "load centrality", "closeness centrality") );
option = tc_getStringFromList("Select type of centrality:", strList, "degree centrality");
tc_deleteStringsArray(strList);

N = [];

if option == 0:                #compute the centrality values
	N = nx.degree_centrality(G);
elif option == 1:
	N = nx.betweenness_centrality(G);
elif option == 2:
	N = nx.load_centrality(G);
elif option == 3:
	N = nx.closeness_centrality(G);

if len(N) == numNodes:  #print and display the values
	s = "name\tcentrality\n";
	best = 0;
	worst = 0;
	minV = -1;
	maxV = -1;
	for i in range(0,numNodes):
		tc_displayNumber( tc_getItem(nodes,i), round(N[i],3));
		if minV < 0 or minV > N[i]:
			worst = i;
			minV = N[i];
		elif maxV < 0 or maxV < N[i]:
			best = i;
			maxV = N[i];
		s += nodeNames[i] + "\t" + str(N[i]) + "\n";
	tc_print(s);
	if worst > -1 and best > -1:
		tc_highlight( tc_getItem(nodes,best) , "#FF0000" );
		tc_highlight( tc_getItem(nodes,worst) , "#00FF00");

