#include "TCstructs.h"

struct Matrix newMatrix(int rows, int cols)
{
	int i;
	struct Matrix M;
	M.rows = M.rownames.length = rows;
	M.cols = M.colnames.length = cols;
	M.colnames.strings = (char**)malloc( (cols+1) * sizeof(char*) );
	M.rownames.strings = (char**)malloc( (rows+1) * sizeof(char*) );
	for (i=0; i < cols+1; ++i)
		M.colnames.strings[i] = 0;
	for (i=0; i < rows+1; ++i)
		M.rownames.strings[i] = 0;
	return M;
}

double getValue(struct Matrix M, int i, int j)
{ 
	if (i >= 0 && j >= 0 && i < M.rows && j < M.cols)
		return M.values[ i*M.cols + j ];
	return 0.0;
}

void setValue(struct Matrix M, int i, int j, double d)
{ 
	if (i >= 0 && j >= 0 && i < M.rows && j < M.cols)
		M.values[ i*M.cols + j ] = d;
}

const char * getRowName(struct Matrix M, int i)
{ 
	if (i >= 0 && i < M.rows)
		return M.rownames.strings[i];
	return "";
}

void setRowName(struct Matrix M, int i, const char * s)
{
	int n=0;
	char * str;
	if (i >= 0 && i < M.rows)
	{
		while (s && s[n]) ++n;
		str = (char*)malloc((n+1)*sizeof(char));
		sprintf(str,"%s\0",s);
		M.rownames.strings[i] = str;
	}
}

const char * getColumnName(struct Matrix M, int j)
{ 
	if (j >= 0 && j < M.cols)
		return M.colnames.strings[j];
	return "";
}

void setColumnName(struct Matrix M, int j, const char * s)
{
	int n=0;
	char * str;
	if (j >= 0 && j < M.cols)
	{
		while (s && s[n]) ++n;
		str = (char*)malloc((n+1)*sizeof(char));
		sprintf(str,"%s\0",s);
		M.colnames.strings[j] = str;
	}
}

const char* getString(struct TableOfStrings S, int i, int j)
{
	if (i >= 0 && j >= 0 && i < S.rows && j < S.cols)
		return S.strings[ i*S.cols + j ];
	return "";
}

void setString(struct TableOfStrings S, int i, int j, const char * s)
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

const char* ithString(struct ArrayOfStrings S, int i)
{
	if (i >= 0 && i < S.length)
		return S.strings[ i ];
	return "";
}

void * ithItem(struct ArrayOfItems A, int i)
{
	if (i >= 0 && i < A.length)
		return A.items[ i ];
	return 0;
}

