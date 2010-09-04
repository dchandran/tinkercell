#ifndef TINKERCELL_TC_PARTINSERTION_API_H
#define TINKERCELL_TC_PARTINSERTION_API_H

#include "../TCstructs.h"
BEGIN_C_DECLS

/*! 
 \brief insert an item with the given name and family. returns the inserted connection
 \ingroup Insert and remove
*/
TCAPIEXPORT int tc_insert(const char* name, const char* family);

/*! 
 \brief initializing function
 \ingroup init
*/
TCAPIEXPORT void tc_NodeInsertion_api(
		int (*insertItem)(const char* , const char* )
);

END_C_DECLS
#endif

