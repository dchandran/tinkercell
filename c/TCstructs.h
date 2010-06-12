#ifndef TINKERCELL_CSTRUCTS_H
#define TINKERCELL_CSTRUCTS_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
typedef void* Item;
typedef const char * String;

/*!\brief An array of strings with length information. Use nthString(M,i) to get the i-th string.*/
typedef struct 
{
	int length;
	char ** strings;
} ArrayOfStrings;


/*!\brief An array of void * objects with length information. Use nthItem(M,i) to get the i-th item.*/
typedef struct 
{
	int length;
	void ** items;
} ArrayOfItems;


/*!\brief A 2D table of doubles with row and column names. Use getValue(M,i,j) to get the i,j-th value in TableOfReals M.*/
typedef struct 
{
	int rows, cols;
	double * values;
	ArrayOfStrings rownames;
	ArrayOfStrings colnames;
} TableOfReals;


/*!\brief A 2D table of strings with row and column names. Use getString(M,i,j) to get the i,j-th value in TableOfReals M.*/
typedef struct 
{
	int rows, cols;
	char ** strings;
	ArrayOfStrings rownames;
	ArrayOfStrings colnames;
} TableOfStrings;

/*!\brief Create a matrix with the given rows and columns*/
TableOfReals newMatrix(int rows, int cols);

/*!\brief Create a strings table with the given rows and columns*/
TableOfStrings newTableOfStrings(int rows, int cols);

/*!\brief Create an array of strings*/
ArrayOfStrings newArrayOfStrings(int len);

/*!\brief Create an array of items*/
ArrayOfItems newArrayOfItems(int len);

/*!\brief get i,jth value from a TableOfReals*/
double getValue(TableOfReals M, int i, int j);

/*!\brief set i,jth value of a TableOfReals*/
void setValue(TableOfReals M, int i, int j, double d);

/*!\brief get ith row name from a TableOfReals*/
const char * getRowName(TableOfReals M, int i);

/*!\brief set ith row name for a TableOfReals*/
void setRowName(TableOfReals M, int i, const char * s);

/*!\brief get jth column name of a TableOfReals*/
const char * getColumnName(TableOfReals M, int j);

/*!\brief set jth column name of a TableOfReals*/
void setColumnName(TableOfReals M, int j, const char * s);

/*!\brief get i,j-th string in a table*/
const char* getString(TableOfStrings S, int i, int j);

/*!\brief set i,jth string in a table*/
void setString(TableOfStrings S, int i, int j, const char * s);

/*!\brief get ith string in array of strings*/
const char* nthString(ArrayOfStrings S, int i);

/*!\brief set ith string in array of strings*/
void setNthString(ArrayOfStrings S, int i, const char * c);

/*!\brief get ith void * item in array of items*/
void * nthItem(ArrayOfItems A, int i);

/*!\brief set ith void * item in array of items*/
void setNthItem(ArrayOfItems A, int i, void * o);

/*!\brief delete a matrix*/
void deleteMatrix(TableOfReals *M);

/*!\brief delete a strings table*/
void deleteTableOfStrings(TableOfStrings *M);

/*!\brief delete an array */
void deleteArrayOfItems(ArrayOfItems *A);

/*!\brief delete an array */
void deleteArrayOfStrings(ArrayOfStrings *C);

/*!\brief combine two matrices by appending their columns. row size must be equal for both matrices*/
TableOfReals cbind(TableOfReals A, TableOfReals B);

/*!\brief combine two matrices by appending their row. column sizes must be equal for both matrices*/
TableOfReals rbind(TableOfReals A, TableOfReals B);

# if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
#   if defined(STATIC_LINKED)
#     define TCAPIEXPORT
#   else
#     define TCAPIEXPORT __declspec(dllexport)
#   endif
# else
#   if defined(__GNUC__) && defined(GCC_HASCLASSVISIBILITY)
#     define TCAPIEXPORT __attribute__ ((visibility("default")))
#   else
#     define TCAPIEXPORT
#   endif
# endif

#endif

