#include <math.h>
#include <stdlib.h>
#include "mtrand.h"

/*
* get the i,j th value from a 2D array stored as a single 1D array with N columns
*/
#ifndef getValue
#define getValue(array, N, i, j) ( array[ (i)*(N) + (j) ] )
#endif

double rnorm();

#ifndef _PropensityFunction
#define _PropensityFunction
typedef void (*PropensityFunction)(double time,double* y,double* rates,void* params);
#endif

double * Langevin(int n, int m, double * N, PropensityFunction propensity, double * inits, double endTime, double dt, void * params);
