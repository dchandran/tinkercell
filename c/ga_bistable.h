/********************************************************************************************************

Copyright (C) 2008 Deepak Chandran
contact: dchandran1@gmail.com

	This file uses genetic algorithm optimization method to find parameters for a system of ODE such 
	that the ODE system will have two or more stable states. 
	
	The objective function tries to find a saddle or unstable point in the ODE system by doing
	reverse time or partial-reverse time simulation. Once a saddle or unstable point is found, the algorithm
	solves the IVP problem starting around the unstable or saddle point, hoping to reach two stable points. 
	
	The unstable or saddle points are found by optimizing: 
	
		p (parameters of the system) and a (vector of reals)
		
	where the ODE system is:
	
		dx/dt = a * f(x,p)
		
	where f(x,p) is the original system defined by the user.
	The alpha vector alters the stability of the critical points, allowing the algorithm to search
	for saddle points of the original system by searching for stable points of this new transformed system.
	
	
*********************************************************************************************************/

#ifndef GA_BISTABLE_H
#define GA_BISTABLE_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "cvodesim.h"
#include "mtrand.h"
#include "ga.h"

/*! \brief ODE parameters and alpha for optimization
 * \ingroup bistable
*/
typedef struct
{
   int numVars;    //number of odes
   int numParams;  //number of parameters
   double *params; //parameters of the model
   double *alphas; //coefficients for ode (part of optimization algorithm)
}
Parameters;

/*! \brief return type for the bistability finding algorithm
* \ingroup bistable
*/
typedef struct
{
   Parameters * param; //the parameters that make the system bistable
   double * unstable;  //the unstable point
   double * stable1;  //first stable point
   double * stable2;  //second stable point
} BistablePoint;

#define randnum (mtrand() * 1.0)

/*! \brief Find the parameters that forces the system to have two or more steady states
 * \param number of variables
 * \param number of parameters
 * \param initial values
 * \param max iterations of GA
 * \param initial size of random parameters
 * \param ode function pointer
 * \return parameters for the ode and the alpha values
 * \ingroup bistable
 */
BistablePoint makeBistable(int n, int p,double* iv, int maxiter, int popsz, void (*odefnc)(double,double*,double*,void*));

#endif

