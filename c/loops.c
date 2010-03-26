#include <stdlib.h>
#include <stdio.h>
#include "loops.h"

#define valueAt(array, N, i, j) ( array[ (((i)*(N)) + (j)) ] )

void freeLoopsInfo(LoopsInformation info)
{
	if (info.loopTypes)	free(info.loopTypes);
	if (info.nodes) free(info.nodes);
	if (info.loopLengths) free(info.loopLengths);
	info.numNodes = 0;
	info.numLoops = 0;
	info.loopTypes = 0;
	info.nodes = 0;
	info.loopLengths = 0;
}

static int * Loops;
static int numLoops;
static void recursiveLoopSearch(int, int, int * , double * , int, int *, int *);

static int alreadyExistsInLoops(int * newLoop, int N)
{
	int i,j,k;
	
	for (i=0; i < numLoops; ++i)
	{
		k = 1;
		for (j=0; j < N; ++j)
		{
			if ((valueAt(Loops,N,i,j) + newLoop[j] != 0)  //one or both are non-zero
				&& (valueAt(Loops,N,i,j) * newLoop[j] <= 0)) //opposite sign or one is zero
			{
				k = 0;
				break;
			}
		}
		if (k)
		{
			//for (j=0; j < N; ++j)
				//printf("%i\t",newLoop[j]);
			//printf("\n");
			return 1;
		}
	}
	return 0;
}

/*
Finds the negative and positive loops in a matrix of interactions 
input: a square matrix that shows how one node influences the other (e.g. Jacobian)
output: the number of loops, the type of loops, the nodes in each loop
*/
LoopsInformation getLoops(double * values, int n)
{
	int i,j,k, * path, * temp, * visited, count;
	double prod;
	
	LoopsInformation info;
	info.numNodes = n;
	info.numLoops = 0;
	info.loopTypes = 0;
	info.nodes = 0;
	info.loopLengths = 0;

	if (!values || n < 2)	return info;

	//pick an arbitrary starting value
	Loops = 0;
	numLoops = 0;

	path = malloc(n * sizeof(int));
	temp = malloc(n * sizeof(int));
	visited = malloc(n * sizeof(int));
	
	for (j=0; j < n; ++j) visited[j] = 0;
	
	//get all loops
	for (i=0; i < n; ++i)
	{
		//printf("node %i start\n",i);
		for (j=0; j < n; ++j) path[j] = 0;
		if (visited[i] == 0)
		{
			recursiveLoopSearch(i,1,path,values,n,visited,temp);
		}
	}
	
	//all loops are stored in Loops and numLoops

	free(temp);
	free(visited);
	free(path);
	if (numLoops > 0)
	{
		info.numLoops = numLoops;
		info.loopTypes = malloc(numLoops * sizeof(int));
		info.loopLengths = malloc(numLoops * sizeof(int));
		info.nodes = malloc(numLoops * sizeof(int*));
	}
	
	for (i=0; i < numLoops; ++i)  //for each loop in Loops
	{
		count = 0;
		prod = 1.0;
		for (j=0; j < n; ++j) //for all the nodes...
		{
			if (valueAt(Loops,n,i,j) > 0) //... in loop i
			{
				++count;
			}
		}

		info.loopLengths[i] = count; //number of nodes in loop i
		info.nodes[i] = malloc(count * sizeof(int));

		for (j=0; j < n; ++j)
		{
			k = valueAt(Loops,n,i,j);
			if (k > 0) //... in loop i
			{
				info.nodes[i][k-1] = j;
			}
		}

		for (j=0; j < count; ++j)
		{
			if (j < (count-1))
				k = j+1;
			else
				k = 0;
			prod *= valueAt(values,n,info.nodes[i][j],info.nodes[i][k]);
		}

		if (prod > 0)
			info.loopTypes[i] = 1; //loop i is positive
		else
			info.loopTypes[i] = -1; //loop i is negative
	}

	return info;
}

/*! \brief
Recursive function that traverses all possible paths in a graph and returns
all the closed loops in that graph. Output is stored in global array Loops and numLoops
*/
void recursiveLoopSearch(int n, int index, int * path, double * values, int N, int * visited, int * temp)
{
	int i,j,k,s;
	
	if (path[n] > 0) //found a loop, i.e. current node has been visited
	{
		s = path[n];
		for (i=0; i < N; ++i)
		{
			if (path[i] >= s)
				temp[i] = path[i] - s + 1;			
			else
				temp[i] = 0;
		}
		
		if (!alreadyExistsInLoops(temp,N))
		{
			//copy old loops
			int * newLoops = malloc((1+numLoops) * N * sizeof(int));
			for (i=0; i < (numLoops * N); ++i)	
				newLoops[i] = Loops[i];
			if (Loops) free(Loops);
			Loops = newLoops;

			//append new loop
			for (i=0; i < N; ++i)
				Loops[ (numLoops * N) + i ] = temp[i];
			
			++numLoops;
		}
		
	}
	else
	{
		visited[n] = 1;
		k = path[n];
		path[n] = index; //add current node to path

		for (i=0; i < N; ++i)  //loop through all nodes...
		{
			if (valueAt(values,N,n,i) != 0) //... that interact with node-n
			{
				 //follow that path
				recursiveLoopSearch(i, index+1, path, values, N, visited, temp);
			}
		}

		path[n] = k;  //remove current node from path
	}
}

