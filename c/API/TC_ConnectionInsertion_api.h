#ifndef TINKERCELL_TC_CONNECTIONINSERTION_API_H
#define TINKERCELL_TC_CONNECTIONINSERTION_API_H

#include "../TCstructs.h"
BEGIN_C_DECLS

/*! 
 \brief connect a set of parts (in) to another (out). give the connection name and family. returns the inserted connection
 \ingroup Connections
*/
TCAPIEXPORT int tc_insertConnection(ArrayOfItems parts, const char* name, const char* family);

/*! 
 \brief get the connected parts for a connection
 \ingroup Connections
*/
TCAPIEXPORT ArrayOfItems tc_getConnectedNodes(int connection);
/*! 
 \brief get the parts with a specific role in the given connection, such as "Reactant"
 \ingroup Connections
*/
TCAPIEXPORT ArrayOfItems tc_getConnectedNodesWithRole(int connection, const char* role);
/*! 
 \brief get connections for a part
 \ingroup Connections
*/
TCAPIEXPORT ArrayOfItems tc_getConnections(int part);
/*! 
 \brief get connections where the given parts has a specific role, such as "Reactant"
 \ingroup Connections
*/
TCAPIEXPORT ArrayOfItems tc_getConnectionsWithRole(int part, const char* role);

/*! 
 \brief initialize connections
 \ingroup init
*/
TCAPIEXPORT void tc_ConnectionInsertion_api(
		int (*insertConnection)(ArrayOfItems, const char*, const char*),
		ArrayOfItems (*getConnectedParts)(int),
		ArrayOfItems (*getConnectedPartsWithRole)(int,const char*),
		ArrayOfItems (*getConnections)(int),
		ArrayOfItems (*getConnectionsWithRole)(int,const char*)
	);

END_C_DECLS
#endif

