#include <stdlib.h>
#include "TC_StoichiometryTool_api.h"

TableOfReals (*_tc_getStoichiometry)(ArrayOfItems ) = 0;
/*! 
\brief get stoichiometry for the given items
\ingroup Stoichiometry
*/
TableOfReals tc_getStoichiometry(ArrayOfItems A)
{
	if (_tc_getStoichiometry)
		return _tc_getStoichiometry(A);
	return newMatrix(0,0);
}

void (*_tc_setStoichiometry)(ArrayOfItems ,TableOfReals N) = 0;
/*! 
\brief set stoichiometry for the given items (must be labeled)
\ingroup Stoichiometry
*/
void tc_setStoichiometry(ArrayOfItems A,TableOfReals N)
{
	if (_tc_setStoichiometry)
		_tc_setStoichiometry(A,N);
}

ArrayOfStrings (*_tc_getRates)(ArrayOfItems A) = 0;
/*! 
\brief get rates for the given items
\ingroup Stoichiometry
*/
ArrayOfStrings tc_getRates(ArrayOfItems A)
{
	if (_tc_getRates)
		return _tc_getRates(A);
	return newArrayOfStrings(0);
}

void (*_tc_setRates)(ArrayOfItems ,ArrayOfStrings rates) = 0;
/*! 
\brief set rates for the given items (same order as N)
\ingroup Stoichiometry
*/
void tc_setRates(ArrayOfItems A,ArrayOfStrings rates)
{
	if (_tc_setRates)
		_tc_setRates(A,rates);
}

/*! 
\brief get stoichiometry for the given items
\ingroup init
*/
TableOfReals tc_getStoichiometryFor(Item x)
{
	Item a[] = { x };
	ArrayOfItems A;
	A.length = 1;
	A.items = a;
	return _tc_getStoichiometry(A);
}
/*! 
\brief get rate for the given items
\ingroup init
*/
String tc_getRate(Item x)
{
	Item a[] = { x };
	ArrayOfItems A;
	ArrayOfStrings s;
	A.length = 1;
	A.items = a;
	s = tc_getRates(A);
	return nthString(s,0);
}
/*! 
\brief set rate for the given items
\ingroup init
*/
void tc_setRate(Item x, String r)
{
	Item a[] = { x };
	ArrayOfStrings c;
	ArrayOfItems A;

	if (!x) return;

	A.length = 1;
	A.items = a;
	c = _tc_getRates(A);
	if (!c.strings || c.length < 1 || !c.strings[0]) return;
	
	free(c.strings[0]);
	setNthString(c,0,r);
	_tc_setRates(A,c);

	deleteArrayOfStrings(&c);
}
/*!
\brief set stoichiometry for the given items
\ingroup init
*/
void tc_setStoichiometryFor(Item x, TableOfReals N)
{
	Item a[] = { x };
	ArrayOfItems A;
	A.length = 1;
	A.items = a;
	_tc_setStoichiometry(A,N);
}
/*! 
\brief initialize stiochiometry functions
\ingroup init
*/
void tc_StoichiometryTool_api(
							  TableOfReals (*getStoichiometry)(ArrayOfItems ),
							  void (*setStoichiometry)(ArrayOfItems ,TableOfReals ),
							  ArrayOfStrings (*getRates)(ArrayOfItems ),
							  void (*setRates)(ArrayOfItems ,ArrayOfStrings )
							  )
{
	_tc_getStoichiometry = getStoichiometry;
	_tc_setStoichiometry = setStoichiometry;
	_tc_getRates = getRates;
	_tc_setRates = setRates;
}

