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

/*!\brief An array of strings with length information. Use tc_getString(M,i) to get the i-th string.*/
typedef struct TCAPIEXPORT tc_strings
{
	int length;
	char ** strings;
} tc_strings;


/*!\brief An array of int objects with length information. Use tc_getItem(M,i) to get the i-th item.*/
typedef struct TCAPIEXPORT tc_items
{
	int length;
	long* items;
} tc_items;


/*!\brief A 2D table of doubles with row and column names. Use tc_getMatrixValue(M,i,j) to get the i,j-th value in tc_matrix M.*/
typedef struct TCAPIEXPORT tc_matrix
{
	int rows, cols;
	double * values;
	tc_strings rownames;
	tc_strings colnames;
} tc_matrix;


/*!\brief A 2D table of strings with row and column names. Use tc_getTableValue(M,i,j) to get the i,j-th value in tc_matrix M.*/
typedef struct TCAPIEXPORT tc_table
{
	int rows, cols;
	char ** strings;
	tc_strings rownames;
	tc_strings colnames;
} tc_table;

BEGIN_C_DECLS

/*!\brief Create a matrix with the given rows and columns*/
TCAPIEXPORT tc_matrix tc_createMatrix(int rows, int cols);

/*!\brief Create a strings table with the given rows and columns*/
TCAPIEXPORT tc_table tc_createTable(int rows, int cols);

/*!\brief Create an array of strings*/
TCAPIEXPORT tc_strings tc_createStringsArray(int len);

/*!\brief Create an array of items*/
TCAPIEXPORT tc_items tc_createItemsArray(int len);

/*!\brief get i,jth value from a tc_matrix*/
TCAPIEXPORT double tc_getMatrixValue(tc_matrix M, int i, int j);

/*!\brief set i,jth value of a tc_matrix*/
TCAPIEXPORT void tc_setMatrixValue(tc_matrix M, int i, int j, double d);

/*!\brief get ith row name from a tc_matrix*/
TCAPIEXPORT const char * tc_getRowName(tc_matrix M, int i);

/*!\brief set ith row name for a tc_matrix*/
TCAPIEXPORT void tc_setRowName(tc_matrix M, int i, const char * s);

/*!\brief get jth column name of a tc_matrix*/
TCAPIEXPORT const char * tc_getColumnName(tc_matrix M, int j);

/*!\brief set jth column name of a tc_matrix*/
TCAPIEXPORT void tc_setColumnName(tc_matrix M, int j, const char * s);

/*!\brief get i,j-th string in a table*/
TCAPIEXPORT const char* tc_getTableValue(tc_table S, int i, int j);

/*!\brief set i,jth string in a table*/
TCAPIEXPORT void tc_setTableValue(tc_table S, int i, int j, const char * s);

/*!\brief get ith string in array of strings*/
TCAPIEXPORT const char* tc_getString(tc_strings S, int i);

/*!\brief set ith string in array of strings*/
TCAPIEXPORT void tc_setString(tc_strings S, int i, const char * c);

/*!\brief get ith long item in array of items*/
TCAPIEXPORT long tc_getItem(tc_items A, int i);

/*!\brief set ith long item in array of items*/
TCAPIEXPORT void tc_setItem(tc_items A, int i, long o);

/*!\brief delete a matrix*/
TCAPIEXPORT void tc_deleteMatrix(tc_matrix *M);

/*!\brief delete a strings table*/
TCAPIEXPORT void tc_deleteTable(tc_table *M);

/*!\brief delete an array */
TCAPIEXPORT void tc_deleteItemsArray(tc_items *A);

/*!\brief delete an array */
TCAPIEXPORT void tc_deleteStringsArray(tc_strings *C);

/*!\brief combine two matrices by appending their columns. row size must be equal for both matrices*/
TCAPIEXPORT tc_matrix tc_appendColumns(tc_matrix A, tc_matrix B);

/*!\brief combine two matrices by appending their row. column sizes must be equal for both matrices*/
TCAPIEXPORT tc_matrix tc_appendRows(tc_matrix A, tc_matrix B);

END_C_DECLS
#endif

