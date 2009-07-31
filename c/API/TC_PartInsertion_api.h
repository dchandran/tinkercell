#ifndef TINKERCELL_TC_PARTINSERTION_API_H
#define TINKERCELL_TC_PARTINSERTION_API_H

#include "../TCstructs.h"
/*! 
 \brief insert an item with the given name and family. returns the inserted connection
 \ingroup Insert and remove
*/
OBJ (*tc_insert)(const char* name, const char* family) = 0;
/*! 
 \brief initializing function
 \ingroup init
*/
void tc_PartInsertion_api(
		OBJ (*insertItem)(const char* , const char* )
)
{
	tc_insert = insertItem;
}

#endif
