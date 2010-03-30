#include "TC_NodeInsertion_api.h"

void* (*_tc_insert)(const char* name, const char* family) = 0;
/*! 
 \brief insert an item with the given name and family. returns the inserted connection
 \ingroup Insert and remove
*/
void* tc_insert(const char* name, const char* family)
{
	if (_tc_insert)
		return _tc_insert(name,family);
	return 0;
}

/*! 
 \brief initializing function
 \ingroup init
*/
void tc_NodeInsertion_api(
		void* (*insertItem)(const char* , const char* )
)
{
	_tc_insert = insertItem;
}

