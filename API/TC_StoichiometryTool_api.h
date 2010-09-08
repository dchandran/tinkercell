#ifndef TINKERCELL_TC_ModelingTOOL_API_H
#define TINKERCELL_TC_ModelingTOOL_API_H

#include "TCstructs.h"
BEGIN_C_DECLS

/*! 
\brief get Modeling for the given items
\param tc_items list of items to get stoichiometry matrix from. use tc_allItems() for whole model.
\return tc_matrix stoichiometry matrix with rownames (molecules) and column names (reactions)
\ingroup Modeling
*/
TCAPIEXPORT tc_matrix tc_getStoichiometry(tc_items A);
/*! 
\brief set Modeling for the given items (must be labeled)
\param tc_items list of items to set stoichiometry matrix for. use tc_allItems() for whole model.
\param tc_matrix new stoichiometry matrix with rownames (molecules) and column names (reactions)
\
\ingroup Modeling
*/
TCAPIEXPORT void tc_setStoichiometry(tc_items A,tc_matrix N);
/*! 
\brief get rates for the given items
\param tc_items list of items to get reaction rate equations from. use tc_allItems() for whole model.
\return tc_strings reaction rate equations for given items
\ingroup Modeling
*/
TCAPIEXPORT tc_strings tc_getRates(tc_items A);
/*! 
\brief set rates for the given items (same order as N)
\param tc_items list of items to set reaction rate equations for. use tc_allItems() for whole model.
\return tc_strings reaction rate equations for given items
\ingroup Modeling
*/
TCAPIEXPORT void tc_setRates(tc_items A,tc_strings rates);
/*! 
\brief get Modeling for the given items
\param int address of a connection item
\return tc_matrix stoichiometry matrix for the item
\ingroup Modeling
*/
TCAPIEXPORT tc_matrix tc_getStoichiometryFor(long x);
/*! 
\brief get rate for the given items
\param int address of a connection item
\return tc_matrix reaction rate equations for given item
\ingroup Modeling
*/
TCAPIEXPORT const char* tc_getRate(long x);
/*! 
\brief set rate for the given items
\param int address of a connection item
\param tc_matrix reaction rate equations for given item
\ingroup Modeling
*/
TCAPIEXPORT void tc_setRate(long x, const char* r);
/*! 
\brief set Modeling for the given items
\param int address of a connection item
\param tc_matrix stoichiometry matrix for given item
\ingroup Modeling
*/
TCAPIEXPORT void tc_setStoichiometryFor(long x, tc_matrix N);
/*! 
\brief initialize stiochiometry plug-in
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

