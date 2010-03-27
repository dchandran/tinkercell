#ifndef TINKERCELL_TC_STOICHIOMETRYTOOL_API_H
#define TINKERCELL_TC_STOICHIOMETRYTOOL_API_H

#include "../TCstructs.h"

/*! 
\brief get stoichiometry for the given items
\ingroup Stoichiometry
*/
Matrix tc_getStoichiometry(ArrayOfItems A);
/*! 
\brief set stoichiometry for the given items (must be labeled)
\ingroup Stoichiometry
*/
void tc_setStoichiometry(ArrayOfItems A,Matrix N);
/*! 
\brief get rates for the given items
\ingroup Stoichiometry
*/
ArrayOfStrings tc_getRates(ArrayOfItems A);
/*! 
\brief set rates for the given items (same order as N)
\ingroup Stoichiometry
*/
void tc_setRates(ArrayOfItems A,ArrayOfStrings rates);

/*! 
\brief get stoichiometry for the given items
\ingroup init
*/
Matrix tc_getStoichiometryFor(void* x);
/*! 
\brief get rate for the given items
\ingroup init
*/
const char* tc_getRate(void* x);
/*! 
\brief set rate for the given items
\ingroup init
*/
void tc_setRate(void* x, const char* r);
/*! 
\brief set stoichiometry for the given items
\ingroup init
*/
void tc_setStoichiometryFor(void* x, Matrix N);
/*! 
\brief initialize stiochiometry functions
\ingroup init
*/
void tc_StoichiometryTool_api(
							  Matrix (*getStoichiometry)(ArrayOfItems ),
							  void (*setStoichiometry)(ArrayOfItems ,Matrix ),
							  ArrayOfStrings (*getRates)(ArrayOfItems ),
							  void (*setRates)(ArrayOfItems ,ArrayOfStrings )
							  );
#endif
