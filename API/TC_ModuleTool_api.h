#ifndef TINKERCELL_TC_MODULETOOL_API_H
#define TINKERCELL_TC_MODULETOOL_API_H

#include "TC_structs.h"
BEGIN_C_DECLS

/*!
 \brief load a sub-model to represent the processes inside an existing connection
 \param long connection that will be the parent of the new model
 \param const char* file name of new model
 \ingroup Module
*/
TCAPIEXPORT void tc_substituteModel(long item, const char* filename);

/*!
 \brief get the list of possible model files that can be used as a sub-model to represent the processes inside an existing connection
 \param long connection that will be the parent of the new model
  \return tc_list list of file names
 \ingroup Module
*/
TCAPIEXPORT tc_strings tc_listOfPossibleModels(long item);

/*!
 \brief initializing function
 \ingroup init
*/
TCAPIEXPORT void tc_ModuleTool_api(
	void (*substituteModel)(long, const char*),
	tc_strings (*listOfModels)(long));

END_C_DECLS
#endif
