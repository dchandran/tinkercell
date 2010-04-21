/****************************************************************************
 **
 ** Copyright (C) 2008 Deepak Chandran
 ** Contact: Deepak Chandran (dchandran1@gmail.com)
 **
 ****************************************************************************/

#ifndef DEEPAK_GILLESPIE_IMPLEMENTATION
#define DEEPAK_GILLESPIE_IMPLEMENTATION

#include <math.h>
#include <stdlib.h>
#include "mtrand.h"

#ifndef _PropensityFunction
#define _PropensityFunction
typedef void (*PropensityFunction)(double time,double* y,double* rates,void* params);
#endif

#ifndef _EventFunction
#define _EventFunction
typedef int (*EventFunction)(int i, double time, double* y, void* params);
#endif

#ifndef _ResponseFunction
#define _ResponseFunction
typedef void (*ResponseFunction)(int i, double* y, void* params);
#endif

/*! \brief Stochastic simulation using Gillespie algorithm
* \param int number of species (rows of stoichiometry matrix)
* \param int number of reactions (columns of stoichiometry matrix)
* \param double* stoichiometry matrix as one array, arranged as { row1, row2, row3... }, where each row is for one species
* \param (*f)(time, y-values, rates) pointer to propensity function -- f(time, y-values, rates) --- assign values to the rates array
* \param double* initial values for species
* \param double start time
* \param double end time 
* \param int max size of array to allocate (will stop even if end time is not reached)
* \param int returns the size of the final array here
* \param void* any external data
* \param int number of events (use 0 for none)
* \param EventFunction event function (use 0 for none)
* \param ResponseFunction response function (use 0 for none)
* \return double* one dimentional array -- { row1, row2...}, where each row contains {time1,x1,x2,....}
* \ingroup simulation
*/
double * SSA(int, int, double *, PropensityFunction, double*, double, double,int, int*,void*,int numEvents, EventFunction eventFunction, ResponseFunction responseFunction);

/*! \brief Get rates from the simulated data
* \param double* simulated data
* \param int number of rows in the simulated data
* \param int number of species
* \param int number of reactions
* \param (*f)(time, y-values, rates) pointer to propensity function -- f(time, y-values, rates) --- assign values to the rates array
* \param void* any external data
* \return double* one dimentional array -- { row1, row2...}, where each row contains {time1,x1,x2,....}
* \ingroup simulation
*/
double * getRatesFromSimulatedData(double* data, int rows, int , int , PropensityFunction, void* param);

/*! \brief Continuous stochastic simulation using Langevin algorithm (stochastic diff. eqs)
* \param int number of species (rows of stoichiometry matrix)
* \param int number of reactions (columns of stoichiometry matrix)
* \param double* stoichiometry matrix as one array, arranged as { row1, row2, row3... }, where each row is for one species
* \param (*f)(time, y-values, rates) pointer to propensity function -- f(time, y-values, rates) --- assign values to the rates array
* \param double* initial values for species
* \param double start time
* \param double end time 
* \param int max size of array to allocate (will stop even if end time is not reached)
* \param int returns the size of the final array here
* \param void* any external data
* \return double* one dimentional array -- { row1, row2...}, where each row contains {time1,x1,x2,....}
* \ingroup simulation
*/
double * Langevin(int n, int m, double * N, PropensityFunction propensity, double * inits, double endTime, double dt, void * params);

/* \brief normal random number with mu=0 , var=1
* \return double
* \ingroup simulation
*/
double rnorm();

#endif
