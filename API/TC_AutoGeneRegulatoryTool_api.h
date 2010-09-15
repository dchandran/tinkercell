#ifndef TINKERCELL_TC_AUTOGENEREGULATORYTOOL_API_H
#define TINKERCELL_TC_AUTOGENEREGULATORYTOOL_API_H

#include "TC_structs.h"

BEGIN_C_DECLS

/*! 
 \brief Get all DNA parts inside the given container or module
 \param int address of an item in the network
 \ingroup Get items
*/
TCAPIEXPORT tc_items tc_partsIn(long o);
/*! 
 \brief Get all DNA parts upstream of the given part
 \param int address of an item in the network
 \ingroup Get items
*/
TCAPIEXPORT tc_items tc_partsUpstream(long o);
/*! 
 \brief Get all DNA parts downstream of the given part
 \param int address of an item in the network
 \ingroup Get items
*/
TCAPIEXPORT tc_items tc_partsDownstream(long o);
/*! 
 \brief Align the given DNA parts in the order given
 \param tc_items a list of items
 \ingroup Get and set position
*/
TCAPIEXPORT void tc_alignParts(tc_items a);

/*! 
 \brief initialize auto-gene regulatory plugin C API
 \ingroup init
*/
TCAPIEXPORT void tc_AutoGeneRegulatoryTool_api(
		tc_items (*f1)(long), tc_items (*f2)(long), tc_items (*f3)(long), void (*f4)(tc_items)
	);

END_C_DECLS

#endif
