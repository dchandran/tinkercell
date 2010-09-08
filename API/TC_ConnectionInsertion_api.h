#ifndef TINKERCELL_TC_CONNECTIONINSERTION_API_H
#define TINKERCELL_TC_CONNECTIONINSERTION_API_H

#include "TCstructs.h"
BEGIN_C_DECLS

/*! 
 \brief connect a set of parts. The role of each part is automatically determined by its type. Give the connection name and family. returns the inserted connection
 \param tc_items nodes to be connected
 \param string name of new connection
 \param string type of the new connection, i.e. one of the connection types in the catalog
 \ingroup Connections
*/
TCAPIEXPORT long tc_insertConnection(tc_items parts, const char* name, const char* family);

/*! 
 \brief get the connected parts for a connection
 \param int address of a connection, e.g. obtained using tc_find 
 \return tc_items all nodes connection by the given connection
 \ingroup Connections
*/
TCAPIEXPORT tc_items tc_getConnectedNodes(long connection);
/*! 
 \brief get the parts with a specific role in the given connection, such as reactant
 \param int address of a connection, e.g. obtained using tc_find 
 \param string a role, e.g. Reactant
 \return tc_items all nodes in the given connection with the given role
 \ingroup Connections
*/
TCAPIEXPORT tc_items tc_getConnectedNodesWithRole(long connection, const char* role);
/*! 
 \brief get connections for a part
 \param int address of a node, e.g. obtained using tc_find 
 \return tc_items all connections linked to the given node
 \ingroup Connections
*/
TCAPIEXPORT tc_items tc_getConnections(long part);
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
		long (*insertConnection)(tc_items, const char*, const char*),
		tc_items (*getConnectedParts)(long),
		tc_items (*getConnectedPartsWithRole)(long,const char*),
		tc_items (*getConnections)(long),
		tc_items (*getConnectionsWithRole)(long,const char*)
	);

END_C_DECLS
#endif

