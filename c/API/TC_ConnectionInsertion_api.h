#ifndef TINKERCELL_TC_CONNECTIONINSERTION_API_H
#define TINKERCELL_TC_CONNECTIONINSERTION_API_H

#include "TCstructs.h"
BEGIN_C_DECLS

/*! 
 \brief connect a set of parts (in) to another (out). give the connection name and family. returns the inserted connection
 \ingroup Connections
*/
TCAPIEXPORT long tc_insertConnection(tc_items parts, const char* name, const char* family);

/*! 
 \brief get the connected parts for a connection
 \ingroup Connections
*/
TCAPIEXPORT tc_items tc_getConnectedNodes(long connection);
/*! 
 \brief get the parts with a specific role in the given connection, such as "Reactant"
 \ingroup Connections
*/
TCAPIEXPORT tc_items tc_getConnectedNodesWithRole(long connection, const char* role);
/*! 
 \brief get connections for a part
 \ingroup Connections
*/
TCAPIEXPORT tc_items tc_getConnections(long part);
/*! 
 \brief get connections where the given parts has a specific role, such as "Reactant"
 \ingroup Connections
*/
TCAPIEXPORT tc_items tc_getConnectionsWithRole(long part, const char* role);

/*! 
 \brief initialize connections
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

