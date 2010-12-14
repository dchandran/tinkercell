#ifndef TINKERCELL_TC_COPASI_API_H
#define TINKERCELL_TC_COPASI_API_H

#include "TC_structs.h"
#include "copasi/copasi_api.h"
BEGIN_C_DECLS

/*!
 \brief update the COPASI model from current TinkerCell model
 \return copasi_model pointer to an existing copasi model
 \ingroup Export
*/
TCAPIEXPORT void tc_updateCopasiModel(copasi_model);
/*!
 \brief initializing function
 \ingroup init
*/
TCAPIEXPORT void tc_COPASI_api(void (*updateCopasiModel)(copasi_model));

END_C_DECLS
#endif

