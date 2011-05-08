#ifndef TINKERCELL_CSTRUCTS_H
#define TINKERCELL_CSTRUCTS_H

#ifndef BEGIN_C_DECLS
#ifdef __cplusplus
#        define BEGIN_C_DECLS extern "C" {
#        define END_C_DECLS }
#   else
#        define BEGIN_C_DECLS
#        define END_C_DECLS
#endif
#endif

# ifndef TCAPIEXPORT
#  if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
#    if defined(STATIC_LINKED)
#      define TCAPIEXPORT
#    else
#            if defined(USE_STDCALL)
#                 define TCAPIEXPORT __stdcall __declspec(dllexport)
#            else
#                 define TCAPIEXPORT __declspec(dllexport)
#             endif
#     endif
#  else
#    if defined(__GNUC__) && defined(GCC_HASCLASSVISIBILITY)
#      define TCAPIEXPORT __attribute__ ((visibility("default")))
#    else
#      define TCAPIEXPORT
#    endif
#  endif
# endif //TCAPIEXPORT

BEGIN_C_DECLS

/*!\brief An array of strings with length information. Use tc_getString(M,i) to get the i-th string*/
typedef struct
{
	int length;
	char ** strings;
} tc_strings;


/*!\brief An array of int objects with length information. Use tc_getItem(M,i) to get the i-th item.*/
typedef struct
{
	int length;
	long* items;
} tc_items;


/*!\brief A 2D table of doubles with row and column names. Use tc_getMatrixValue(M,i,j) to get the i,j-th value in tc_matrix M.*/
typedef struct
{
	int rows, cols;
	double * values;
	tc_strings rownames;
	tc_strings colnames;
} tc_matrix;


/*!\brief A 2D table of strings with row and column names. Use tc_getTableValue(M,i,j) to get the i,j-th value in tc_matrix M.*/
typedef struct
{
	int rows, cols;
	char ** strings;
	tc_strings rownames;
	tc_strings colnames;
} tc_table;

/*!\brief Create a matrix with the given rows and columns
 \param int number of rows
 \param int number of columns
  \return tc_matrix
 \ingroup Basic
*/
TCAPIEXPORT tc_matrix tc_createMatrix(int rows, int cols);

/*!\brief Create a strings table with the given rows and columns
 \param int number of rows
 \param int number of columns
  \return tc_table
 \ingroup Basic
*/
TCAPIEXPORT tc_table tc_createTable(int rows, int cols);

/*!\brief Create an array of strings
 \param int length
 \return tc_strings
 \ingroup Basic
*/
TCAPIEXPORT tc_strings tc_createStringsArray(int len);

/*!\brief Create an array of items
 \param int number of items
 \return tc_items
 \ingroup Basic
*/
TCAPIEXPORT tc_items tc_createItemsArray(int len);

/*!\brief get i,jth value from a tc_matrix
 \param tc_matrix matrix
 \param int row
 \param int column
 \return double value at the given row, column
 \ingroup Basic
*/
TCAPIEXPORT double tc_getMatrixValue(tc_matrix M, int i, int j);

/*!\brief set i,jth value of a tc_matrix
 \param tc_matrix matrix
 \param int row
 \param int column
 \param double value at the given row, column
 \ingroup Basic
*/
TCAPIEXPORT void tc_setMatrixValue(tc_matrix M, int i, int j, double d);

/*!\brief get ith row name from a tc_matrix
 \param tc_matrix matrix
 \param int row
 \return string row name
 \ingroup Basic
*/
TCAPIEXPORT const char * tc_getRowName(tc_matrix M, int i);

/*!\brief set ith row name for a tc_matrix
 \param tc_matrix matrix
 \param int row
 \param string row name
 \ingroup Basic
*/
TCAPIEXPORT void tc_setRowName(tc_matrix M, int i, const char * s);

/*!\brief get jth column name of a tc_matrix
 \param tc_matrix matrix
 \param int column
 \return string column name
 \ingroup Basic
*/
TCAPIEXPORT const char * tc_getColumnName(tc_matrix M, int j);

/*!\brief set jth column name of a tc_matrix
 \param tc_matrix matrix
 \param int column
 \param string column name
 \ingroup Basic
*/
TCAPIEXPORT void tc_setColumnName(tc_matrix M, int j, const char * s);

/*!\brief get i,j-th string in a table
 \param tc_table table
 \param int row
 \param int column
 \return string value at row,column
 \ingroup Basic
*/
TCAPIEXPORT const char* tc_getTableValue(tc_table S, int i, int j);

/*!\brief set i,jth string in a table
 \param tc_table table
 \param int row
 \param int column
 \param string value at row,column
 \ingroup Basic
*/
TCAPIEXPORT void tc_setTableValue(tc_table S, int i, int j, const char * s);

/*!\brief get ith string in array of strings
 \param tc_strings array
 \param int index
 \return string value
 \ingroup Basic
*/
TCAPIEXPORT const char* tc_getString(tc_strings S, int i);

/*!\brief set ith string in array of strings
 \param tc_strings array
 \param int index
 \param string value
 \ingroup Basic
*/
TCAPIEXPORT void tc_setString(tc_strings S, int i, const char * c);

/*!\brief get ith long item in array of items
\param tc_items array
 \param int index
 \return long value
 \ingroup Basic
*/
TCAPIEXPORT long tc_getItem(tc_items A, int i);

/*!\brief set ith long item in array of items
 \param tc_items array
 \param int index
 \param long value
 \ingroup Basic
*/
TCAPIEXPORT void tc_setItem(tc_items A, int i, long o);

/*!\brief get the index of a string in the array
 \param tc_strings array
 \param char* a string in the array
 \return int index of that string
 \ingroup Basic
*/
TCAPIEXPORT int tc_getStringIndex(tc_strings A, const char * s);

/*!\brief get the row number of a row name
 \param tc_matrix matrix
 \param char* a string in the matrix
 \return int index of that string
 \ingroup Basic
*/
TCAPIEXPORT int tc_getRowIndex(tc_matrix, const char * s);

/*!\brief get the column number of a column name
 \param tc_matrix matrix
 \param char* a string in the matrix
 \return int index of that string
 \ingroup Basic
*/
TCAPIEXPORT int tc_getColumnIndex(tc_matrix, const char * s);

/*!\brief delete a matrix
 \param &tc_matrix pointer to matrix
 \ingroup Basic
*/
TCAPIEXPORT void tc_deleteMatrix(tc_matrix M);

/*!\brief delete a strings table
 \param &tc_table pointer to table
 \ingroup Basic
*/
TCAPIEXPORT void tc_deleteTable(tc_table M);

/*!\brief delete an array of items
 \param &tc_items pointer to array
 \ingroup Basic
*/
TCAPIEXPORT void tc_deleteItemsArray(tc_items A);

/*!\brief delete an array of strings
 \param &tc_strings pointer to array
 \ingroup Basic
*/
TCAPIEXPORT void tc_deleteStringsArray(tc_strings C);

/*!\brief combine two matrices by appending their columns. row size must be equal for both matrices
 \param tc_matrix first matrix
 \param tc_matrix fsecond matrix
 \return tc_matrix new combined matrix
 \ingroup Basic
*/
TCAPIEXPORT tc_matrix tc_appendColumns(tc_matrix A, tc_matrix B);

/*!\brief combine two matrices by appending their row. column sizes must be equal for both matrices
 \param tc_matrix first matrix
 \param tc_matrix fsecond matrix
 \return tc_matrix new combined matrix
 \ingroup Basic
*/
TCAPIEXPORT tc_matrix tc_appendRows(tc_matrix A, tc_matrix B);

/*!\brief print a matrix to file
 \param char* file name
 \param tc_matrix
 \ingroup Basic
*/
TCAPIEXPORT void tc_printMatrixToFile(const char* file, tc_matrix M);

/*!\brief print a matrix to stdout
 \param char* file name
 \param tc_matrix
 \ingroup Basic
*/
TCAPIEXPORT void tc_printOutMatrix(tc_matrix M);

/*!\brief print a table to file
 \param char* file name
 \param tc_table
 \ingroup Basic
*/
TCAPIEXPORT void tc_printTableToFile(const char* file, tc_table M);

/*!\brief print a table to stdout
 \param tc_table
 \ingroup Basic
*/
TCAPIEXPORT void tc_printOutTable(tc_table M);

END_C_DECLS
#endif

