#ifndef TINKERCELL_CSTRUCTS_H
#define TINKERCELL_CSTRUCTS_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef void * OBJ;

/*!
\brief An array of strings with length information
	   Use getString(M,i,j) to get the i,j-th value in Matrix M.
*/
struct ArrayOfStrings
{
	int length;
	char ** strings;
};


/*!
\brief An array of void * objects with length information
	   Use getString(M,i,j) to get the i,j-th value in Matrix M.
*/
struct ArrayOfItems
{
	int length;
	void ** items;
};


/*!
\brief A 2D table of doubles with row and column names 
	   Use valueAt(M,i,j) to get the i,j-th value in Matrix M.
*/
struct Matrix
{
	int rows, cols;
	double * values;
	struct ArrayOfStrings rownames;
	struct ArrayOfStrings colnames;
};

/*!
\brief A 2D table of strings with row and column names 
	   Use stringAt(M,i,j) to get the i,j-th value in Matrix M.
*/
struct TableOfStrings
{
	int rows, cols;
	char ** strings;
	struct ArrayOfStrings rownames;
	struct ArrayOfStrings colnames;
};

struct Matrix newMatrix(int rows, int cols);
double getValue(struct Matrix M, int i, int j);
void setValue(struct Matrix M, int i, int j, double d);
const char * getRowName(struct Matrix M, int i);
void setRowName(struct Matrix M, int i, const char * s);
const char * getColumnName(struct Matrix M, int j);
void setColumnName(struct Matrix M, int j, const char * s);
const char* getString(struct TableOfStrings S, int i, int j);
void setString(struct TableOfStrings S, int i, int j, const char * s);
const char* ithString(struct ArrayOfStrings S, int i);
void * ithItem(struct ArrayOfItems A, int i);

#endif

