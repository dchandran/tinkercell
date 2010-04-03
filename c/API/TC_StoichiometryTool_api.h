#ifndef TINKERCELL_TC_ModelingTOOL_API_H
#define TINKERCELL_TC_ModelingTOOL_API_H

#include "../TCstructs.h"

/*! 
\brief get Modeling for the given items
\ingroup Modeling
*/
Matrix tc_getStoichiometry(ArrayOfItems A);
/*! 
\brief set Modeling for the given items (must be labeled)
\ingroup Modeling
*/
void tc_setStoichiometry(ArrayOfItems A,Matrix N);
/*! 
\brief get rates for the given items
\ingroup Modeling
*/
ArrayOfStrings tc_getRates(ArrayOfItems A);
/*! 
\brief set rates for the given items (same order as N)
\ingroup Modeling
*/
void tc_setRates(ArrayOfItems A,ArrayOfStrings rates);
/*! 
\brief get Modeling for the given items
\ingroup Modeling
*/
Matrix tc_getStoichiometryFor(Item x);
/*! 
\brief get rate for the given items
\ingroup Modeling
*/
String tc_getRate(Item x);
/*! 
\brief set rate for the given items
\ingroup Modeling
*/
void tc_setRate(Item x, String r);
/*! 
\brief set Modeling for the given items
\ingroup Modeling
*/
void tc_setStoichiometryFor(Item x, Matrix N);
/*! 
\brief initialize stiochiometry functions
\ingroup Modeling
*/
void tc_ModelingTool_api(
							  Matrix (*getStoichiometry)(ArrayOfItems ),
							  void (*setStoichiometry)(ArrayOfItems ,Matrix ),
							  ArrayOfStrings (*getRates)(ArrayOfItems ),
							  void (*setRates)(ArrayOfItems ,ArrayOfStrings )
							  );
#endif
