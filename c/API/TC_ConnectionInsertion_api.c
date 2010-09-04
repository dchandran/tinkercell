#include "TC_ConnectionInsertion_api.h"

int (*_tc_insertConnection)(ArrayOfItems parts, const char* name, const char* family) = 0;
/*! 
 \brief connect a set of parts (in) to another (out). give the connection name and family. returns the inserted connection
 \ingroup Connections
*/
int tc_insertConnection(ArrayOfItems parts, const char* name, const char* family) 
{
	if (_tc_insertConnection)
		return _tc_insertConnection(parts, name, family);
	return 0;
}

ArrayOfItems (*_tc_getConnectedNodes)(int connection) = 0;
/*! 
 \brief get the connected parts for a connection
 \ingroup Connections
*/
ArrayOfItems tc_getConnectedNodes(int connection)
{
	if (_tc_getConnectedNodes)
		return _tc_getConnectedNodes(connection);
	return newArrayOfItems(0);
}

ArrayOfItems (*_tc_getConnectedNodesWithRole)(int connection, const char* role) = 0;
/*! 
 \brief get the parts with a role in a connection, such as reactants
 \ingroup Connections
*/
ArrayOfItems tc_getConnectedNodesWithRole(int connection, const char* role)
{
	if (_tc_getConnectedNodesWithRole)
		return _tc_getConnectedNodesWithRole(connection,role);
	return newArrayOfItems(0);
}

ArrayOfItems (*_tc_getConnections)(int part) = 0;
/*! 
 \brief get connections for a part
 \ingroup Connections
*/
ArrayOfItems tc_getConnections(int part)
{
	if (_tc_getConnections)
		return _tc_getConnections(part);
	return newArrayOfItems(0);
}

ArrayOfItems (*_tc_getConnectionsWithRole)(int part, const char* role) = 0;
/*! 
 \brief get connections where the given part has the given role, e.g. reactant
 \ingroup Connections
*/
ArrayOfItems tc_getConnectionsWithRole(int part, const char* role)
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
		int (*insertConnection)(ArrayOfItems, const char*,const char*),
		ArrayOfItems (*getConnectedParts)(int),
		ArrayOfItems (*getConnectedPartsWithRole)(int,const char*),
		ArrayOfItems (*getConnections)(int),
		ArrayOfItems (*getConnectionsWithRole)(int,const char*)
	)
{
	_tc_insertConnection = insertConnection;
	_tc_getConnectedNodes = getConnectedParts;
	_tc_getConnectedNodesWithRole = getConnectedPartsWithRole;
	_tc_getConnections = getConnections;
	_tc_getConnectionsWithRole = getConnectionsWithRole;
}

