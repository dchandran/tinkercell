#ifndef TINKERCELL_TC_NAMEANDFAMILY_API_H
#define TINKERCELL_TC_NAMEANDFAMILY_API_H

#include "../TCstructs.h"

/*! 
 \brief get annotation for this item, i.e. family, author, descriptions, etc.
 \ingroup Annotation
*/
ArrayOfStrings tc_getAnnotation(void* o);
/*! 
 \brief set annotation for this item, i.e. family, author, descriptions, etc.
 \ingroup Annotation
*/
void tc_setAnnotation(void* o,ArrayOfStrings annot);

/*! 
 \brief initialize main
 \ingroup init
*/
void tc_NameFamily_api_initialize(
	   ArrayOfStrings (*tc_getAnnotation0)(void*),
		void (*tc_setAnnotation0)(void*,ArrayOfStrings)
	);

#endif
