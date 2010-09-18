all: BioCoder.obj Graph.obj

BioCoder.obj: BioCoder.cpp
	cl -c -IBioCoder BioCoder.cpp /FoBioCoder.obj

Graph.obj: Graph.cpp
	cl -c -IBioCoder Graph.cpp /FoGraph.obj

clean:
	del BioCoder.obj Graph.obj
