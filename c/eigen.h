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
int eigenvalues(double * A, int n, double * reals, double * im);

