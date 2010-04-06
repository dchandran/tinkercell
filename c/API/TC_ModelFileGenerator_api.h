#ifndef TINKERCELL_TC_MODELFILEGENERATOR_API_H
#define TINKERCELL_TC_MODELFILEGENERATOR_API_H

#include "../TCstructs.h"

/*! 
 \brief write the ODE, stoichiometry, and rates functions to a file
 \ingroup Programming
*/
TCAPIEXPORT int tc_writeModel(String file, ArrayOfItems items);

/*! 
 \brief initialize model generator functions
 \ingroup init
*/
TCAPIEXPORT void tc_ModelFileGenerator_api(
	int (*modelgen)(String, ArrayOfItems )
);

#endif
