#include "BioCoder.h"
#include "Graph.h"
#include <stdlib.h>
#include <stdio.h>

// the next unique ID that should be assigned to a node
int MAX_NODE_ID = 0;

// list of all nodes
Nodelist all_nodes = {0};

// create a new node, with the given name and type, and assign it a unique id
Node* create_node_with_type(char* name, fluid_type type) {
	Node* node = (Node*)calloc(1, sizeof(Node));
	add_to_list(node, &all_nodes);
	node->in = (Nodelist*)calloc(1, sizeof(Nodelist));
	node->out = (Nodelist*)calloc(1, sizeof(Nodelist));
	node->name = name;
	node->id = MAX_NODE_ID++;
	node->type = type;
	node->time = 0;
	node->pcr_or_centrifuge = 0;
	return node;
}

// create a new node, with the given name, and assign it a unique id.
// by default this will be of type fluid sample.
Node* create_node(char* name) {
	return create_node_with_type(name, CONTAINER);
}

// adds an edge from node1 to node2
void create_edge(Node* node1, Node* node2) {
	add_to_list(node2, node1->out);
	add_to_list(node1, node2->in);
}

// adds an edge from x1 to x2, where x1 and x2 are Fluids, Samples, or Operations
// if nodes do not yet exist for these quantities, then create them
void create_edge_from_fluids(void* x1, void* x2) {
	Fluid* f1 = (Fluid*)x1;
	Fluid* f2 = (Fluid*)x2;
	// create nodes if needed
	if (f1->node == NULL) {
		f1->node = create_node_with_type(f1->original_name, f1->type);
	}
	if (f2->node == NULL) {
		f2->node = create_node_with_type(f2->original_name, f2->type);
	}
	// connect the nodes
	create_edge(f1->node, f2->node);
}

void create_edge_from_fluid_to_container(void* x1, void* x2) {
	Fluid* f1 = (Fluid*)x1;
	Container* f2 = (Container*)x2;
	// create nodes if needed
	if (f1->node == NULL) {
		f1->node = create_node_with_type(f1->original_name, f1->type);
	}
	if (f2->node == NULL) {
		f2->node = create_node_with_type(f2->name, f2->type);
	}
	// connect the nodes
	create_edge(f1->node, f2->node);
}

void create_edge_from_containers(void* x1, void* x2) {
	Container* f1 = (Container*)x1;
	Container* f2 = (Container*)x2;
	// create nodes if needed
	if (f1->node == NULL) {
		f1->node = create_node_with_type(f1->name, f1->type);
	}
	if (f2->node == NULL) {
		f2->node = create_node_with_type(f2->name, f2->type);
	}
	// connect the nodes
	create_edge(f1->node, f2->node);
}
void create_edge_from_container_to_fluid(void* x1, void* x2) {
	Container* f1 = (Container*)x1;
	Fluid* f2 = (Fluid*)x2;
	// create nodes if needed
	if (f1->node == NULL) {
		f1->node = create_node_with_type(f1->name, f1->type);
	}
	if (f2->node == NULL) {
		f2->node = create_node_with_type(f2->original_name, f2->type);
	}
	// connect the nodes
	create_edge(f1->node, f2->node);
}
// adds the given node to the list of nodes.
void add_to_list(Node* node, Nodelist* list) {
	// if list is empty, point it to new item
	if (list->node == NULL) {
		list->node = node;
	} else {
		Nodelist* last;
		// advance to end of list
		while (list->next != NULL) {
			list = list->next;
		}
		// add node to end
		last = (Nodelist*)calloc(1, sizeof(Nodelist));
		last->node = node;
		list->next = last;
	}
}

// exports the current runtime graph to the given filename, in .dot format
void export_graph(char* filename) {
	char* color;
	float color_val, ttr;
	char* temp1 = "";
	temp1 = (char *)calloc(strlen(filename)+ 10, sizeof(char));
	temp1 = strcat(temp1, filename);
	temp1 = strcat(temp1, ".dot");
	FILE* fp = fopen(filename, "w");
	Nodelist *list = &all_nodes;
	int mul;
	// print header
	fprintf(fp, "digraph G {\n");
	fprintf(fp, "  size=\"8.5,10.25\";\n");
	// every node prints itself and its outgoing edges
	while (list->node != NULL) {
		Node *node = list->node;
		Nodelist *out = node->out;
		switch(node->type){
			case 0:fprintf(fp, "%d [label=\"%s\" color=blue];\n", node->id, node->name);break;
			case 1:fprintf(fp, "%d [label=\"%s\" color=blue];\n", node->id, node->name);break;
			case 2:fprintf(fp, "%d [label=\"%s\" shape=octagon color=red];\n", node->id, node->name);break;
			case 3: if((node->unit== "sec")||(node->unit== "secs"))
						mul = 1;
					else if((node->unit== "min")||(node->unit== "mins"))
						mul = 60;
					else
						mul = 3600;
					ttr = get_ttr();
					color_val = (node->time*mul)/ttr*100;
					if (color_val <= 10)
						color = ".59166 .61 .7";
					else if ((color_val >10)&&(color_val <= 20))
						color = ".55833 .44 .81";
					else if ((color_val >20)&&(color_val <= 30))
						color = ".53888 .26 .91";
					else if ((color_val >30)&&(color_val <= 40))
						color = ".53333 .09 .97";
					else if ((color_val >40)&&(color_val <= 50))
						color = ".51694 .24 1.0";
					else if ((color_val >50)&&(color_val <= 60))
						color = ".11666 .43 .99";
					else if ((color_val >60)&&(color_val <= 70))
						color = ".07777 0.61 .99";
					else if ((color_val >70)&&(color_val <= 80))
						color = ".03888 .72 .95";
					else
						color = ".00555 .81 .84";

					if (node->pcr_or_centrifuge == 1)
						{
						if(node->time == 0)
							fprintf(fp, "%d [shape = box, style = filled, color = \"%s\",label = \"%s\"];\n", node->id, color, node->name);
						else
							//if (node->unit == "mins"||node->unit == "min") mul = 0.01; 
							//else if (node->unit == "hrs"||node->unit == "hr") mul = 0.1; 
							fprintf(fp, "%d [shape = box, style = filled, color = \"%s\", label = \"%s (Duration: %g %s)\"];\n", node->id, color, node->name, node->time, node->unit); 
						}
					else{
						if(node->time == 0)
							fprintf(fp, "%d [label = \"%s\", shape = box];\n", node->id, node->name);	
						else
							{
							//if (node->unit == "mins"||node->unit == "min") mul = 0.01; 
							//else if (node->unit == "hrs"||node->unit == "hr") mul = 0.1; 
							fprintf(fp, "%d [shape = box, style = filled, color = \"%s\", label = \"%s (Duration: %g %s)\"];\n", node->id, color, node->name, node->time, node->unit); 
							}
						}
					break;
			default:break;
			}
		while (out->node != NULL) {
			fprintf(fp, "%d -> %d;\n", node->id, out->node->id);
			if (out->next == NULL) break;
			out = out->next;
			}
		if (list->next == NULL) break;
		list = list->next;
	}
	fprintf(fp, "}\n");
	fclose(fp);
}