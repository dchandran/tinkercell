#include "TC_AutoGeneRegulatoryTool_api.h"

ArrayOfItems (*_tc_partsIn)(int) = 0;
/*! 
 \brief Get all DNA parts inside the given container or module
 \ingroup Get items
*/
ArrayOfItems tc_partsIn(int o)
{
	if (_tc_partsIn)
		return _tc_partsIn(o);
	return newArrayOfItems(0);
}

ArrayOfItems (*_tc_partsUpstream)(int) = 0;
/*! 
 \brief Get all DNA parts upstream of the given part
 \ingroup Get items
*/
ArrayOfItems tc_partsUpstream(int o)
{
	if (_tc_partsUpstream)
		return _tc_partsUpstream(o);
	return newArrayOfItems(0);
}

ArrayOfItems (*_tc_partsDownstream)(int) = 0;
/*! 
 \brief Get all DNA parts downstream of the given part
 \ingroup Get items
*/
ArrayOfItems tc_partsDownstream(int o)
{
	if (_tc_partsDownstream)
		return _tc_partsDownstream(o);
	return newArrayOfItems(0);
}

void (*_tc_alignParts)(ArrayOfItems) = 0;
/*! 
 \brief Align the given DNA parts in the order given
 \ingroup Get and set position
*/
void tc_alignParts(ArrayOfItems a)
{
	if (_tc_alignParts)
		_tc_alignParts(a);
}

/*! 
 \brief initialize grouping
 \ingroup init
*/
void tc_AutoGeneRegulatoryTool_api(
		ArrayOfItems (*f1)(int), ArrayOfItems (*f2)(int), ArrayOfItems (*f3)(int), void (*f4)(ArrayOfItems)
	)
{
	_tc_partsIn = f1;
	_tc_partsUpstream = f2;
	_tc_partsDownstream = f3;
	_tc_alignParts = f4;
}

