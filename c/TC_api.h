#ifndef TINKERCELL_TCAPI_H
#define TINKERCELL_TCAPI_H

#include "TCstructs.h"

void TCFreeMatrix(Matrix M);
void TCFreeArray(Array A);
void TCFreeChars(char** C);

#include "API/TC_Main_api.h"
#include "API/TC_BasicGraphicsToolbox_api.h"
#include "API/TC_BasicInformationTool_api.h"
#include "API/TC_ConnectionInsertion_api.h"
#include "API/TC_ConnectionSelection_api.h"
#include "API/TC_GroupHandlerTool_api.h"
#include "API/TC_NodeInsertion_api.h"
#include "API/TC_StoichiometryTool_api.h"
#include "API/TC_DynamicLibraryTool_api.h"
#include "API/TC_PlotTool_api.h"
#include "API/TC_ModelFileGenerator_api.h"
#include "API/TC_EventsAssignments_api.h"
#include "API/TC_NameFamily_api.h"
#include "API/TC_AutoGeneRegulatoryTool_api.h"
#include "API/TC_Antimony_api.h"

void TCFreeMatrix(Matrix M)
{
	int i;
	if (M.values)
		free(M.values);
	if (M.rownames)
	{
		for (i=0; i < (M).rows; ++i) free(M.rownames[i]);
		free(M.rownames);
	}
	if (M.colnames)
	{
		for (i=0; i < (M).cols; ++i) free(M.colnames[i]);
		free(M.colnames);
	}
}

void TCFreeArray(Array A)
{
	if (A) free(A);
}

void TCFreeChars(char** C)
{
	int i;
	if (!C) return;
	for (i=0; C[i] != 0; ++i) free(C[i]);
	free(C);
}

Matrix cbind(Matrix A, int fromA, int toA, Matrix B, int fromB, int toB)
{
	int i,j,k=0;
	Matrix C;

	C.colnames = C.rownames = 0;
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

	if (A.colnames && B.colnames)
	{
		C.colnames = (char**)malloc( (C.cols + 1) * sizeof(char*) );
		C.colnames[C.cols] = 0;
		for (i=0; i < A.cols; ++i)
		{
			k = 0;
			while (A.colnames[i] && A.colnames[i][k]) ++k;
			C.colnames[i] = (char*)malloc((1+k) * sizeof(char));
			C.colnames[i][k] = 0;
			for (j=0; j < k; ++j)
				C.colnames[i][j] = A.colnames[i][j];
		}
		for (i=0; i < B.cols; ++i)
		{
			k = 0;
			while (B.colnames[i] && B.colnames[i][k]) ++k;
			C.colnames[i+A.cols] = (char*)malloc((1+k) * sizeof(char));
			C.colnames[i+A.cols][k] = 0;
			for (j=0; j < k; ++j)
				C.colnames[i+A.cols][j] = B.colnames[i][j];
		}
	}

	if (A.rownames && B.rownames)
	{
		C.rownames = (char**)malloc( (C.rows + 1) * sizeof(char*) );
		C.rownames[C.rows] = 0;
		for (i=0; i < A.rows; ++i)
		{
			k = 0;
			while (A.rownames[i] && A.rownames[i][k]) ++k;
			C.rownames[i] = (char*)malloc((1+k) * sizeof(char));
			C.rownames[i][k] = 0;
			for (j=0; j < k; ++j)
				C.rownames[i][j] = A.rownames[i][j];
		}
	}

	return C;
}

Matrix rbind(Matrix A, int fromA, int toA, Matrix B, int fromB, int toB)
{
	int i,j,k=0;
	Matrix C;

	C.colnames = C.rownames = 0;
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

	if (A.rownames && B.rownames)
	{
		C.rownames = (char**)malloc( (C.rows + 1) * sizeof(char*) );
		C.rownames[C.rows] = 0;
		for (i=0; i < A.rows; ++i)
		{
			k = 0;
			while (A.rownames[i] && A.rownames[i][k]) ++k;
			C.rownames[i] = (char*)malloc((k+1) * sizeof(char));
			C.rownames[i][k] = 0;
			for (j=0; j < k; ++j)
				C.rownames[i][j] = A.colnames[i][j];
		}
		for (i=0; i < B.rows; ++i)
		{
			k = 0;
			while (B.rownames[i] && B.rownames[i][k]) ++k;
			C.rownames[i+A.cols] = (char*)malloc((1+k) * sizeof(char));
			C.rownames[i+A.cols][k] = 0;
			for (j=0; j < k; ++j)
				C.rownames[i+A.cols][j] = B.rownames[i][j];
		}
	}

	if (A.colnames && B.colnames)
	{
		C.colnames = (char**)malloc( (C.cols + 1) * sizeof(char*) );
		C.colnames[C.cols] = 0;
		for (i=0; i < A.cols; ++i)
		{
			k = 0;
			while (A.colnames[i] && A.colnames[i][k]) ++k;
			C.colnames[i] = (char*)malloc((1+k) * sizeof(char));
			C.colnames[i][k] = 0;
			for (j=0; j < k; ++j)
				C.colnames[i][j] = A.colnames[i][j];
		}
	}

	return C;
}

float max(float a, float b) { if (a > b) return a; else return b; }

float min(float a, float b) { if (a < b) return a; else return b; }

#endif
