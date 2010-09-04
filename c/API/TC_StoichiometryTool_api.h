#ifndef TINKERCELL_TC_ModelingTOOL_API_H
#define TINKERCELL_TC_ModelingTOOL_API_H

#include "../TCstructs.h"
BEGIN_C_DECLS

/*! 
\brief get Modeling for the given items
\ingroup Modeling
*/
TCAPIEXPORT TableOfReals tc_getStoichiometry(ArrayOfItems A);
/*! 
\brief set Modeling for the given items (must be labeled)
\ingroup Modeling
*/
TCAPIEXPORT void tc_setStoichiometry(ArrayOfItems A,TableOfReals N);
/*! 
\brief get rates for the given items
\ingroup Modeling
*/
TCAPIEXPORT ArrayOfStrings tc_getRates(ArrayOfItems A);
/*! 
\brief set rates for the given items (same order as N)
\ingroup Modeling
*/
TCAPIEXPORT void tc_setRates(ArrayOfItems A,ArrayOfStrings rates);
/*! 
\brief get Modeling for the given items
\ingroup Modeling
*/
TCAPIEXPORT TableOfReals tc_getStoichiometryFor(int x);
/*! 
\brief get rate for the given items
\ingroup Modeling
*/
TCAPIEXPORT const char* tc_getRate(int x);
/*! 
\brief set rate for the given items
\ingroup Modeling
*/
TCAPIEXPORT void tc_setRate(int x, const char* r);
/*! 
\brief set Modeling for the given items
\ingroup Modeling
*/
TCAPIEXPORT void tc_setStoichiometryFor(int x, TableOfReals N);
/*! 
\brief initialize stiochiometry functions
\ingroup Modeling
*/
TCAPIEXPORT void tc_StoichiometryTool_api(
							  TableOfReals (*getStoichiometry)(ArrayOfItems ),
							  void (*setStoichiometry)(ArrayOfItems ,TableOfReals ),
							  ArrayOfStrings (*getRates)(ArrayOfItems ),
							  void (*setRates)(ArrayOfItems ,ArrayOfStrings )
							  );

END_C_DECLS
#endif

