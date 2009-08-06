#ifndef TINKERCELL_CSTRUCTS_H
#define TINKERCELL_CSTRUCTS_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef void* OBJ;
typedef OBJ* ARRAY;
typedef OBJ* Array;

/*!
\brief A struct with values, rows, columns, and names for rows and columns. Use valueAt(M,i,j) to get the i,j-th value in Matrix M.
*/
typedef struct 
{
	int rows, cols;
	double * values;
	char** rownames;
	char** colnames;
} Matrix;

typedef Matrix matrix;

#define valueAt(M, i, j) ( M.values[ (i)*(M.cols) + (j) ] )

#endif

