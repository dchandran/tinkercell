def doLayout(type): #type = (spring, circular, random, spectral, shell, pydot, graphviz) 

	import pytc
	import networkx as nx

	nodes = pytc.itemsOfFamily("node");
	connections = pytc.itemsOfFamily("connection");

	#connections are from 1...numConnections, nodes are the rest

	numNodes = len(nodes);
	numConnections = len(connections);

	M = [];

	for  i in range(0,numConnections):
		connected_nodes = pytc.getConnectedParts( connections[i] );
		for j in connected_nodes:
			n = 0;
			for k in range(0,numNodes):
				if nodes[k] == j:
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
	#   pytc.setCenterPoint(connections[i],Pos[i][0],Pos[i][1]);

	for i in range(numConnections,numConnections+numNodes):
		Array.append(nodes[i-numConnections]);
		PosX.append(Pos[i][0]);
		PosY.append(Pos[i][1]);
	#   pytc.setPos(nodes[i-numConnections],Pos[i][0],Pos[i][1]);

	pytc.setAllStraight();
	pytc.setPosMulti(Array,[ PosX, PosY ]);

	print "layout finished";