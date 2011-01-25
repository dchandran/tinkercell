#include "TC_ModelFileGenerator_api.h"

int (*_tc_writeModel)(const char* file, tc_items items) = 0;
/*! 
 \brief write the ODE, stoichiometry, and rates functions to a file
 \ingroup Modeling
*/ TCAPIEXPORT 
int tc_writeModel(const char* file, tc_items items)
{
	if (_tc_writeModel)
		return _tc_writeModel(file,items);
	return 0;
}

/*! 
 \brief initialize model generator functions
 \ingroup init
*/ TCAPIEXPORT 
void tc_ModelFileGenerator_api(		
	int (*modelgen)(const char*, tc_items )
)
{
	_tc_writeModel = modelgen;
}

