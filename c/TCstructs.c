#include "TCstructs.h"

Matrix newMatrix(int rows, int cols)
{
	int i;
	Matrix M;
	M.rows = rows;
	M.cols = cols;
	M.colnames = (char**)malloc( (cols+1) * sizeof(char*) );
	M.rownames = (char**)malloc( (rows+1) * sizeof(char*) );
	for (i=0; i < cols+1; ++i)
		M.colnames[i] = 0;
	for (i=0; i < rows+1; ++i)
		M.rownames[i] = 0;
	return M;
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
	if (i >= 0 && i < M.rows)
		return M.rownames[i];
	return "";
}

void setRowName(Matrix M, int i, const char * s)
{
	int n=0;
	char * str;
	if (i >= 0 && i < M.rows)
	{
		while (s && s[n]) ++n;
		str = (char*)malloc((n+1)*sizeof(char));
		sprintf(str,"%s\0",s);
		M.rownames[i] = str;
	}
}

const char * getColumnName(Matrix M, int j)
{ 
	if (j >= 0 && j < M.cols)
		return M.colnames[j];
	return "";
}

void setColumnName(Matrix M, int j, const char * s)
{
	int n=0;
	char * str;
	if (j >= 0 && j < M.cols)
	{
		while (s && s[n]) ++n;
		str = (char*)malloc((n+1)*sizeof(char));
		sprintf(str,"%s\0",s);
		M.colnames[j] = str;
	}
}

const char* getString(TableOfStrings S, int i, int j)
{
	if (i >= 0 && j >= 0 && i < S.rows && j < S.cols)
		return S.strings[ i*S.cols + j ];
	return "";
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

const char* ithString(ArrayOfStrings S, int i)
{
	if (i >= 0 && i < S.length)
		return S.strings[ i ];
	return "";
}

void * ithItem(ArrayOfItems A, int i)
{
	if (i >= 0 && i < A.length)
		return A.items[ i ];
	return 0;
}

