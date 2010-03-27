#include "TC_ModelFileGenerator_api.h"

Matrix (*_tc_getModelParameters)(ArrayOfItems) = 0;
/*! 
 \brief get only the parameters used in the model
 \ingroup Attributes
*/
Matrix tc_getModelParameters(ArrayOfItems A)
{
	if (_tc_getModelParameters)
		return _tc_getModelParameters(A);
	return newMatrix(0,0);
}

int (*_tc_writeModel)(const char* file, ArrayOfItems items) = 0;
/*! 
 \brief write the ODE, stoichiometry, and rates functions to a file
 \ingroup Modeling
*/
int tc_writeModel(const char* file, ArrayOfItems items)
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
	int (*modelgen)(const char*, ArrayOfItems ),
	Matrix (*getModelParameters)(ArrayOfItems)
)
{
	_tc_writeModel = modelgen;
	_tc_getModelParameters = getModelParameters;
}

