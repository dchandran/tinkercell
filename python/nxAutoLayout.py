def doLayout(type): #type = (spring, circular, random, spectral, shell, pydot, graphviz) 

	from tinkercell import *
	import networkx as nx
	from tc2py import *
   
	nodes = tc_itemsOfFamily("node");
	connections = tc_itemsOfFamily("connection");

	#connections are from 1...numConnections, nodes are the rest

	numNodes = nodes.length;
	numConnections = connections.length;

	M = [];

	for  i in range(0,numConnections):
		connected_nodes = tc_getConnectedNodes( tc_getItem(connections,i) );
		for j in range(0,connected_nodes.length):
			n = 0;
			for k in range(0,numNodes):
				if tc_getItem(nodes,k) == tc_getItem(connected_nodes,j):
					n = k;
					break;
			
			n += numConnections;
			M.append(  (i,n) );   #connection i and node k are connected

	G = nx.Graph();
	G.add_nodes_from( range( 0, numConnections + numNodes ) );
	G.add_edges_from(M);

	Pos = [];
	if type == 'spring':
		Pos = nx.spring_layout(G);
	elif type == 'circular':
		Pos = nx.circular_layout(G);
	elif type == 'pydot':
		Pos = nx.pydot_layout(G);
	elif type == 'random':
		Pos = nx.random_layout(G);
	elif type == 'spectral':
		Pos = nx.spectral_layout(G);
	elif type == 'shell':
		Pos = nx.shell_layout(G);
	elif type == 'graphviz':
		Pos = nx.graphviz_layout(G);
	else:
		Pos = nx.spring_layout(G);
	minx = 0;
	maxx = 0;
	miny = 0;
	maxy = 0;
	for i in range(0,len(Pos)):
		if minx == 0 or minx > Pos[i][0]:
			minx = Pos[i][0];
		if miny == 0 or miny > Pos[i][1]:
		  miny = Pos[i][1];
		if maxx == 0 or maxx < Pos[i][0]:
			maxx = Pos[i][0];
		if maxy == 0 or maxy < Pos[i][1]:
			maxy = Pos[i][0];

	width = (maxx - minx);
	height = (maxy - miny)

	for i in range(0,len(Pos)):
	   Pos[i][0] *= 500.0/width;
	   Pos[i][0] -= (500.0*(minx) - 100.0);
	   Pos[i][1] *= 500.0/height;
	   Pos[i][1] -= (500.0*(miny) - 100.0);

	Array = [];
	PosX = [];
	PosY = [];

	for i in range(0,numConnections):
		Array.append(connections[i]);
		PosX.append(Pos[i][0]);
		PosY.append(Pos[i][1]);

	for i in range(numConnections,numConnections+numNodes):
		Array.append(tc_getItem(nodes,i-numConnections));
		PosX.append(Pos[i][0]);
		PosY.append(Pos[i][1]);

	tc_setAllStraight(1);
	
	Array2 = tc_createItemsArray( len(Array) );
	for i in range(0,len(Array)):
		tc_setItem(Array2,i,Array[i]);
	
	Pos = tc_createMatrix( len(PosX), 2 );
	for i in range(0,len(PosX)):
		tc_setMatrixValue(Pos, i, 0, PosX[i]);
		tc_setMatrixValue(Pos, i, 1, PosY[i]);

	tc_setPosMulti(Array2,Pos);

	print "layout finished";

