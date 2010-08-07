#include "TC_ConnectionInsertion_api.h"

Item (*_tc_insertConnection)(ArrayOfItems parts, String name, String family) = 0;
/*! 
 \brief connect a set of parts (in) to another (out). give the connection name and family. returns the inserted connection
 \ingroup Connections
*/
Item tc_insertConnection(ArrayOfItems parts, String name, String family) 
{
	if (_tc_insertConnection)
		return _tc_insertConnection(parts, name, family);
	return 0;
}

ArrayOfItems (*_tc_getConnectedNodes)(Item connection) = 0;
/*! 
 \brief get the connected parts for a connection
 \ingroup Connections
*/
ArrayOfItems tc_getConnectedNodes(Item connection)
{
	if (_tc_getConnectedNodes)
		return _tc_getConnectedNodes(connection);
	return newArrayOfItems(0);
}

ArrayOfItems (*_tc_getConnectedNodesIn)(Item connection) = 0;
/*! 
 \brief get the parts going IN to a connection, such as reactants
 \ingroup Connections
*/
ArrayOfItems tc_getConnectedNodesIn(Item connection)
{
	if (_tc_getConnectedNodesIn)
		return _tc_getConnectedNodesIn(connection);
	return newArrayOfItems(0);
}

ArrayOfItems (*_tc_getConnectedNodesOut)(Item connection) = 0;
/*! 
 \brief get the parts going OUT to a connection, such as products
 \ingroup Connections
*/
ArrayOfItems tc_getConnectedNodesOut(Item connection)
{
	if (_tc_getConnectedNodesOut)
		return _tc_getConnectedNodesOut(connection);
	return newArrayOfItems(0);
}

ArrayOfItems (*_tc_getConnectedNodesOther)(Item connection) = 0;
/*! 
 \brief get the parts that are neither IN nor OUT, such as modifiers
 \ingroup Connections
*/
ArrayOfItems tc_getConnectedNodesOther(Item connection)
{
	if (_tc_getConnectedNodesOther)
		return _tc_getConnectedNodesOther(connection);
	return newArrayOfItems(0);
}

ArrayOfItems (*_tc_getConnections)(Item part) = 0;
/*! 
 \brief get connections for a part
 \ingroup Connections
*/
ArrayOfItems tc_getConnections(Item part)
{
	if (_tc_getConnections)
		return _tc_getConnections(part);
	return newArrayOfItems(0);
}

ArrayOfItems (*_tc_getConnectionsIn)(Item part) = 0;
/*! 
 \brief get connections where the given part is an in-node, e.g. reactant
 \ingroup Connections
*/
ArrayOfItems tc_getConnectionsIn(Item part)
{
	if (_tc_getConnectionsIn)
		return _tc_getConnectionsIn(part);
	return newArrayOfItems(0);
}

ArrayOfItems (*_tc_getConnectionsOut)(Item part) = 0;
/*! 
 \brief get connections where the given part is an out-node, e.g. product
 \ingroup Connections
*/
ArrayOfItems tc_getConnectionsOut(Item part)
{
	if (_tc_getConnectionsOut)
		return _tc_getConnectionsOut(part);
	return newArrayOfItems(0);
}

ArrayOfItems (*_tc_getConnectionsOther)(Item part) = 0;
/*! 
 \brief get connections where the given part is not an in-node or out-node, e.g. modifier
 \ingroup Connections
*/
ArrayOfItems tc_getConnectionsOther(Item part)
{
	if (_tc_getConnectionsOther)
		return _tc_getConnectionsOther(part);
	return newArrayOfItems(0);
}

/*! 
 \brief initialize connections
 \ingroup init
*/
void tc_ConnectionInsertion_api(
		Item (*insertConnection)(ArrayOfItems, String, String),
		ArrayOfItems (*getConnectedParts)(Item),
		ArrayOfItems (*getConnectedPartsIn)(Item),
		ArrayOfItems (*getConnectedPartsOut)(Item),
		ArrayOfItems (*getConnectedPartsOther)(Item),
		ArrayOfItems (*getConnections)(Item),
		ArrayOfItems (*getConnectionsIn)(Item),
		ArrayOfItems (*getConnectionsOut)(Item),
		ArrayOfItems (*getConnectionsOther)(Item)
	)
{
	_tc_insertConnection = insertConnection;
	_tc_getConnectedNodes = getConnectedParts;
	_tc_getConnectedNodesIn = getConnectedPartsIn;
	_tc_getConnectedNodesOut = getConnectedPartsOut;
	_tc_getConnectedNodesOther = getConnectedPartsOther;
	_tc_getConnections = getConnections;
	_tc_getConnectionsIn = getConnectionsIn;
	_tc_getConnectionsOut = getConnectionsOut;
	_tc_getConnectionsOther = getConnectionsOther;
}

