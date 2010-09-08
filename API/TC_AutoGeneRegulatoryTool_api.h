#ifndef TINKERCELL_TC_AUTOGENEREGULATORYTOOL_API_H
#define TINKERCELL_TC_AUTOGENEREGULATORYTOOL_API_H

#include "TCstructs.h"

BEGIN_C_DECLS

/*! 
 \brief Get all DNA parts inside the given container or module
 \ingroup Get items
*/
TCAPIEXPORT tc_items tc_partsIn(long o);
/*! 
 \brief Get all DNA parts upstream of the given part
 \ingroup Get items
*/
TCAPIEXPORT tc_items tc_partsUpstream(long o);
/*! 
 \brief Get all DNA parts downstream of the given part
 \ingroup Get items
*/
TCAPIEXPORT tc_items tc_partsDownstream(long o);
/*! 
 \brief Align the given DNA parts in the order given
 \ingroup Get and set position
*/
TCAPIEXPORT void tc_alignParts(tc_items a);

/*! 
 \brief initialize grouping
 \ingroup init
*/
TCAPIEXPORT void tc_AutoGeneRegulatoryTool_api(
		tc_items (*f1)(long), tc_items (*f2)(long), tc_items (*f3)(long), void (*f4)(tc_items)
	);

END_C_DECLS

#endif
