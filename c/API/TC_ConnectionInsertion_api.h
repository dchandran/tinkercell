#ifndef TINKERCELL_TC_CONNECTIONINSERTION_API_H
#define TINKERCELL_TC_CONNECTIONINSERTION_API_H

#include "../TCstructs.h"

/*! 
 \brief connect a set of parts (in) to another (out). give the connection name and family. returns the inserted connection
 \ingroup Connections
*/
TCAPIEXPORT Item tc_insertConnection(ArrayOfItems partsFrom, ArrayOfItems partsTo, String name, String family);

/*! 
 \brief get the connected parts for a connection
 \ingroup Connections
*/
TCAPIEXPORT ArrayOfItems tc_getConnectedNodes(Item connection);
/*! 
 \brief get the parts going IN to a connection, such as reactants
 \ingroup Connections
*/
TCAPIEXPORT ArrayOfItems tc_getConnectedNodesIn(Item connection);
/*! 
 \brief get the parts going OUT to a connection, such as products
 \ingroup Connections
*/
TCAPIEXPORT ArrayOfItems tc_getConnectedNodesOut(Item connection);
/*! 
 \brief get the parts that are neither IN nor OUT, such as modifiers
 \ingroup Connections
*/
TCAPIEXPORT ArrayOfItems tc_getConnectedNodesOther(Item connection);
/*! 
 \brief get connections for a part
 \ingroup Connections
*/
TCAPIEXPORT ArrayOfItems tc_getConnections(Item part);
/*! 
 \brief get connections where the given part is an in-node, e.g. reactant
 \ingroup Connections
*/
TCAPIEXPORT ArrayOfItems tc_getConnectionsIn(Item part);
/*! 
 \brief get connections where the given part is an out-node, e.g. product
 \ingroup Connections
*/
TCAPIEXPORT ArrayOfItems tc_getConnectionsOut(Item part);
/*! 
 \brief get connections where the given part is not an in-node or out-node, e.g. modifier
 \ingroup Connections
*/
TCAPIEXPORT ArrayOfItems tc_getConnectionsOther(Item part);

/*! 
 \brief initialize connections
 \ingroup init
*/
TCAPIEXPORT void tc_ConnectionInsertion_api(
		Item (*insertConnection)(ArrayOfItems, ArrayOfItems, String, String),
		ArrayOfItems (*getConnectedParts)(Item),
		ArrayOfItems (*getConnectedPartsIn)(Item),
		ArrayOfItems (*getConnectedPartsOut)(Item),
		ArrayOfItems (*getConnectedPartsOther)(Item),
		ArrayOfItems (*getConnections)(Item),
		ArrayOfItems (*getConnectionsIn)(Item),
		ArrayOfItems (*getConnectionsOut)(Item),
		ArrayOfItems (*getConnectionsOther)(Item)
	);

#endif
