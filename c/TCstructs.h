#ifndef TINKERCELL_CSTRUCTS_H
#define TINKERCELL_CSTRUCTS_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/*!\brief An array of strings with length information. Use ithString(M,i) to get the i-th string.*/
struct ArrayOfStrings
{
	int length;
	char ** strings;
};


/*!\brief An array of void * objects with length information. Use ithItem(M,i) to get the i-th item.*/
struct ArrayOfItems
{
	int length;
	void ** items;
};


/*!\brief A 2D table of doubles with row and column names. Use getValue(M,i,j) to get the i,j-th value in Matrix M.*/
struct Matrix
{
	int rows, cols;
	double * values;
	struct ArrayOfStrings rownames;
	struct ArrayOfStrings colnames;
};

/*!\brief A 2D table of strings with row and column names. Use getString(M,i,j) to get the i,j-th value in Matrix M.*/
struct TableOfStrings
{
	int rows, cols;
	char ** strings;
	struct ArrayOfStrings rownames;
	struct ArrayOfStrings colnames;
};

/*!\brief Create a matrix with the given rows and columns*/
struct Matrix newMatrix(int rows, int cols);

/*!\brief Create a strings table with the given rows and columns*/
struct TableOfStrings newTableOfStrings(int rows, int cols);

/*!\brief Create an array of strings*/
struct ArrayOfStrings newArrayOfStrings(int len);

/*!\brief Create an array of items*/
struct ArrayOfItems newArrayOfItems(int len);

/*!\brief get i,jth value from a Matrix*/
double getValue(struct Matrix M, int i, int j);

/*!\brief set i,jth value of a Matrix*/
void setValue(struct Matrix M, int i, int j, double d);

/*!\brief get ith row name from a Matrix*/
const char * getRowName(struct Matrix M, int i);

/*!\brief set ith row name for a Matrix*/
void setRowName(struct Matrix M, int i, const char * s);

/*!\brief get jth column name of a Matrix*/
const char * getColumnName(struct Matrix M, int j);

/*!\brief set jth column name of a Matrix*/
void setColumnName(struct Matrix M, int j, const char * s);

/*!\brief get i,j-th string in a table*/
const char* getString(struct TableOfStrings S, int i, int j);

/*!\brief set i,jth string in a table*/
void setString(struct TableOfStrings S, int i, int j, const char * s);

/*!\brief get ith string in array of strings*/
const char* ithString(struct ArrayOfStrings S, int i);

/*!\brief set ith void * item in array of items*/
void * ithItem(struct ArrayOfItems A, int i);

/*!\brief delete a matrix*/
void deleteMatrix(struct Matrix M);

/*!\brief delete a strings table*/
void deleteTableOfStrings(struct TableOfStrings M);

/*!\brief delete an array */
void deleteArrayOfItems(struct ArrayOfItems A);

/*!\brief delete an array */
void deleteArrayOfStrings(struct ArrayOfStrings C);

/*!\brief combine two matrices by appending their columns. row size must be equal for both matrices*/
struct Matrix cbind(struct Matrix A, struct Matrix B);

/*!\brief combine two matrices by appending their row. column sizes must be equal for both matrices*/
struct Matrix rbind(struct Matrix A, struct Matrix B);

#endif

