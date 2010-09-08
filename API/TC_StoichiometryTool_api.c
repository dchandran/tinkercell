#include <stdlib.h>
#include "TC_StoichiometryTool_api.h"

tc_matrix (*_tc_getStoichiometry)(tc_items ) = 0;
/*! 
\brief get stoichiometry for the given items
\ingroup Stoichiometry
*/
tc_matrix tc_getStoichiometry(tc_items A)
{
	if (_tc_getStoichiometry)
		return _tc_getStoichiometry(A);
	return tc_createMatrix(0,0);
}

void (*_tc_setStoichiometry)(tc_items ,tc_matrix N) = 0;
/*! 
\brief set stoichiometry for the given items (must be labeled)
\ingroup Stoichiometry
*/
void tc_setStoichiometry(tc_items A,tc_matrix N)
{
	if (_tc_setStoichiometry)
		_tc_setStoichiometry(A,N);
}

tc_strings (*_tc_getRates)(tc_items A) = 0;
/*! 
\brief get rates for the given items
\ingroup Stoichiometry
*/
tc_strings tc_getRates(tc_items A)
{
	if (_tc_getRates)
		return _tc_getRates(A);
	return tc_createStringsArray(0);
}

void (*_tc_setRates)(tc_items ,tc_strings rates) = 0;
/*! 
\brief set rates for the given items (same order as N)
\ingroup Stoichiometry
*/
void tc_setRates(tc_items A,tc_strings rates)
{
	if (_tc_setRates)
		_tc_setRates(A,rates);
}

/*! 
\brief get stoichiometry for the given items
\ingroup init
*/
tc_matrix tc_getStoichiometryFor(long x)
{
	long a[] = { x };
	tc_items A;
	A.length = 1;
	A.items = a;
	return _tc_getStoichiometry(A);
}
/*! 
\brief get rate for the given items
\ingroup init
*/
const char* tc_getRate(long x)
{
	long a[] = { x };
	tc_items A;
	tc_strings s;
	A.length = 1;
	A.items = a;
	s = tc_getRates(A);
	return tc_getString(s,0);
}
/*! 
\brief set rate for the given items
\ingroup init
*/
void tc_setRate(long x, const char* r)
{
	long a[] = { x };
	tc_strings c;
	tc_items A;

	if (!x) return;

	A.length = 1;
	A.items = a;
	c = _tc_getRates(A);
	if (!c.strings || c.length < 1 || !c.strings[0]) return;
	
	free(c.strings[0]);
	tc_setString(c,0,r);
	_tc_setRates(A,c);

	tc_deleteStringsArray(&c);
}
/*!
\brief set stoichiometry for the given items
\ingroup init
*/
void tc_setStoichiometryFor(long x, tc_matrix N)
{
	long a[] = { x };
	tc_items A;
	A.length = 1;
	A.items = a;
	_tc_setStoichiometry(A,N);
}
/*! 
\brief initialize stiochiometry functions
\ingroup init
*/
void tc_StoichiometryTool_api(
							  tc_matrix (*getStoichiometry)(tc_items ),
							  void (*setStoichiometry)(tc_items ,tc_matrix ),
							  tc_strings (*getRates)(tc_items ),
							  void (*setRates)(tc_items ,tc_strings )
							  )
{
	_tc_getStoichiometry = getStoichiometry;
	_tc_setStoichiometry = setStoichiometry;
	_tc_getRates = getRates;
	_tc_setRates = setRates;
}

