#ifndef TINKERCELL_TC_COPASI_API_H
#define TINKERCELL_TC_COPASI_API_H

#include "TC_structs.h"
BEGIN_C_DECLS

/*!
 \brief get the COPASI model from current TinkerCell model
 \return copasi_model
 \ingroup Export
*/
TCAPIEXPORT copasi_model tc_CopasiModel();
/*!
 \brief initializing function
 \ingroup init
*/
TCAPIEXPORT void tc_COPASI_api(copasi_model (*getCopasiModel)());

END_C_DECLS
#endif

