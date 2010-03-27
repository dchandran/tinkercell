#include "TC_NameFamily_api.h"

ArrayOfStrings (*_tc_getAnnotation)(void*) = 0;
/*! 
 \brief get annotation for this item, i.e. family, author, descriptions, etc.
 \ingroup Annotation
*/
ArrayOfStrings tc_getAnnotation(void* o)
{
	if (_tc_getAnnotation)
		return _tc_getAnnotation(o);
	return newArrayOfStrings(0);
}

void (*_tc_setAnnotation)(void*,ArrayOfStrings) = 0;
/*! 
 \brief set annotation for this item, i.e. family, author, descriptions, etc.
 \ingroup Annotation
*/
void tc_setAnnotation(void* o,ArrayOfStrings annot)
{
	if (_tc_setAnnotation)
		_tc_setAnnotation(o,annot);
}

/*! 
 \brief initialize main
 \ingroup init
*/
void tc_NameFamily_api_initialize(
	   ArrayOfStrings (*tc_getAnnotation0)(void*),
		void (*tc_setAnnotation0)(void*,ArrayOfStrings)
	)
{
	_tc_getAnnotation = tc_getAnnotation0;
	_tc_setAnnotation = tc_setAnnotation0;
}

