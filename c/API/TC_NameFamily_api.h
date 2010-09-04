#ifndef TINKERCELL_TC_NAMEANDFAMILY_API_H
#define TINKERCELL_TC_NAMEANDFAMILY_API_H

#include "TCstructs.h"
BEGIN_C_DECLS

/*! 
 \brief get annotation for this item, i.e. family, author, descriptions, etc.
 \ingroup Annotation
*/
TCAPIEXPORT ArrayOfStrings tc_getAnnotation(long o);
/*! 
 \brief set annotation for this item, i.e. family, author, descriptions, etc.
 \ingroup Annotation
*/
TCAPIEXPORT void tc_setAnnotation(long o,ArrayOfStrings annot);

/*! 
 \brief initialize main
 \ingroup init
*/
TCAPIEXPORT void tc_NameFamily_api_initialize(
	   ArrayOfStrings (*tc_getAnnotation0)(long),
		void (*tc_setAnnotation0)(long,ArrayOfStrings)
	);

END_C_DECLS
#endif

