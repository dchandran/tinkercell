#ifndef TINKERCELL_TC_MODELFILEGENERATOR_API_H
#define TINKERCELL_TC_MODELFILEGENERATOR_API_H

#include "TCstructs.h"
BEGIN_C_DECLS

/*! 
 \brief write the ODE, stoichiometry, and rates functions to a file
 \param string output filename
 \param tc_items items to include in the model. use tc_allItems for the whole model
 \ingroup Programming
*/
TCAPIEXPORT int tc_writeModel(const char* file, tc_items items);

/*! 
 \brief initialize model generator plug-in
 \ingroup init
*/
TCAPIEXPORT void tc_ModelFileGenerator_api(
	int (*modelgen)(const char*, tc_items )
);

END_C_DECLS
#endif

