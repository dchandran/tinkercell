#ifndef TINKERCELL_TC_MODELFILEGENERATOR_API_H
#define TINKERCELL_TC_MODELFILEGENERATOR_API_H

#include "../TCstructs.h"

/*! 
 \brief get only the parameters used in the model
 \ingroup Attributes
*/
Matrix (*tc_getModelParameters)(Array) = 0;
/*! 
 \brief write the ODE, stoichiometry, and rates functions to a file
 \ingroup Modeling
*/
int (*tc_writeModel)(const char* file, Array items) = 0;
/*! 
 \brief initialize model generator functions
 \ingroup init
*/
void tc_ModelFileGenerator_api(		
	int (*modelgen)(const char*, Array ),
	Matrix (*getModelParameters)(Array)
)
{
	tc_writeModel = modelgen;
	tc_getModelParameters = getModelParameters;
}

#endif
