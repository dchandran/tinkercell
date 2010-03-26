#ifndef TINKERCELL_TC_STOICHIOMETRYTOOL_API_H
#define TINKERCELL_TC_STOICHIOMETRYTOOL_API_H

#include "../TCstructs.h"

Matrix (*_tc_getStoichiometry)(Array ) = 0;
/*! 
\brief get stoichiometry for the given items
\ingroup Stoichiometry
*/
Matrix tc_getStoichiometry(Array A)
{
	Matrix M;
	if (_tc_getStoichiometry)
		return _tc_getStoichiometry(A);
	M.rows = M.cols = 0;
	M.colnames = M.rownames = 0;
	M.values = 0;
	return M;
}

void (*_tc_setStoichiometry)(Array ,Matrix N) = 0;
/*! 
\brief set stoichiometry for the given items (must be labeled)
\ingroup Stoichiometry
*/
void tc_setStoichiometry(Array A,Matrix N)
{
	if (_tc_setStoichiometry)
		_tc_setStoichiometry(A,N);
}

ArrayOfStrings (*_tc_getRates)(Array A) = 0;
/*! 
\brief get rates for the given items
\ingroup Stoichiometry
*/
ArrayOfStrings tc_getRates(Array A)
{
	if (_tc_getRates)
		return _tc_getRates(A);
	return 0;
}

void (*_tc_setRates)(Array ,ArrayOfStrings rates) = 0;
/*! 
\brief set rates for the given items (same order as N)
\ingroup Stoichiometry
*/
void tc_setRates(Array A,ArrayOfStrings rates)
{
	if (_tc_setRates)
		_tc_setRates(A,rates);
}

/*! 
\brief get stoichiometry for the given items
\ingroup init
*/
Matrix tc_getStoichiometryFor(OBJ x)
{
	OBJ a[] = { x, 0 };
	return _tc_getStoichiometry(a);
}
/*! 
\brief get rate for the given items
\ingroup init
*/
char* tc_getRate(OBJ x)
{
	OBJ a[] = { x, 0 };
	return _tc_getRates(a)[0];
}
/*! 
\brief set rate for the given items
\ingroup init
*/
void tc_setRate(OBJ x, char* r)
{
	OBJ a[] = { 0, 0 };
	ArrayOfStrings c;

	if (!x) return;

	a[0] = x;
	c = _tc_getRates(a);
	if (!c.strings || c.length < 1 || !c.strings[0]) return;
	free(c.strings[0]);
	c.strings[0] = r;
	_tc_setRates(a,c);
	free(c);
}
/*! 
\brief set stoichiometry for the given items
\ingroup init
*/
void tc_setStoichiometryFor(OBJ x, Matrix N)
{
	OBJ a[] = { x, 0 };
	_tc_setStoichiometry(a,N);
}
/*! 
\brief initialize stiochiometry functions
\ingroup init
*/
void tc_StoichiometryTool_api(
							  Matrix (*getStoichiometry)(Array ),
							  void (*setStoichiometry)(Array ,Matrix ),
							  ArrayOfStrings (*getRates)(Array ),
							  void (*setRates)(Array ,ArrayOfStrings )
							  )
{
	_tc_getStoichiometry = getStoichiometry;
	_tc_setStoichiometry = setStoichiometry;
	_tc_getRates = getRates;
	_tc_setRates = setRates;
}

#endif
