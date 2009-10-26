/*!
  \file    cells_ssa.h
  \author: Deepak Chandran (dchandran1@gmail.com)
  \brief   Ssimulator that performs a multi-cell stochastic simulation using 
			a variant of the Gillespie algorithm. 
	
	The algorithm has two levels of events (or reactions):
	
	Level 1: Cell birth and death
	Level 2: reactions inside each cell
	
	An overview of the algorithm:
	
		current time: t0
		Calculate next time for cell birth or cell death: t1
	    Calculate probability of cell death and event
		
		select an event: cell birth or cell death
		
		update cell count
		
		from t0 until t1:
		    for each cell:
			    initialize concentrations for that cell
				do Gillespie simulation
				store final concentrations for that cell
				
	
	Output: all values are aligned to a grid. 
 
 ****************************************************************************/

#ifndef DEEPAK_MULTICELL_SIMULATION
#define DEEPAK_MULTICELL_SIMULATION

#include <math.h>
#include <stdlib.h>
#include "mtrand.h"

#ifndef _PropensityFunction
#define _PropensityFunction
typedef void (*PropensityFunction)(double time,double* y,double* rates,void* params);
#endif

#ifndef getValue
/*! \brief
* get the i,j th value from a 2D array stored as a single 1D array with N columns
*/
#define getValue(array, N, i, j) ( array[ (i)*(N) + (j) ] )
#endif

/*! \brief Stochastic simulation with cell growth (logistic model)
* \param int number of species (rows of stoichiometry matrix)
* \param int number of reactions (columns of stoichiometry matrix)
* \param double* stoichiometry matrix as one array, arranged as { row1, row2, row3... }, where each row is for one species
* \param (*f)(time, y-values, rates) pointer to propensity function that assigns values to the rates array -- f(time, y-values, rates)
* \param double* initial values for species
* \param double end time 
* \param int grid size (number of data points)
* \param void* any external data
* \param int number of cells
* \param double replication rate for cells
* \param double death rate for cells
* \param double percent advantage for mutants 
* \return double** two linearized 2D dimentional arrays: [0] cell growth data [1] concentrations data
* \ingroup gillespie
*/
double ** cells_ssa(int, int, double *, PropensityFunction, double*, double, int, void*, int, double, double, double);

#endif
