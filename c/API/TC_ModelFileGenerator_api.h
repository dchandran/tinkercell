#ifndef TINKERCELL_TC_MODELFILEGENERATOR_API_H
#define TINKERCELL_TC_MODELFILEGENERATOR_API_H

#include "../TCstructs.h"

/*! 
 \brief get only the parameters used in the model
 \ingroup Attributes
*/
Matrix tc_getModelParameters(ArrayOfItems A);
/*! 
 \brief write the ODE, stoichiometry, and rates functions to a file
 \ingroup Modeling
*/
int tc_writeModel(const char* file, ArrayOfItems items);

/*! 
 \brief initialize model generator functions
 \ingroup init
*/
void tc_ModelFileGenerator_api(
	int (*modelgen)(const char*, ArrayOfItems ),
	Matrix (*getModelParameters)(ArrayOfItems)
);

#endif
