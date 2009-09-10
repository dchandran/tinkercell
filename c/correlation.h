/****************************************************************************
 **
 ** Copyright (C) 2008 Deepak Chandran
 ** Contact double* Deepak Chandran (dchandran1@gmail.com)
 **
 ****************************************************************************/

#ifndef DEEPAK_CORRELATION_AND_STUFF
#define DEEPAK_CORRELATION_AND_STUFF

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef getValue
/*! \brief
* get the i,j th value from a 2D array stored as a single 1D array with N columns
*/
#define getValue(array, N, i, j) ( array[ (i)*(N) + (j) ] )
#endif

/* calculates correlation between two vectors
 * \param double* first vector of doubles
 * \param double* second vector of doubles
 * \param double* size of both vectors
 * \return double correlation
*/
double correlation(double *, double *, int sz);
/* calculates maximum correlation between two vectors by adjusting the starting points
 * \param double* first vector of doubles
 * \param double* second vector of doubles
 * \param double* size of both vectors
 * \param double* minumim overlap
 * \return double covariance
*/
double maxCorrelation(double *, double *, int sz, int minSz);
/* calculates correlation between two columns of two (or the same) matrix
 * \param double* first matrix (single array)
 * \param double* seconddouble* matrix (since array)
 * \param double* column of first matrix
 * \param double* column of second matrix
 * \param double* number of columns in first matrix
 * \param double* number of columns in second matrix
 * \param double* number of rows in both matrices
 * \return double covariance
*/
double colCorrelation(double *, double *, int, int, int, int, int);

#endif
