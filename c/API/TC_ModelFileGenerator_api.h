#ifndef TINKERCELL_TC_MODELFILEGENERATOR_API_H
#define TINKERCELL_TC_MODELFILEGENERATOR_API_H

#include "../TCstructs.h"
BEGIN_C_DECLS

/*! 
 \brief write the ODE, stoichiometry, and rates functions to a file
 \ingroup Programming
*/
TCAPIEXPORT int tc_writeModel(const char* file, ArrayOfItems items);

/*! 
 \brief initialize model generator functions
 \ingroup init
*/
TCAPIEXPORT void tc_ModelFileGenerator_api(
	int (*modelgen)(const char*, ArrayOfItems )
);

END_C_DECLS
#endif

