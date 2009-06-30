#ifndef GA_BISTABLE_H
#define GA_BISTABLE_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "cvodesim.h"
#include "mtrand.h"
#include "ga.h"

/*! \brief ODE parameters and alpha for optimization*/
typedef struct
{
   int numVars;    //number of odes
   int numParams;  //number of parameters
   double *params; //parameters of the model
   double *alphas; //coefficients for ode (part of optimization algorithm)
}
Parameters;

/*! \brief return type for the bistability finding algorithm*/
typedef struct
{
   Parameters * param; //the parameters that make the system bistable
   double * unstable;  //the unstable point
   double * stable1;  //first stable point
   double * stable2;  //second stable point
} BistablePoint;

#define randnum (mtrand() * 1.0)

/*! \brief
 * Find the parameters that forces the system to have two or more steady states
 * \param: number of variables
 * \param: number of parameters
 * \param: initial values
 * \param: max iterations of GA
 * \param: initial size of random parameters
 * \param: ode function pointer
 * @ret: parameters for the ode and the alpha values
 */
BistablePoint makeBistable(int n, int p,double* iv, int maxiter, int popsz, void (*odefnc)(double,double*,double*,void*));

//double** getSteadyStates(Parameters * p, double * iv);

#endif
