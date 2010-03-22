/*!
  \file    cvodesim.h
  \author: Deepak Chandran (dchandran1@gmail.com)
  \brief   simulation functions that use CVODE to integrate differential equations

A wrapper for the Sundials CVODE numerical integration library
The wrapper is only for initial value problems. 
Each wrapper functions have two versions: 
	one that accepts a function with differential equation definitions
	one that accepts a stoichiometry matrix and a propensity function
The wrapper provides support for event functions and user defined structs as parameter.

****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef GA_CVODE_WRAPPER_FILE
#define GA_CVODE_WRAPPER_FILE

//#define SUNDIALS_DOUBLE_PRECISION
//#define SUNDIALS_EXTENDED_PRECISION

#include <cvode/cvode.h>             /* prototypes for CVODE fcts. and consts. */
#include <cvode/cvode_band.h>
#include <nvector/nvector_serial.h>  /* serial N_Vector types, fcts., and macros */
#include <sundials/sundials_band.h>  /* definitions of type BandMat and macros */
#include <sundials/sundials_types.h> /* definition of type realtype */
#include <sundials/sundials_math.h>  /* definition of ABS and EXP */
#include "eigen.h"   /*eigenvalue computation */

/*!
 * \brief The type for the user defined differential equation function
 */
typedef void (*ODEFunction)(double time,double* y,double* dydt,void* params);

/*!
 * \brief The type for the user defined propensity function
 */
#ifndef _PropensityFunction
#define _PropensityFunction
typedef void (*PropensityFunction)(double time,double* y,double* rates,void* params);
#endif

/*!
 * \brief The type for the user defined event function
 */
typedef double (*EventFunction)(double time,double* y,void* params);

/*!
 * \brief set events for the system
 * \param number of events
 * \param array with pointers to event functions
 * \ingroup cvodewrapper
*/
void ODEevents(int numEvents, EventFunction * eventFunctions);

/*!
 * \brief set the flags
 * \param only positive values
 * \ingroup cvodewrapper
*/
void ODEflags(int);

/*!
 * \brief specify number of variables in the ODE system
 * \param relative error allowed
 * \param absolute error allowed
 * \ingroup cvodewrapper
*/
void ODEtolerance(double,double);

/*!
 * \brief The simulate function. Uses CVODE integrator
 * \param number of variables
 * \param array of initial values
 * \param ode function pointer
 * \param start time for the simulation
 * \param ending time for the simulation
 * \param time increments for the simulation
 * \param user data type for storing other information
 * \return one dimentional array -- { row1, row2...}, where each row contains {time1,x1,x2,....}. Use getValue(y,n,i,j) if needed
 * \ingroup cvodewrapper
 */
double* ODEsim(int N, double * initValues, ODEFunction function, double startTime, double endTime, double stepSize, void * params);


/*!
* \brief The simulate function that accepts a stoichiometry matrix and propensity function. Uses CVODE integrator
* \param  number of species (rows of stoichiometry matrix)
* \param  number of reactions (columns of stoichiometry matrix)
* \param  stoichiometry matrix as one array, arranged as { row1, row2, row3... }, where each row is for one species
* \param  pointer to propensity function -- f(time, y-values, rates) --- assign values to the rates array
* \param  intial values for species
* \param  start time
* \param  end time 
* \param  time increments for the simulation
* \param  user data type for storing other information
* \return  one dimentional array -- { row1, row2...}, where each row contains {time1,x1,x2,....}. Use getValue(y,n,i,j) if needed
* \ingroup cvodewrapper
*/
double * ODEsim2(int, int, double *, PropensityFunction f, double*, double, double, double,void*);


/*!
 * \brief Gets jacobian matrix of the system at the given point
 * \param number of variables
 * \param array of values (point where Jacobian will be calculated)
 * \param ode function pointer
 * \param additional parameters needed for ode function
 * \param double* (output) if non-zero, the real part of the eigenvalues will be returned here
 * \param double* (output) if non-zero, the imaginary part of the eigenvalues will be returned here
 * \return 2D array made into linear array -- use getValue(array,N,i,j)
 * \ingroup cvodewrapper
 */
double* jacobian(int N, double * point,  ODEFunction function, void * params, double * eigenreal, double * eigenim);

/*!
 * \brief Gets jacobian matrix of the system at the given point
 * \param  number of species (rows of stoichiometry matrix)
 * \param  number of reactions (columns of stoichiometry matrix)
 * \param  stoichiometry matrix as one array, arranged as { row1, row2, row3... }, where each row is for one species
 * \param  pointer to propensity function -- f(time, y-values, rates) --- assign values to the rates array
 * \param array of values (point where Jacobian will be calculated)
 * \param additional parameters needed for ode function
 * \param double* (output) if non-zero, the real part of the eigenvalues will be returned here
 * \param double* (output) if non-zero, the imaginary part of the eigenvalues will be returned here
 * \return 2D array made into linear array -- use getValue(array,N,i,j)
 * \ingroup cvodewrapper
 */
double* jacobian2(int m, int n, double * N, PropensityFunction f, double * point, void * params, double * eigenreal, double * eigenim);

/*!
 * \brief Bring a system to steady state
 * \param number of variables
 * \param array of initial values
 * \param ode function pointer
 * \param minimum allowed value
 * \param maximum time for simulation
 * \param the difference in time to use for estimating steady state
 * \return array of values
 * \ingroup cvodewrapper
 */
double* steadyState(int N, double * initialValues, ODEFunction function, void * params, double minerr, double maxtime, double delta);

/*!
 * \brief Bring a system to steady state
 * \param  number of species (rows of stoichiometry matrix)
 * \param  number of reactions (columns of stoichiometry matrix)
 * \param  stoichiometry matrix as one array, arranged as { row1, row2, row3... }, where each row is for one species
 * \param  pointer to propensity function -- f(time, y-values, rates) --- assign values to the rates array
 * \param array of initial values
 * \param minimum allowed value
 * \param maximum time for simulation
 * \param the difference in time to use for estimating steady state
 * \return array of values
 * \ingroup cvodewrapper
 */
double* steadyState2(int m, int n, double * N, PropensityFunction f, double * initialValues, void * params, double minerr, double maxtime, double delta);

/*!
 * \brief Find the rates of change after simulating for the given amount of time
 * \param number of variables
 * \param array of initial values
 * \param ode function pointer
 * \param time for simulation
 * \return array of values
 * \ingroup cvodewrapper
 */
double* getDerivatives(int N, double * initValues, ODEFunction function, double startTime, double endTime, double stepSize, void * params);

/*!
 * \brief Find the rates of change after simulating for the given amount of time
 * \param  number of species (rows of stoichiometry matrix)
 * \param  number of reactions (columns of stoichiometry matrix)
 * \param  stoichiometry matrix as one array, arranged as { row1, row2, row3... }, where each row is for one species
 * \param  pointer to propensity function -- f(time, y-values, rates) --- assign values to the rates array
 * \param array of initial values
 * \param time for simulation
 * \return array of values
 * \ingroup cvodewrapper
 */
double* getDerivatives2(int m, int n, double * N, PropensityFunction f, double * initValues, double startTime, double endTime, double stepSize, void * params);

#ifndef getValue
#define getValue(array, N, i, j) ( array[ (((i)*(N)) + (j)) ] )
#endif

/*!
* \brief print a linearized 2D table to a file
* \param filename to write to
* \param data to write
* \param number of rows
* \param number of columns
*/
void writeToFile(char* filename, double* data, int rows, int cols);

#endif
