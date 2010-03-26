#ifndef TINKERCELL_CSTRUCTS_H
#define TINKERCELL_CSTRUCTS_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef void * OBJ;

#ifdef SWIG
%module tc
%{
#endif

/*!
\brief An array of strings with length information
	   Use getString(M,i,j) to get the i,j-th value in Matrix M.
*/
typedef struct 
{
	int length;
	char ** strings;
} ArrayOfStrings;


/*!
\brief An array of void * objects with length information
	   Use getString(M,i,j) to get the i,j-th value in Matrix M.
*/
typedef struct 
{
	int length;
	void ** items;
} ArrayOfItems;


/*!
\brief A 2D table of doubles with row and column names 
	   Use valueAt(M,i,j) to get the i,j-th value in Matrix M.
*/
typedef struct 
{
	int rows, cols;
	double * values;
	char** rownames;
	char** colnames;
} Matrix;

/*!
\brief A 2D table of strings with row and column names 
	   Use stringAt(M,i,j) to get the i,j-th value in Matrix M.
*/
typedef struct 
{
	int rows, cols;
	char ** strings;
	ArrayOfStrings rownames;
	ArrayOfStrings colnames;
} TableOfStrings;


Matrix newMatrix(int rows, int cols);
double getValue(Matrix M, int i, int j);
void setValue(Matrix M, int i, int j, double d);
const char * getRowName(Matrix M, int i);
void setRowName(Matrix M, int i, const char * s);
const char * getColumnName(Matrix M, int j);
void setColumnName(Matrix M, int j, const char * s);
const char* getString(TableOfStrings S, int i, int j);
void setString(TableOfStrings S, int i, int j, const char * s);
const char* ithString(ArrayOfStrings S, int i);
void * ithItem(ArrayOfItems A, int i);

#ifdef SWIG
%}
#endif

#endif

