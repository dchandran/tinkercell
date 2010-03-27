#include "TC_ConnectionInsertion_api.h"

void* (*_tc_insertConnection)(ArrayOfItems partsFrom, ArrayOfItems partsTo, const char* name, const char* family) = 0;
/*! 
 \brief connect a set of parts (in) to another (out). give the connection name and family. returns the inserted connection
 \ingroup Connections
*/
void* tc_insertConnection(ArrayOfItems partsFrom, ArrayOfItems partsTo, const char* name, const char* family) 
{
	if (_tc_insertConnection)
		return _tc_insertConnection(partsFrom, partsTo, name, family);
	return 0;
}

ArrayOfItems (*_tc_getConnectedNodes)(void* connection) = 0;
/*! 
 \brief get the connected parts for a connection
 \ingroup Connections
*/
ArrayOfItems tc_getConnectedNodes(void* connection)
{
	if (_tc_getConnectedNodes)
		return _tc_getConnectedNodes(connection);
	return newArrayOfItems(0);
}

ArrayOfItems (*_tc_getConnectedNodesIn)(void* connection) = 0;
/*! 
 \brief get the parts going IN to a connection, such as reactants
 \ingroup Connections
*/
ArrayOfItems tc_getConnectedNodesIn(void* connection)
{
	if (_tc_getConnectedNodesIn)
		return _tc_getConnectedNodesIn(connection);
	return newArrayOfItems(0);
}

ArrayOfItems (*_tc_getConnectedNodesOut)(void* connection) = 0;
/*! 
 \brief get the parts going OUT to a connection, such as products
 \ingroup Connections
*/
ArrayOfItems tc_getConnectedNodesOut(void* connection)
{
	if (_tc_getConnectedNodesOut)
		return _tc_getConnectedNodesOut(connection);
	return newArrayOfItems(0);
}

ArrayOfItems (*_tc_getConnectedNodesOther)(void* connection) = 0;
/*! 
 \brief get the parts that are neither IN nor OUT, such as modifiers
 \ingroup Connections
*/
ArrayOfItems tc_getConnectedNodesOther(void* connection)
{
	if (_tc_getConnectedNodesOther)
		return _tc_getConnectedNodesOther(connection);
	return newArrayOfItems(0);
}

ArrayOfItems (*_tc_getConnections)(void* part) = 0;
/*! 
 \brief get connections for a part
 \ingroup Connections
*/
ArrayOfItems tc_getConnections(void* part)
{
	if (_tc_getConnections)
		return _tc_getConnections(part);
	return newArrayOfItems(0);
}

ArrayOfItems (*_tc_getConnectionsIn)(void* part) = 0;
/*! 
 \brief get connections where the given part is an in-node, e.g. reactant
 \ingroup Connections
*/
ArrayOfItems tc_getConnectionsIn(void* part)
{
	if (_tc_getConnectionsIn)
		return _tc_getConnectionsIn(part);
	return newArrayOfItems(0);
}

ArrayOfItems (*_tc_getConnectionsOut)(void* part) = 0;
/*! 
 \brief get connections where the given part is an out-node, e.g. product
 \ingroup Connections
*/
ArrayOfItems tc_getConnectionsOut(void* part)
{
	if (_tc_getConnectionsOut)
		return _tc_getConnectionsOut(part);
	return newArrayOfItems(0);
}

ArrayOfItems (*_tc_getConnectionsOther)(void* part) = 0;
/*! 
 \brief get connections where the given part is not an in-node or out-node, e.g. modifier
 \ingroup Connections
*/
ArrayOfItems tc_getConnectionsOther(void* part)
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
		void* (*insertConnection)(ArrayOfItems, ArrayOfItems, const char*, const char*),
		ArrayOfItems (*getConnectedParts)(void*),
		ArrayOfItems (*getConnectedPartsIn)(void*),
		ArrayOfItems (*getConnectedPartsOut)(void*),
		ArrayOfItems (*getConnectedPartsOther)(void*),
		ArrayOfItems (*getConnections)(void*),
		ArrayOfItems (*getConnectionsIn)(void*),
		ArrayOfItems (*getConnectionsOut)(void*),
		ArrayOfItems (*getConnectionsOther)(void*)
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

