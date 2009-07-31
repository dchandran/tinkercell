#ifndef TINKERCELL_TC_NAMEANDFAMILY_API_H
#define TINKERCELL_TC_NAMEANDFAMILY_API_H

#include "../TCstructs.h"
/*! 
 \brief get annotation for this item, i.e. family, author, descriptions, etc.
 \ingroup Annotation
*/
char** (*tc_getAnnotation)(OBJ) = 0;
/*! 
 \brief set annotation for this item, i.e. family, author, descriptions, etc.
 \ingroup Annotation
*/
void (*tc_setAnnotation)(OBJ,char**) = 0;

/*! 
 \brief initialize main
 \ingroup init
*/
void tc_NameFamily_api_initialize(
	   char** (*tc_getAnnotation0)(OBJ),
		void (*tc_setAnnotation0)(OBJ,char**)
	)
{
	tc_getAnnotation = tc_getAnnotation0;
	tc_setAnnotation = tc_setAnnotation0;
}

#endif
