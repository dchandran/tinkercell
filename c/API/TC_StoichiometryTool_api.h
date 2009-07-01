#ifndef TINKERCELL_TC_STOICHIOMETRYTOOL_API_H
#define TINKERCELL_TC_STOICHIOMETRYTOOL_API_H

#include "../TCstructs.h"

/*! 
 \brief get stoichiometry for the given items
 \ingroup Stoichiometry
*/
Matrix (*tc_getStoichiometry)(Array );
/*! 
 \brief set stoichiometry for the given items (must be labeled)
 \ingroup Stoichiometry
*/
void (*tc_setStoichiometry)(Array ,Matrix N);
/*! 
 \brief get rates for the given items
 \ingroup Stoichiometry
*/
char** (*tc_getRates)(Array );
/*! 
 \brief set rates for the given items (same order as N)
 \ingroup Stoichiometry
*/
void (*tc_setRates)(Array ,char** rates);

/*! 
 \brief get stoichiometry for the given items
 \ingroup Stoichiometry
*/
Matrix (*tc_getStoichiometryFor)(OBJ);
/*! 
 \brief set stoichiometry for the given items (must be labeled)
 \ingroup Stoichiometry
*/
void (*tc_setStoichiometryFor)(OBJ,Matrix N);
/*! 
 \brief get rates for the given items
 \ingroup Stoichiometry
*/
char* (*tc_getRate)(OBJ);
/*! 
 \brief set rates for the given items (same order as N)
 \ingroup Stoichiometry
*/
void (*tc_setRate)(OBJ,char* rate);

/*! 
 \brief get stoichiometry for the given items
 \ingroup init
*/
Matrix TC_GETSTOICHIOMETRY(OBJ x)
{
    OBJ a[] = { x, 0 };
	return tc_getStoichiometry(a);
}
/*! 
 \brief get rate for the given items
 \ingroup init
*/
char* TC_GETRATE(OBJ x)
{
    OBJ a[] = { x, 0 };
	return tc_getRates(a)[0];
}
/*! 
 \brief set rate for the given items
 \ingroup init
*/
void TC_SETRATE(OBJ x, char* r)
{
	if (!x) return;
    OBJ a[] = { x, 0 };
	char ** c = tc_getRates(a);
	if (!c || !c[0]) return;
	free(c[0]);
	c[0] = r;
	tc_setRates(a,c);
	TCFreeChars(c);
}
/*! 
 \brief set stoichiometry for the given items
 \ingroup init
*/
void TC_SETSTOICHIOMETRY(OBJ x, Matrix N)
{
    OBJ a[] = { x, 0 };
	tc_setStoichiometry(a,N);
}
/*! 
 \brief initialize stiochiometry functions
 \ingroup init
*/
void tc_StoichiometryTool_api(
		Matrix (*getStoichiometry)(Array ),
		void (*setStoichiometry)(Array ,Matrix ),
		char** (*getRates)(Array ),
		void (*setRates)(Array ,char** )
)
{
	tc_getStoichiometry = getStoichiometry;
	tc_setStoichiometry = setStoichiometry;
	tc_getRates = getRates;
	tc_setRates = setRates;
	
	tc_getStoichiometryFor = &(TC_GETSTOICHIOMETRY);
	tc_setStoichiometryFor = &(TC_SETSTOICHIOMETRY);
	tc_getRate = &(TC_GETRATE);
	tc_setRate = &(TC_SETRATE);
}

#endif
