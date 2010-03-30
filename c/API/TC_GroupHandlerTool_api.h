#ifndef TINKERCELL_TC_GROUPHANDLERTOOL_API_H
#define TINKERCELL_TC_GROUPHANDLERTOOL_API_H

#include "../TCstructs.h"

/*! 
 \brief merge an array of items
 \ingroup Merging
*/
void tc_merge(ArrayOfItems parts);

/*! 
 \brief separate all the graphical items in the handle 
 \ingroup Merging
*/
void tc_separate(void* part);

/*! 
 \brief initialize grouping
 \ingroup init
*/
void tc_GroupHandlerTool_api(
		void (*merge)(ArrayOfItems),
		void (*separate)(void*)
	);

#endif
