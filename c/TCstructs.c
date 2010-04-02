#include "TCstructs.h"

Matrix newMatrix(int rows, int cols)
{
	int i;
	Matrix M;
	M.rows = rows;
	M.cols = cols;
	M.colnames = newArrayOfStrings(cols);
	M.rownames = newArrayOfStrings(rows);
	
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

TableOfStrings newTableOfStrings(int rows, int cols)
{
	int i;
	TableOfStrings M;
	M.rows = rows;
	M.cols = cols;

	M.colnames = newArrayOfStrings(cols);
	M.rownames = newArrayOfStrings(rows);
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

ArrayOfStrings newArrayOfStrings(int len)
{
	int i;
	ArrayOfStrings A;
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

ArrayOfItems newArrayOfItems(int len)
{
	int i;
	ArrayOfItems A;
	if (len < 1)
	{
		A.length = 0;
		A.items = 0;
	}
	else
	{
		A.length = len;
		A.items = (void**)malloc(len * sizeof(void*));
		for (i=0; i < len; ++i)
			A.items[i] = 0;
	}
	return A;
}
double getValue(Matrix M, int i, int j)
{ 
	if (i >= 0 && j >= 0 && i < M.rows && j < M.cols)
		return M.values[ i*M.cols + j ];
	return 0.0;
}

void setValue(Matrix M, int i, int j, double d)
{ 
	if (i >= 0 && j >= 0 && i < M.rows && j < M.cols)
		M.values[ i*M.cols + j ] = d;
}

const char * getRowName(Matrix M, int i)
{ 
	return nthString(M.rownames,i);
}

void setRowName(Matrix M, int i, const char * s)
{
	setNthString(M.rownames,i,s);
}

const char * getColumnName(Matrix M, int i)
{ 
	return nthString(M.colnames,i);
}

void setColumnName(Matrix M, int i, const char * s)
{
	setNthString(M.colnames,i,s);
}

const char* getString(TableOfStrings S, int i, int j)
{
	if (i >= 0 && j >= 0 && i < S.rows && j < S.cols)
		return S.strings[ i*S.cols + j ];
	return 0;
}

void setString(TableOfStrings S, int i, int j, const char * s)
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

const char* nthString(ArrayOfStrings S, int i)
{
	if (i >= 0 && i < S.length)
		return S.strings[ i ];
	return 0;
}

void setNthString(ArrayOfStrings S, int i, const char * s)
{
	int n=0;
	char * str;
	if (i >= 0 && i < S.length)
	{
		while (s && s[n]) ++n;
		str = (char*)malloc((n+1)*sizeof(char));
		sprintf(str,"%s\0",s);
	
		S.strings[ i ] = str;
	}
}

void * nthItem(ArrayOfItems A, int i)
{
	if (i >= 0 && i < A.length)
		return A.items[ i ];
	return 0;
}

void setNthItem(ArrayOfItems A, int i, void * o)
{
	if (i >= 0 && i < A.length)
		A.items[ i ] = o;
}

void deleteMatrix(Matrix * M)
{
	if (!M) return;
	if (M->values)
		free(M->values);
	M->rows = M->cols = 0;	
	M->values = 0;
	deleteArrayOfStrings(&M->rownames);
	deleteArrayOfStrings(&M->colnames);

}

void deleteTableOfStrings(TableOfStrings * M)
{
	if (!M) return;
	if (M->strings)
		free(M->strings);
	M->rows = M->cols = 0;
	M->strings = 0;
	deleteArrayOfStrings(&M->rownames);
	deleteArrayOfStrings(&M->colnames);

}

void deleteArrayOfItems(ArrayOfItems * A)
{
	if (!A) return;
	if (A->items) 
		free(A->items);
	A->length = 0;
	A->items = 0;
}

void deleteArrayOfStrings(ArrayOfStrings * C)
{
	int i;
	if (!C) return;
	if (C->strings)
	{
		for (i=0; i < C->length; ++i) 
			if (C->strings[i]) 
				free(C->strings[i]);
		free(C->strings);
	}
	C->length = 0;
	C->strings = 0;
}

Matrix cbind(Matrix A, Matrix B)
{
	int i,j,k=0;
	Matrix C;
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

Matrix rbind(Matrix A, Matrix B)
{
	int i,j,k=0;
	Matrix C;
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

