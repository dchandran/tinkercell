#include "TC_ConnectionInsertion_api.h"

tc_items (*_tc_getConnectedNodesWithRole)(long connection, const char* role) = 0;
/*! 
 \brief get the parts with a role in a connection, such as reactants
 \ingroup Connections
*/ TCAPIEXPORT 
tc_items tc_getConnectedNodesWithRole(long connection, const char* role)
{
	if (_tc_getConnectedNodesWithRole)
		return _tc_getConnectedNodesWithRole(connection,role);
	return tc_createItemsArray(0);
}

tc_items (*_tc_getConnectionsWithRole)(long part, const char* role) = 0;
/*! 
 \brief get connections where the given part has the given role, e.g. reactant
 \ingroup Connections
*/ TCAPIEXPORT 
tc_items tc_getConnectionsWithRole(long part, const char* role)
{
	if (_tc_getConnectionsWithRole)
		return _tc_getConnectionsWithRole(part,role);
	return tc_createItemsArray(0);
}

/*! 
 \brief initialize connections
 \ingroup init
*/ TCAPIEXPORT 
void tc_ConnectionInsertion_api(
		tc_items (*getConnectedPartsWithRole)(long,const char*),
		tc_items (*getConnectionsWithRole)(long,const char*)
	)
{
	_tc_getConnectedNodesWithRole = getConnectedPartsWithRole;
	_tc_getConnectionsWithRole = getConnectionsWithRole;
}

