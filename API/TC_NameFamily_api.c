#include "TC_NameFamily_api.h"

tc_strings (*_tc_getAnnotation)(long) = 0;
/*! 
 \brief get annotation for this item, i.e. family, author, descriptions, etc.
 \ingroup Annotation
*/
tc_strings tc_getAnnotation(long o)
{
	if (_tc_getAnnotation)
		return _tc_getAnnotation(o);
	return tc_createStringsArray(0);
}

void (*_tc_setAnnotation)(long,tc_strings) = 0;
/*! 
 \brief set annotation for this item, i.e. family, author, descriptions, etc.
 \ingroup Annotation
*/
void tc_setAnnotation(long o,tc_strings annot)
{
	if (_tc_setAnnotation)
		_tc_setAnnotation(o,annot);
}

/*! 
 \brief initialize main
 \ingroup init
*/
void tc_NameFamily_api_initialize(
	   tc_strings (*tc_getAnnotation0)(long),
		void (*tc_setAnnotation0)(long,tc_strings)
	)
{
	_tc_getAnnotation = tc_getAnnotation0;
	_tc_setAnnotation = tc_setAnnotation0;
}

