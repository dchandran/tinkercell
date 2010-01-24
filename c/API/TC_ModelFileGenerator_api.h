#ifndef TINKERCELL_TC_MODELFILEGENERATOR_API_H
#define TINKERCELL_TC_MODELFILEGENERATOR_API_H

#include "../TCstructs.h"

Matrix (*_tc_getModelParameters)(Array) = 0;
/*! 
 \brief get only the parameters used in the model
 \ingroup Attributes
*/
Matrix tc_getModelParameters(Array A)
{
	Matrix M;
	if (_tc_getModelParameters)
		return _tc_getModelParameters(A);
	M.rows = M.cols = 0;
	M.rownames = M.colnames = 0;
	M.values = 0;
	return M;
}

int (*_tc_writeModel)(const char* file, Array items) = 0;
/*! 
 \brief write the ODE, stoichiometry, and rates functions to a file
 \ingroup Modeling
*/
int tc_writeModel(const char* file, Array items)
{
	if (_tc_writeModel)
		return _tc_writeModel(file,items);
	return 0;
}

/*! 
 \brief initialize model generator functions
 \ingroup init
*/
void tc_ModelFileGenerator_api(		
	int (*modelgen)(const char*, Array ),
	Matrix (*getModelParameters)(Array)
)
{
	_tc_writeModel = modelgen;
	_tc_getModelParameters = getModelParameters;
}

#endif
