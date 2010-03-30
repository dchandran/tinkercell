/********************************************************************************************************

Copyright (C) 2008 Deepak Chandran
contact: dchandran1@gmail.com

	This file contains an algorithm for finding loops in an adjacency matrix. 
	The edges can be negative or positive, such as in a Jacobian matrix. 
	
	The final output will list negative and positive loops. A negative loop is where the total
	product of all the edges in the loop is negative. Positive loop is a loop that is not negative.
	
	Originally, this code was used to find loops in the Jacobian matrix of an ODE. Loops
	in the Jacobian can indicate locations of positive feedback or negative feedback. However, 
	the code is genetic for any square matrix. 
	
	
*********************************************************************************************************/
#ifndef FIND_LOOPS_IN_SQUARE_ADJ_MATRIX
#define FIND_LOOPS_IN_SQUARE_ADJ_MATRIX

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/*!
* \brief this struct is used to return the results from the loop search algorithm.
* \ingroup loops
*/
typedef struct
{
	int numLoops;   //number of loops
	int numNodes;	//number of nodes in total
	int * loopLengths; //number of nodes in each loop
	int * loopTypes;	//type of each loop. -1 = negative, +1 = positive
	int * loopHomogeneous;	//type of each loop. 0 = mix of + and -, +1 = all same (eithe - or +)
	int ** nodes;   //array of nodes (index) in each loop. use numLoops and loopsLengths[i] to iterate
} 
LoopsInformation;

/*!
* \brief free all the array in the loop return struct
* \ingroup loops
*/
void freeLoopsInfo(LoopsInformation info);

/*! \brief
Finds the negative and positive loops in a matrix of interactions 
* \param a square matrix that shows how one node influences the other (e.g. Jacobian)
* \return the number of loops, the type of loops, the nodes in each loop
* \ingroup loops
*/
LoopsInformation getLoops(double * values, int n);

#endif
