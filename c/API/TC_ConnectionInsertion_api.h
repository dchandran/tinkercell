#ifndef TINKERCELL_TC_CONNECTIONINSERTION_API_H
#define TINKERCELL_TC_CONNECTIONINSERTION_API_H

#include "../TCstructs.h"

OBJ (*_tc_insertConnection)(Array partsFrom, Array partsTo, const char* name, const char* family) = 0;
/*! 
 \brief connect a set of parts (in) to another (out). give the connection name and family. returns the inserted connection
 \ingroup Connections
*/
OBJ tc_insertConnection(Array partsFrom, Array partsTo, const char* name, const char* family) 
{
	if (_tc_insertConnection)
		return _tc_insertConnection(partsFrom, partsTo, name, family);
	return 0;
}

Array (*_tc_getConnectedNodes)(OBJ connection) = 0;
/*! 
 \brief get the connected parts for a connection
 \ingroup Connections
*/
Array tc_getConnectedNodes(OBJ connection)
{
	if (_tc_getConnectedNodes)
		return _tc_getConnectedNodes(connection);
	return 0;
}

Array (*_tc_getConnectedNodesIn)(OBJ connection) = 0;
/*! 
 \brief get the parts going IN to a connection, such as reactants
 \ingroup Connections
*/
Array tc_getConnectedNodesIn(OBJ connection)
{
	if (_tc_getConnectedNodesIn)
		return _tc_getConnectedNodesIn(connection);
	return 0;
}

Array (*_tc_getConnectedNodesOut)(OBJ connection) = 0;
/*! 
 \brief get the parts going OUT to a connection, such as products
 \ingroup Connections
*/
Array tc_getConnectedNodesOut(OBJ connection)
{
	if (_tc_getConnectedNodesOut)
		return _tc_getConnectedNodesOut(connection);
	return 0;
}

Array (*_tc_getConnectedNodesOther)(OBJ connection) = 0;
/*! 
 \brief get the parts that are neither IN nor OUT, such as modifiers
 \ingroup Connections
*/
Array tc_getConnectedNodesOther(OBJ connection)
{
	if (_tc_getConnectedNodesOther)
		return _tc_getConnectedNodesOther(connection);
	return 0;
}

Array (*_tc_getConnections)(OBJ part) = 0;
/*! 
 \brief get connections for a part
 \ingroup Connections
*/
Array tc_getConnections(OBJ part)
{
	if (_tc_getConnections)
		return _tc_getConnections(part);
	return 0;
}

Array (*_tc_getConnectionsIn)(OBJ part) = 0;
/*! 
 \brief get connections where the given part is an in-node, e.g. reactant
 \ingroup Connections
*/
Array tc_getConnectionsIn(OBJ part)
{
	if (_tc_getConnectionsIn)
		return _tc_getConnectionsIn(part);
	return 0;
}

Array (*_tc_getConnectionsOut)(OBJ part) = 0;
/*! 
 \brief get connections where the given part is an out-node, e.g. product
 \ingroup Connections
*/
Array tc_getConnectionsOut(OBJ part)
{
	if (_tc_getConnectionsOut)
		return _tc_getConnectionsOut(part);
	return 0;
}

Array (*_tc_getConnectionsOther)(OBJ part) = 0;
/*! 
 \brief get connections where the given part is not an in-node or out-node, e.g. modifier
 \ingroup Connections
*/
Array tc_getConnectionsOther(OBJ part)
{
	if (_tc_getConnectionsOther)
		return _tc_getConnectionsOther(part);
	return 0;
}

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

#endif
