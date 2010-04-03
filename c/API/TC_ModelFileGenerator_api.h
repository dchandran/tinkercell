#ifndef TINKERCELL_TC_MODELFILEGENERATOR_API_H
#define TINKERCELL_TC_MODELFILEGENERATOR_API_H

#include "../TCstructs.h"

/*! 
 \brief get only the parameters used in the model
 \ingroup Modeling
*/
Matrix tc_getModelParameters(ArrayOfItems A);
/*! 
 \brief write the ODE, stoichiometry, and rates functions to a file
 \ingroup Programming
*/
int tc_writeModel(String file, ArrayOfItems items);

/*! 
 \brief initialize model generator functions
 \ingroup init
*/
void tc_ModelFileGenerator_api(
	int (*modelgen)(String, ArrayOfItems ),
	Matrix (*getModelParameters)(ArrayOfItems)
);

#endif
