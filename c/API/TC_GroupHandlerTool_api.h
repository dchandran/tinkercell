#ifndef TINKERCELL_TC_GROUPHANDLERTOOL_API_H
#define TINKERCELL_TC_GROUPHANDLERTOOL_API_H

#include "../TCstructs.h"
BEGIN_C_DECLS

/*! 
 \brief merge an array of items
 \ingroup Merging
*/
TCAPIEXPORT void tc_merge(ArrayOfItems parts);

/*! 
 \brief separate all the graphical items in the handle 
 \ingroup Merging
*/
TCAPIEXPORT void tc_separate(int part);

/*! 
 \brief initialize grouping
 \ingroup init
*/
TCAPIEXPORT void tc_GroupHandlerTool_api(
		void (*merge)(ArrayOfItems),
		void (*separate)(int)
	);

END_C_DECLS
#endif

