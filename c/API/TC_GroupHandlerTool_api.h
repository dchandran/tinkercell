#ifndef TINKERCELL_TC_GROUPHANDLERTOOL_API_H
#define TINKERCELL_TC_GROUPHANDLERTOOL_API_H

#include "../TCstructs.h"

/*! 
 \brief merge an array of items
 \ingroup Merging
*/
void (*tc_merge)(Array parts) = 0;
/*! 
 \brief separate all the graphical items in the handle 
 \ingroup Merging
*/
void (*tc_separate)(OBJ part) = 0;
/*! 
 \brief initialize grouping
 \ingroup init
*/
void tc_GroupHandlerTool_api(
		void (*merge)(Array),
		void (*separate)(OBJ)
	)
{
	tc_merge = merge;
	tc_separate = separate;
}

#endif
