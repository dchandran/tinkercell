#include "TC_ConnectionInsertion_api.h"

long (*_tc_insertConnection)(ArrayOfItems parts, const char* name, const char* family) = 0;
/*! 
 \brief connect a set of parts (in) to another (out). give the connection name and family. returns the inserted connection
 \ingroup Connections
*/
long tc_insertConnection(ArrayOfItems parts, const char* name, const char* family) 
{
	if (_tc_insertConnection)
		return _tc_insertConnection(parts, name, family);
	return 0;
}

ArrayOfItems (*_tc_getConnectedNodes)(long connection) = 0;
/*! 
 \brief get the connected parts for a connection
 \ingroup Connections
*/
ArrayOfItems tc_getConnectedNodes(long connection)
{
	if (_tc_getConnectedNodes)
		return _tc_getConnectedNodes(connection);
	return newArrayOfItems(0);
}

ArrayOfItems (*_tc_getConnectedNodesWithRole)(long connection, const char* role) = 0;
/*! 
 \brief get the parts with a role in a connection, such as reactants
 \ingroup Connections
*/
ArrayOfItems tc_getConnectedNodesWithRole(long connection, const char* role)
{
	if (_tc_getConnectedNodesWithRole)
		return _tc_getConnectedNodesWithRole(connection,role);
	return newArrayOfItems(0);
}

ArrayOfItems (*_tc_getConnections)(long part) = 0;
/*! 
 \brief get connections for a part
 \ingroup Connections
*/
ArrayOfItems tc_getConnections(long part)
{
	if (_tc_getConnections)
		return _tc_getConnections(part);
	return newArrayOfItems(0);
}

ArrayOfItems (*_tc_getConnectionsWithRole)(long part, const char* role) = 0;
/*! 
 \brief get connections where the given part has the given role, e.g. reactant
 \ingroup Connections
*/
ArrayOfItems tc_getConnectionsWithRole(long part, const char* role)
{
	if (_tc_getConnectionsWithRole)
		return _tc_getConnectionsWithRole(part,role);
	return newArrayOfItems(0);
}

/*! 
 \brief initialize connections
 \ingroup init
*/
void tc_ConnectionInsertion_api(
		long (*insertConnection)(ArrayOfItems, const char*,const char*),
		ArrayOfItems (*getConnectedParts)(long),
		ArrayOfItems (*getConnectedPartsWithRole)(long,const char*),
		ArrayOfItems (*getConnections)(long),
		ArrayOfItems (*getConnectionsWithRole)(long,const char*)
	)
{
	_tc_insertConnection = insertConnection;
	_tc_getConnectedNodes = getConnectedParts;
	_tc_getConnectedNodesWithRole = getConnectedPartsWithRole;
	_tc_getConnections = getConnections;
	_tc_getConnectionsWithRole = getConnectionsWithRole;
}

