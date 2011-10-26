#ifndef TINKERCELL_TC_CONNECTIONINSERTION_API_H
#define TINKERCELL_TC_CONNECTIONINSERTION_API_H

#include "TC_structs.h"
BEGIN_C_DECLS

/*! 
 \brief get the parts with a specific role in the given connection, such as reactant
 \param int address of a connection, e.g. obtained using tc_find 
 \param string a role, e.g. Reactant
 \return tc_items all nodes in the given connection with the given role
 \ingroup Connections
*/
TCAPIEXPORT tc_items tc_getConnectedNodesWithRole(long connection, const char* role);

/*! 
 \brief get connections where the given parts has a specific role, such as reactant
 \param int address of a node, e.g. obtained using tc_find 
 \param string a role, such as reactant
 \return tc_items connections linked to the given node with the given role
 \ingroup Connections
*/
TCAPIEXPORT tc_items tc_getConnectionsWithRole(long part, const char* role);

/*! 
 \brief initialize connections insertions plug-in
 \ingroup init
*/
TCAPIEXPORT void tc_ConnectionInsertion_api(
		tc_items (*getConnectedPartsWithRole)(long,const char*),
		tc_items (*getConnectionsWithRole)(long,const char*)
	);

END_C_DECLS
#endif

