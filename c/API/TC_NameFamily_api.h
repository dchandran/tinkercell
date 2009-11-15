#ifndef TINKERCELL_TC_NAMEANDFAMILY_API_H
#define TINKERCELL_TC_NAMEANDFAMILY_API_H

#include "../TCstructs.h"

char** (*_tc_getAnnotation)(OBJ) = 0;
/*! 
 \brief get annotation for this item, i.e. family, author, descriptions, etc.
 \ingroup Annotation
*/
char** tc_getAnnotation(OBJ o)
{
	if (_tc_getAnnotation)
		return _tc_getAnnotation(o);
	return 0;
}

void (*_tc_setAnnotation)(OBJ,char**) = 0;
/*! 
 \brief set annotation for this item, i.e. family, author, descriptions, etc.
 \ingroup Annotation
*/
void tc_setAnnotation(OBJ o,char** annot)
{
	if (_tc_setAnnotation)
		_tc_setAnnotation(o,annot);
}

/*! 
 \brief initialize main
 \ingroup init
*/
void tc_NameFamily_api_initialize(
	   char** (*tc_getAnnotation0)(OBJ),
		void (*tc_setAnnotation0)(OBJ,char**)
	)
{
	_tc_getAnnotation = tc_getAnnotation0;
	_tc_setAnnotation = tc_setAnnotation0;
}

#endif
