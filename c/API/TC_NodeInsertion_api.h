#ifndef TINKERCELL_TC_PARTINSERTION_API_H
#define TINKERCELL_TC_PARTINSERTION_API_H

#include "../TCstructs.h"

/*! 
 \brief insert an item with the given name and family. returns the inserted connection
 \ingroup Insert and remove
*/
Item tc_insert(const char* name, const char* family);

/*! 
 \brief initializing function
 \ingroup init
*/
void tc_NodeInsertion_api(
		Item (*insertItem)(const char* , const char* )
);

#endif
