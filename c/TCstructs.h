#ifndef TINKERCELL_CSTRUCTS_H
#define TINKERCELL_CSTRUCTS_H

#ifdef __cplusplus
#        define BEGIN_C_DECLS extern "C" {
#        define END_C_DECLS }
#   else
#        define BEGIN_C_DECLS
#        define END_C_DECLS
#endif

# ifndef TCAPIEXPORT
#  if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
#    if defined(STATIC_LINKED)
#      define TCAPIEXPORT
#    else
#      define TCAPIEXPORT __declspec(dllexport)
#     endif
#  else
#    if defined(__GNUC__) && defined(GCC_HASCLASSVISIBILITY)
#      define TCAPIEXPORT __attribute__ ((visibility("default")))
#    else
#      define TCAPIEXPORT
#    endif
#  endif
# endif //TCAPIEXPORT

/*!\brief An array of strings with length information. Use nthString(M,i) to get the i-th string.*/
typedef struct TCAPIEXPORT ArrayOfStrings
{
	int length;
	char ** strings;
} ArrayOfStrings;


/*!\brief An array of int objects with length information. Use nthItem(M,i) to get the i-th item.*/
typedef struct TCAPIEXPORT ArrayOfItems
{
	int length;
	int* items;
} ArrayOfItems;


/*!\brief A 2D table of doubles with row and column names. Use getValue(M,i,j) to get the i,j-th value in TableOfReals M.*/
typedef struct TCAPIEXPORT TableOfReals
{
	int rows, cols;
	double * values;
	ArrayOfStrings rownames;
	ArrayOfStrings colnames;
} TableOfReals;


/*!\brief A 2D table of strings with row and column names. Use getString(M,i,j) to get the i,j-th value in TableOfReals M.*/
typedef struct TCAPIEXPORT TableOfStrings
{
	int rows, cols;
	char ** strings;
	ArrayOfStrings rownames;
	ArrayOfStrings colnames;
} TableOfStrings;

BEGIN_C_DECLS

/*!\brief Create a matrix with the given rows and columns*/
TCAPIEXPORT TableOfReals newMatrix(int rows, int cols);

/*!\brief Create a strings table with the given rows and columns*/
TCAPIEXPORT TableOfStrings newTableOfStrings(int rows, int cols);

/*!\brief Create an array of strings*/
TCAPIEXPORT ArrayOfStrings newArrayOfStrings(int len);

/*!\brief Create an array of items*/
TCAPIEXPORT ArrayOfItems newArrayOfItems(int len);

/*!\brief get i,jth value from a TableOfReals*/
TCAPIEXPORT double getValue(TableOfReals M, int i, int j);

/*!\brief set i,jth value of a TableOfReals*/
TCAPIEXPORT void setValue(TableOfReals M, int i, int j, double d);

/*!\brief get ith row name from a TableOfReals*/
TCAPIEXPORT const char * getRowName(TableOfReals M, int i);

/*!\brief set ith row name for a TableOfReals*/
TCAPIEXPORT void setRowName(TableOfReals M, int i, const char * s);

/*!\brief get jth column name of a TableOfReals*/
TCAPIEXPORT const char * getColumnName(TableOfReals M, int j);

/*!\brief set jth column name of a TableOfReals*/
TCAPIEXPORT void setColumnName(TableOfReals M, int j, const char * s);

/*!\brief get i,j-th string in a table*/
TCAPIEXPORT const char* getString(TableOfStrings S, int i, int j);

/*!\brief set i,jth string in a table*/
TCAPIEXPORT void setString(TableOfStrings S, int i, int j, const char * s);

/*!\brief get ith string in array of strings*/
TCAPIEXPORT const char* nthString(ArrayOfStrings S, int i);

/*!\brief set ith string in array of strings*/
TCAPIEXPORT void setNthString(ArrayOfStrings S, int i, const char * c);

/*!\brief get ith int item in array of items*/
TCAPIEXPORT int nthItem(ArrayOfItems A, int i);

/*!\brief set ith int item in array of items*/
TCAPIEXPORT void setNthItem(ArrayOfItems A, int i, int o);

/*!\brief delete a matrix*/
TCAPIEXPORT void deleteMatrix(TableOfReals *M);

/*!\brief delete a strings table*/
TCAPIEXPORT void deleteTableOfStrings(TableOfStrings *M);

/*!\brief delete an array */
TCAPIEXPORT void deleteArrayOfItems(ArrayOfItems *A);

/*!\brief delete an array */
TCAPIEXPORT void deleteArrayOfStrings(ArrayOfStrings *C);

/*!\brief combine two matrices by appending their columns. row size must be equal for both matrices*/
TCAPIEXPORT TableOfReals cbind(TableOfReals A, TableOfReals B);

/*!\brief combine two matrices by appending their row. column sizes must be equal for both matrices*/
TCAPIEXPORT TableOfReals rbind(TableOfReals A, TableOfReals B);

END_C_DECLS
#endif

