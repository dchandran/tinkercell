#ifndef TINKERCELL_TC_CONNECTIONINSERTION_API_H
#define TINKERCELL_TC_CONNECTIONINSERTION_API_H

#include "../TCstructs.h"

/*! 
 \brief connect a set of parts (in) to another (out). give the connection name and family. returns the inserted connection
 \ingroup Connections
*/
void* tc_insertConnection(ArrayOfItems partsFrom, ArrayOfItems partsTo, const char* name, const char* family);

/*! 
 \brief get the connected parts for a connection
 \ingroup Connections
*/
ArrayOfItems tc_getConnectedNodes(void* connection);
/*! 
 \brief get the parts going IN to a connection, such as reactants
 \ingroup Connections
*/
ArrayOfItems tc_getConnectedNodesIn(void* connection);
/*! 
 \brief get the parts going OUT to a connection, such as products
 \ingroup Connections
*/
ArrayOfItems tc_getConnectedNodesOut(void* connection);
/*! 
 \brief get the parts that are neither IN nor OUT, such as modifiers
 \ingroup Connections
*/
ArrayOfItems tc_getConnectedNodesOther(void* connection);
/*! 
 \brief get connections for a part
 \ingroup Connections
*/
ArrayOfItems tc_getConnections(void* part);
/*! 
 \brief get connections where the given part is an in-node, e.g. reactant
 \ingroup Connections
*/
ArrayOfItems tc_getConnectionsIn(void* part);
/*! 
 \brief get connections where the given part is an out-node, e.g. product
 \ingroup Connections
*/
ArrayOfItems tc_getConnectionsOut(void* part);
/*! 
 \brief get connections where the given part is not an in-node or out-node, e.g. modifier
 \ingroup Connections
*/
ArrayOfItems tc_getConnectionsOther(void* part);

/*! 
 \brief initialize connections
 \ingroup init
*/
void tc_ConnectionInsertion_api(
		void* (*insertConnection)(ArrayOfItems, ArrayOfItems, const char*, const char*),
		ArrayOfItems (*getConnectedParts)(void*),
		ArrayOfItems (*getConnectedPartsIn)(void*),
		ArrayOfItems (*getConnectedPartsOut)(void*),
		ArrayOfItems (*getConnectedPartsOther)(void*),
		ArrayOfItems (*getConnections)(void*),
		ArrayOfItems (*getConnectionsIn)(void*),
		ArrayOfItems (*getConnectionsOut)(void*),
		ArrayOfItems (*getConnectionsOther)(void*)
	);

#endif
