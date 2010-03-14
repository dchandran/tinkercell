#ifndef TINKERCELL_TC_AUTOGENEREGULATORYTOOL_API_H
#define TINKERCELL_TC_AUTOGENEREGULATORYTOOL_API_H

#include "../TCstructs.h"

Array (*_tc_partsIn)(OBJ) = 0;
/*! 
 \brief Get all DNA parts inside the given container or module
 \ingroup Get items
*/
Array tc_partsIn(OBJ o)
{
	if (_tc_partsIn)
		return _tc_partsIn(o);
	return 0;
}

Array (*_tc_partsUpstream)(OBJ) = 0;
/*! 
 \brief Get all DNA parts upstream of the given part
 \ingroup Get items
*/
Array tc_partsUpstream(OBJ o)
{
	if (_tc_partsUpstream)
		return _tc_partsUpstream(o);
	return 0;
}

Array (*_tc_partsDownstream)(OBJ) = 0;
/*! 
 \brief Get all DNA parts downstream of the given part
 \ingroup Get items
*/
Array tc_partsDownstream(OBJ o)
{
	if (_tc_partsDownstream)
		return _tc_partsDownstream(o);
	return 0;
}

void (*_tc_alignParts)(Array) = 0;
/*! 
 \brief Align the given DNA parts in the order given
 \ingroup Get and set position
*/
void tc_alignParts(Array a)
{
	if (_tc_alignParts)
		return _tc_alignParts(a);
}

/*! 
 \brief initialize grouping
 \ingroup init
*/
void tc_AutoGeneRegulatoryTool_api(
		Array (*f1)(OBJ), Array (*f2)(OBJ), Array (*f3)(OBJ), void (*f4)(Array)
	)
{
	_tc_partsIn = f1;
	_tc_partsUpstream = f2;
	_tc_partsDownstream = f3;
	_tc_alignParts = f4;
}

#endif
