#ifndef __GRAPH__
#define __GRAPH__
#define NULL 0
#include "BioCoder.h"

typedef struct _Node {
	int id;
	float time;
	char* unit;
	int type;
	char* name;
	int pcr_or_centrifuge;
	struct _Nodelist* in;
	struct _Nodelist* out;
} Node;

typedef struct _Nodelist {
	struct _Node* node;
	struct _Nodelist* next;
} Nodelist;

// create a new node, with the given name, and assign it a unique id
Node* create_node(char* name);

// adds an edge from node1 to node2
void create_edge(Node* node1, Node* node2);

// adds an edge from x1 to x2, where x1 and x2 are Fluids, Samples, or Operations
// if nodes do not yet exist for these quantities, then create them
void create_edge_from_fluids(void* x1, void* x2);

void create_edge_from_containers(void* x1, void* x2);

void create_edge_from_fluid_to_container(void* x1, void* x2);

void create_edge_from_container_to_fluid(void* x1, void* x2);

// adds the given node to the list of nodes.
void add_to_list(Node* node, Nodelist* list);

// exports the current runtime graph to the given filename, in .dot format
void export_graph(char* filename);

#endif