#ifndef TINKERCELL_TC_ModelingTOOL_API_H
#define TINKERCELL_TC_ModelingTOOL_API_H

#include "TCstructs.h"
BEGIN_C_DECLS

/*! 
\brief get Modeling for the given items
\ingroup Modeling
*/
TCAPIEXPORT tc_matrix tc_getStoichiometry(tc_items A);
/*! 
\brief set Modeling for the given items (must be labeled)
\ingroup Modeling
*/
TCAPIEXPORT void tc_setStoichiometry(tc_items A,tc_matrix N);
/*! 
\brief get rates for the given items
\ingroup Modeling
*/
TCAPIEXPORT tc_strings tc_getRates(tc_items A);
/*! 
\brief set rates for the given items (same order as N)
\ingroup Modeling
*/
TCAPIEXPORT void tc_setRates(tc_items A,tc_strings rates);
/*! 
\brief get Modeling for the given items
\ingroup Modeling
*/
TCAPIEXPORT tc_matrix tc_getStoichiometryFor(long x);
/*! 
\brief get rate for the given items
\ingroup Modeling
*/
TCAPIEXPORT const char* tc_getRate(long x);
/*! 
\brief set rate for the given items
\ingroup Modeling
*/
TCAPIEXPORT void tc_setRate(long x, const char* r);
/*! 
\brief set Modeling for the given items
\ingroup Modeling
*/
TCAPIEXPORT void tc_setStoichiometryFor(long x, tc_matrix N);
/*! 
\brief initialize stiochiometry functions
\ingroup Modeling
*/
TCAPIEXPORT void tc_StoichiometryTool_api(
							  tc_matrix (*getStoichiometry)(tc_items ),
							  void (*setStoichiometry)(tc_items ,tc_matrix ),
							  tc_strings (*getRates)(tc_items ),
							  void (*setRates)(tc_items ,tc_strings )
							  );

END_C_DECLS
#endif

