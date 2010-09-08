#ifndef TINKERCELL_TC_PARTINSERTION_API_H
#define TINKERCELL_TC_PARTINSERTION_API_H

#include "TCstructs.h"
BEGIN_C_DECLS

/*! 
 \brief insert an item with the given name and family. returns the inserted connection
 \param string name of new item
 \param string family name (type) of new item
 \return int address of new item, 0 if insertion failed
 \ingroup Insert and remove
*/
TCAPIEXPORT long tc_insert(const char* name, const char* family);

/*! 
 \brief initialize for node insertion plug-in
 \ingroup init
*/
TCAPIEXPORT void tc_NodeInsertion_api(
		long (*insertItem)(const char* , const char* )
);

END_C_DECLS
#endif

