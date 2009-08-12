import pytc
import networkx as nx

def getIndex(item,array):
	n = len(array)
	for i in range(0,n):
			if array[i] == item:
				return i;
	return null;

pytc.write("generating adjacency matrix...");

nodes = pytc.itemsOfFamily("node");
nodeNames = pytc.getNames(nodes);
connections = pytc.itemsOfFamily("connection");

numNodes = len(nodes);
numConnections = len(connections);

M = [];

#make the adjacency matrix
for  i in range(0,numConnections):
	connected_nodes = pytc.getConnectedNodes( connections[i] );
	n = len(connected_nodes);
	for j1 in range(0, n-1):
		k1 = getIndex( connected_nodes[j1] , nodes);
		for j2 in range(j1, n):
			k2 = getIndex( connected_nodes[j2] , nodes);
			M.append(  (k1,k2) );   #nodes k1 and k2 are connected via connection i

#make the graph
G = nx.Graph();
G.add_nodes_from( range( 0, numNodes ) );
G.add_edges_from(M);

#user interface
option = pytc.getFromList("Select type of centrality:", ("degree centrality","betweenness centrality","load centrality","closeness centrality"));

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
		pytc.displayNumber(nodes[i], round(N[i],3));
		if minV < 0 or minV > N[i]:
			worst = i;
			minV = N[i];
		elif maxV < 0 or maxV < N[i]:
			best = i;
			maxV = N[i];
		s += nodeNames[i] + "\t" + str(N[i]) + "\n";
	pytc.write(s);
	if worst > -1 and best > -1:
		pytc.highlight(nodes[best],255,0,0);
		pytc.highlight(nodes[worst],0,0,255);