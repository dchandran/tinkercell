#include "TC_GroupHandlerTool_api.h"

void (*_tc_merge)(tc_items parts) = 0;
/*! 
 \brief merge an array of items
 \ingroup Merging
*/
void tc_merge(tc_items parts)
{
	if (_tc_merge)
		_tc_merge(parts);
}

void (*_tc_separate)(long part) = 0;
/*! 
 \brief separate all the graphical items in the handle 
 \ingroup Merging
*/
void tc_separate(long part)
{
	if (_tc_separate)
		_tc_separate(part);
}
/*! 
 \brief initialize grouping
 \ingroup init
*/
void tc_GroupHandlerTool_api(
		void (*merge)(tc_items),
		void (*separate)(long)
	)
{
	_tc_merge = merge;
	_tc_separate = separate;
}

