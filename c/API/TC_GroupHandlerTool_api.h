#ifndef TINKERCELL_TC_GROUPHANDLERTOOL_API_H
#define TINKERCELL_TC_GROUPHANDLERTOOL_API_H

#include "../TCstructs.h"

void (*_tc_merge)(ArrayOfItems parts) = 0;
/*! 
 \brief merge an array of items
 \ingroup Merging
*/
void tc_merge(ArrayOfItems parts)
{
	if (_tc_merge)
		_tc_merge(parts);
}

void (*_tc_separate)(void* part) = 0;
/*! 
 \brief separate all the graphical items in the handle 
 \ingroup Merging
*/
void tc_separate(void* part)
{
	if (_tc_separate)
		_tc_separate(part);
}
/*! 
 \brief initialize grouping
 \ingroup init
*/
void tc_GroupHandlerTool_api(
		void (*merge)(ArrayOfItems),
		void (*separate)(void*)
	)
{
	_tc_merge = merge;
	_tc_separate = separate;
}

#endif
