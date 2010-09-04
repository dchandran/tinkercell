#include "TC_ModelFileGenerator_api.h"

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
	int (*modelgen)(const char*, ArrayOfItems )
)
{
	_tc_writeModel = modelgen;
}

