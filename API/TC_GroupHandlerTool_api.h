#ifndef TINKERCELL_TC_GROUPHANDLERTOOL_API_H
#define TINKERCELL_TC_GROUPHANDLERTOOL_API_H

#include "TCstructs.h"
BEGIN_C_DECLS

/*! 
 \brief merge an array of items
 \param tc_items list of items
 \ingroup Merging
*/
TCAPIEXPORT void tc_merge(tc_items parts);

/*! 
 \brief separate all the graphical items in the handle 
 \param int address of item
 \ingroup Merging
*/
TCAPIEXPORT void tc_separate(long part);

/*! 
 \brief initialize grouping plug-in
 \ingroup init
*/
TCAPIEXPORT void tc_GroupHandlerTool_api(
		void (*merge)(tc_items),
		void (*separate)(long)
	);

END_C_DECLS
#endif

