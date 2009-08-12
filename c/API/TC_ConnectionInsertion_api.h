#ifndef TINKERCELL_TC_CONNECTIONINSERTION_API_H
#define TINKERCELL_TC_CONNECTIONINSERTION_API_H

#include "../TCstructs.h"

/*! 
 \brief connect a set of parts (in) to another (out). give the connection name and family. returns the inserted connection
 \ingroup Connections
*/
OBJ (*tc_insertConnection)(Array partsFrom, Array partsTo, const char* name, const char* family) = 0;
/*! 
 \brief get the connected parts for a connection
 \ingroup Connections
*/
Array (*tc_getConnectedNodes)(OBJ connection) = 0;
/*! 
 \brief get the parts going IN to a connection, such as reactants
 \ingroup Connections
*/
Array (*tc_getConnectedNodesIn)(OBJ connection) = 0;
/*! 
 \brief get the parts going OUT to a connection, such as products
 \ingroup Connections
*/
Array (*tc_getConnectedNodesOut)(OBJ connection) = 0;
/*! 
 \brief get the parts that are neither IN nor OUT, such as modifiers
 \ingroup Connections
*/
Array (*tc_getConnectedNodesOther)(OBJ connection) = 0;
/*! 
 \brief get connections for a part
 \ingroup Connections
*/
Array (*tc_getConnections)(OBJ part) = 0;
/*! 
 \brief get connections where the given part is an in-node, e.g. reactant
 \ingroup Connections
*/
Array (*tc_getConnectionsIn)(OBJ part) = 0;
/*! 
 \brief get connections where the given part is an out-node, e.g. product
 \ingroup Connections
*/
Array (*tc_getConnectionsOut)(OBJ part) = 0;
/*! 
 \brief get connections where the given part is not an in-node or out-node, e.g. modifier
 \ingroup Connections
*/
Array (*tc_getConnectionsOther)(OBJ part) = 0;
/*! 
 \brief initialize connections
 \ingroup init
*/
void tc_ConnectionInsertion_api(
		OBJ (*insertConnection)(Array, Array, const char*, const char*),
		Array (*getConnectedParts)(OBJ),
		Array (*getConnectedPartsIn)(OBJ),
		Array (*getConnectedPartsOut)(OBJ),
		Array (*getConnectedPartsOther)(OBJ),
		Array (*getConnections)(OBJ),
		Array (*getConnectionsIn)(OBJ),
		Array (*getConnectionsOut)(OBJ),
		Array (*getConnectionsOther)(OBJ)
	)
{
	tc_insertConnection = insertConnection;
	tc_getConnectedNodes = getConnectedParts;
	tc_getConnectedNodesIn = getConnectedPartsIn;
	tc_getConnectedNodesOut = getConnectedPartsOut;
	tc_getConnectedNodesOther = getConnectedPartsOther;
	tc_getConnections = getConnections;
	tc_getConnectionsIn = getConnectionsIn;
	tc_getConnectionsOut = getConnectionsOut;
	tc_getConnectionsOther = getConnectionsOther;
}

#endif
