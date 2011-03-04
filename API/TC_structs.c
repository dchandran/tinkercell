#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "TC_structs.h"

TCAPIEXPORT tc_matrix tc_createMatrix(int rows, int cols)
{
	int i;
	tc_matrix M;
	M.rows = rows;
	M.cols = cols;
	M.colnames = tc_createStringsArray(cols);
	M.rownames = tc_createStringsArray(rows);
	
	if (rows > 0 && cols > 0)
	{
		M.values = (double*)malloc( rows * cols * sizeof(double) );
		for (i=0; i < cols; ++i)
			M.colnames.strings[i] = 0;
		for (i=0; i < rows; ++i)
			M.rownames.strings[i] = 0;
		for (i=0; i < rows*cols; ++i)
			M.values[i] = 0.0;
	}
	else
		M.values = 0;
	return M;
}

TCAPIEXPORT tc_table tc_createTable(int rows, int cols)
{
	int i;
	tc_table M;
	M.rows = rows;
	M.cols = cols;

	M.colnames = tc_createStringsArray(cols);
	M.rownames = tc_createStringsArray(rows);
	if (rows > 0 && cols > 0)
	{
		M.strings = (char**)malloc( rows * cols * sizeof(char*) );
		for (i=0; i < cols; ++i)
			M.colnames.strings[i] = 0;
		for (i=0; i < rows; ++i)
			M.rownames.strings[i] = 0;
		for (i=0; i < rows*cols; ++i)
			M.strings[i] = 0;
	}
	else
		M.strings = 0;
	return M;
}

TCAPIEXPORT tc_strings tc_createStringsArray(int len)
{
	int i;
	tc_strings A;
	if (len < 1)
	{
		A.length = 0;
		A.strings = 0;
	}
	else
	{
		A.length = len;
		A.strings = (char**)malloc(len * sizeof(char*));
		for (i=0; i < len; ++i)
			A.strings[i] = 0;
	}
	return A;
}

TCAPIEXPORT tc_items tc_createItemsArray(int len)
{
	int i;
	tc_items A;
	if (len < 1)
	{
		A.length = 0;
		A.items = 0;
	}
	else
	{
		A.length = len;
		A.items = (long*)malloc(len * sizeof(long));
		for (i=0; i < len; ++i)
			A.items[i] = 0;
	}
	return A;
}

TCAPIEXPORT double tc_getMatrixValue(tc_matrix M, int i, int j)
{ 
	if (M.values && i >= 0 && j >= 0 && i < M.rows && j < M.cols)
		return M.values[ i*M.cols + j ];
	return 0.0;
}

TCAPIEXPORT void tc_setMatrixValue(tc_matrix M, int i, int j, double d)
{ 
	if (M.values && i >= 0 && j >= 0 && i < M.rows && j < M.cols)
		M.values[ i*M.cols + j ] = d;
}

TCAPIEXPORT const char * tc_getRowName(tc_matrix M, int i)
{ 
	return tc_getString(M.rownames,i);
}

TCAPIEXPORT void tc_setRowName(tc_matrix M, int i, const char * s)
{
	tc_setString(M.rownames,i,s);
}

TCAPIEXPORT const char * tc_getColumnName(tc_matrix M, int i)
{ 
	return tc_getString(M.colnames,i);
}

TCAPIEXPORT void tc_setColumnName(tc_matrix M, int i, const char * s)
{
	tc_setString(M.colnames,i,s);
}

TCAPIEXPORT const char* tc_getTableValue(tc_table S, int i, int j)
{
	if (S.strings && i >= 0 && j >= 0 && i < S.rows && j < S.cols)
		return S.strings[ i*S.cols + j ];
	return 0;
}

TCAPIEXPORT void tc_setTableValue(tc_table S, int i, int j, const char * s)
{
	int n=0;
	char * str;
	if (i >= 0 && j >= 0 && i < S.rows && j < S.cols)
	{
		while (s && s[n]) ++n;
		str = (char*)malloc((n+1)*sizeof(char));
		sprintf(str,"%s\0",s);
	
		S.strings[ i*S.cols + j ] = str;
	}
}

TCAPIEXPORT const char* tc_getString(tc_strings S, int i)
{
	if (S.strings && i >= 0 && i < S.length)
		return S.strings[ i ];
	return 0;
}

TCAPIEXPORT void tc_setString(tc_strings S, int i, const char * s)
{
	int n=0;
	char * str;
	if (i >= 0 && i < S.length)
	{
		while (s && s[n]) ++n;
		
		if (n > 0)
		{
			str = (char*)malloc((n+1)*sizeof(char));
			sprintf(str,"%s\0",s);
			if (S.strings[i])
				free(S.strings[i]);
			S.strings[ i ] = str;
		}
	}
}

TCAPIEXPORT long tc_getItem(tc_items A, int i)
{
	if (i >= 0 && i < A.length)
		return A.items[ i ];
	return 0;
}

TCAPIEXPORT void tc_setItem(tc_items A, int i, long o)
{
	if (i >= 0 && i < A.length)
		A.items[ i ] = o;
}

TCAPIEXPORT void tc_deleteMatrix(tc_matrix M)
{
	if (M.values)
		free(M.values);
	M.rows = M.cols = 0;	
	M.values = 0;
	tc_deleteStringsArray(M.rownames);
	tc_deleteStringsArray(M.colnames);
}

TCAPIEXPORT void tc_deleteTable(tc_table M)
{
	if (M.strings)
		free(M.strings);
	M.rows = M.cols = 0;
	M.strings = 0;
	tc_deleteStringsArray(M.rownames);
	tc_deleteStringsArray(M.colnames);
}

TCAPIEXPORT void tc_deleteItemsArray(tc_items A)
{
	if (A.items) 
		free(A.items);
	A.length = 0;
	A.items = 0;
}

TCAPIEXPORT void tc_deleteStringsArray(tc_strings C)
{
	int i;
	if (C.strings)
	{
		for (i=0; i < C.length; ++i) 
			if (C.strings[i]) 
				free(C.strings[i]);
		free(C.strings);
	}
	C.length = 0;
	C.strings = 0;
}

TCAPIEXPORT tc_matrix tc_appendColumns(tc_matrix A, tc_matrix B)
{
	int i,j,k=0;
	tc_matrix C;
	int fromA = 0, toA = A.cols, fromB = 0, toB = B.cols;

	C.colnames.length = C.rownames.length = 0;
	C.colnames.strings = C.rownames.strings = 0;
	C.rows = C.cols = 0;
	C.values = 0;

	if (A.rows != B.rows) return C;
	if (fromA < 0 || toA < 0 || fromA > A.cols || toA > A.cols ||
		fromB < 0 || toB < 0 || fromB > B.cols || toB > B.cols ||
		fromA >= toA || fromB >= toB)
		return C;

	C.rows = A.rows;
	C.cols = ((toA - fromA) + (toB - fromB));
	C.values = (double*)malloc( C.rows * C.cols * sizeof(double) );
	
	for (i=0; i < (C.rows*C.cols); ++i)
		C.values[i] = 0.0;

	if (A.colnames.strings && B.colnames.strings)
	{
		C.colnames.length = C.cols;
		C.colnames.strings = (char**)malloc( C.cols * sizeof(char*) );
		for (i=0; i < A.cols; ++i)
		{
			k = 0;
			while (A.colnames.strings[i] && A.colnames.strings[i][k]) ++k;
			C.colnames.strings[i] = (char*)malloc((1+k) * sizeof(char));
			C.colnames.strings[i][k] = 0;
			for (j=0; j < k; ++j)
				C.colnames.strings[i][j] = A.colnames.strings[i][j];
		}
		for (i=0; i < B.cols; ++i)
		{
			k = 0;
			while (B.colnames.strings[i] && B.colnames.strings[i][k]) ++k;
			C.colnames.strings[i+A.cols] = (char*)malloc((1+k) * sizeof(char));
			C.colnames.strings[i+A.cols][k] = 0;
			for (j=0; j < k; ++j)
				C.colnames.strings[i+A.cols][j] = B.colnames.strings[i][j];
		}
	}

	if (A.rownames.strings && B.rownames.strings)
	{
		C.rownames.length = C.rows;
		C.rownames.strings = (char**)malloc( C.rows * sizeof(char*) );
		for (i=0; i < A.rows; ++i)
		{
			k = 0;
			while (A.rownames.strings[i] && A.rownames.strings[i][k]) ++k;
			C.rownames.strings[i] = (char*)malloc((1+k) * sizeof(char));
			C.rownames.strings[i][k] = 0;
			for (j=0; j < k; ++j)
				C.rownames.strings[i][j] = A.rownames.strings[i][j];
		}
	}
	
	k = (toA - fromA);
	for (i=fromA; i < toA; ++i)
	{
		for (j=0; j < C.rows; ++j)
			C.values[ j*C.cols + i - fromA ] = A.values[ j * A.cols + i ];
	}
	
	for (i=fromB; i < toB; ++i)
	{
		for (j=0; j < C.rows; ++j)
			C.values[ j*C.cols + k + i - fromB ] = B.values[ j * B.cols + i ];
	}
	return C;
}

TCAPIEXPORT tc_matrix tc_appendRows(tc_matrix A, tc_matrix B)
{
	int i,j,k=0;
	tc_matrix C;
	int fromA = 0, toA = A.rows, fromB = 0, toB = B.rows;

	C.colnames.strings = C.rownames.strings = 0;
	C.colnames.length = C.rownames.length = 0;
	C.rows = C.cols = 0;
	C.values = 0;

	if (A.cols != B.cols) return C;
	if (fromA < 0 || toA < 0 || fromA > A.cols || toA > A.cols ||
		fromB < 0 || toB < 0 || fromB > B.cols || toB > B.cols ||
		fromA >= toA || fromB >= toB)
		return C;

	C.cols = A.cols;
	C.rows = ((toA - fromA) + (toB - fromB));
	C.values = (double*)malloc( C.rows * C.cols * sizeof(double) );
	
	for (i=0; i < (C.rows*C.cols); ++i)
		C.values[i] = 0.0;

	if (A.rownames.strings && B.rownames.strings)
	{
		C.rownames.length = C.rows;
		C.rownames.strings = (char**)malloc( C.rows * sizeof(char*) );
		for (i=0; i < A.rows; ++i)
		{
			k = 0;
			while (A.rownames.strings[i] && A.rownames.strings[i][k]) ++k;
			C.rownames.strings[i] = (char*)malloc((k+1) * sizeof(char));
			C.rownames.strings[i][k] = 0;
			for (j=0; j < k; ++j)
				C.rownames.strings[i][j] = A.colnames.strings[i][j];
		}
		for (i=0; i < B.rows; ++i)
		{
			k = 0;
			while (B.rownames.strings[i] && B.rownames.strings[i][k]) ++k;
			C.rownames.strings[i+A.cols] = (char*)malloc((1+k) * sizeof(char));
			C.rownames.strings[i+A.cols][k] = 0;
			for (j=0; j < k; ++j)
				C.rownames.strings[i+A.cols][j] = B.rownames.strings[i][j];
		}
	}


	if (A.colnames.strings && B.colnames.strings)
	{
		C.colnames.length = C.cols;
		C.colnames.strings = (char**)malloc( (C.cols + 1) * sizeof(char*) );
		C.colnames.strings[C.cols] = 0;
		for (i=0; i < A.cols; ++i)
		{
			k = 0;
			while (A.colnames.strings[i] && A.colnames.strings[i][k]) ++k;
			C.colnames.strings[i] = (char*)malloc((1+k) * sizeof(char));
			C.colnames.strings[i][k] = 0;
			for (j=0; j < k; ++j)
				C.colnames.strings[i][j] = A.colnames.strings[i][j];
		}
	}
	
	k = (toA - fromA);
	for (i=fromA; i < toA; ++i)
	{
		for (j=0; j < C.cols; ++j)
			C.values[ (i-fromA)*C.cols + j ] = A.values[ i * A.cols + j ];
	}
	
	for (i=fromB; i < toB; ++i)
	{
		for (j=0; j < C.cols; ++j)
			C.values[ (i + k - fromB)*C.cols + j ] = B.values[ i * B.cols + j ];
	}

	return C;
}

TCAPIEXPORT void tc_printMatrixToFile(const char* s, tc_matrix output)
{
	int i,j;
	FILE * outfile = fopen(s,"w+");
	if (output.colnames.strings)
	{
		fprintf(outfile, "#\t");
		for (j=0; j < output.cols; ++j)
			if (j < (output.cols-1))
				fprintf(outfile, "%s\t", tc_getColumnName(output, j));
			else
				fprintf(outfile, "%s\n", tc_getColumnName(output, j));
	}
	for (i=0; i < output.rows; ++i)
	{
		if (tc_getRowName(output,i))
			fprintf(outfile, "%s\t", tc_getRowName(output, i));
		for (j=0; j < output.cols; ++j)
			if (j < (output.cols-1))
				fprintf(outfile, "%lf\t", tc_getMatrixValue(output, i, j));
			else
				fprintf(outfile, "%lf\n", tc_getMatrixValue(output, i, j));
	}
	
	fclose(outfile);
}

TCAPIEXPORT void tc_printOutMatrix(tc_matrix output)
{
	int i,j;
	if (output.colnames.strings)
	{
		printf("\t");
		for (j=0; j < output.cols; ++j)
			if (j < (output.cols-1))
				printf("%s\t", tc_getColumnName(output, j));
			else
				printf("%s\n", tc_getColumnName(output, j));
	}

	for (i=0; i < output.rows; ++i)
	{
		if (tc_getRowName(output,i))
			printf("%s\t", tc_getRowName(output, i));
		for (j=0; j < output.cols; ++j)
			if (j < (output.cols-1))
				printf("%lf\t", tc_getMatrixValue(output, i, j));
			else
				printf("%lf\n", tc_getMatrixValue(output, i, j));
	}
}


TCAPIEXPORT void tc_printTableToFile(const char* s, tc_table output)
{
	int i,j;
	FILE * outfile = fopen(s,"w+");
	if (output.colnames.strings)
	{
		fprintf(outfile, "#\t");
		for (j=0; j < output.cols; ++j)
			if (j < (output.cols-1))
				fprintf(outfile, "%s\t", tc_getString(output.colnames, j));
			else
				fprintf(outfile, "%s\n", tc_getString(output.colnames, j));
	}

	if (output.strings)
	{
		for (i=0; i < output.rows; ++i)
		{
			if (tc_getString(output.rownames,i))
				fprintf(outfile, "%s\t", tc_getString(output.rownames,i));
			for (j=0; j < output.cols; ++j)
				if (j < (output.cols-1))
					fprintf(outfile, "%s\t", tc_getTableValue(output, i, j));
				else
					fprintf(outfile, "%s\n", tc_getTableValue(output, i, j));
		}
	}
	fclose(outfile);
}

TCAPIEXPORT void tc_printOutTable(tc_table output)
{
	int i,j;
	if (output.colnames.strings)
	{
		printf("\t");
		for (j=0; j < output.cols; ++j)
			if (j < (output.cols-1))
				printf("%s\t", tc_getString(output.colnames, j));
			else
				printf("%s\n", tc_getString(output.colnames, j));
	}
	
	if (output.strings)
	{
		for (i=0; i < output.rows; ++i)
		{
			if (tc_getString(output.rownames,i))
				printf("%s\t", tc_getString(output.rownames,i));
			for (j=0; j < output.cols; ++j)
				if (j < (output.cols-1))
					printf("%s\t", tc_getTableValue(output, i, j));
				else
					printf("%s\n", tc_getTableValue(output, i, j));
		}
	}
}

