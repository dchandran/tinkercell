#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "f2c.h"
#include "blaswrap.h"
#include "clapack.h"

/* finds the eigenvalues of a matrix using CLAPACK
 * \param: square matrix
 * \param: matrix dimension
 * \param: (output) vector of real values
 * \param: (output) vector of imaginary values
 * \return: 0 = failure 1 = success
*/
int eigenvalues(double * A, int n, double ** reals, double ** im);


/* calculates correlation between two vectors
 * \param: first vector of doubles
 * \param: second vector of doubles
 * \param: size of both vectors
 * \return: covariance
*/
double correlation(double *, double *, int sz);

/* calculates correlation between two columns of two (or the same) matrix
 * \param: first matrix (single array)
 * \param: second matrix (since array)
 * \param: column of first matrix
 * \param: column of second matrix
 * \param: number of columns in first matrix
 * \param: number of columns in second matrix
 * \param: number of rows in both matrices
 * \return: covariance
*/
double colCorrelation(double *, double *, int, int, int, int, int);

#define getValue(array, N, i, j) ( array[ i*(N) + j ] )
