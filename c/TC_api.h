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


#endif
