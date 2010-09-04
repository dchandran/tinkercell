#ifndef TINKERCELL_TC_AUTOGENEREGULATORYTOOL_API_H
#define TINKERCELL_TC_AUTOGENEREGULATORYTOOL_API_H

#include "TCstructs.h"

BEGIN_C_DECLS

/*! 
 \brief Get all DNA parts inside the given container or module
 \ingroup Get items
*/
TCAPIEXPORT ArrayOfItems tc_partsIn(long o);
/*! 
 \brief Get all DNA parts upstream of the given part
 \ingroup Get items
*/
TCAPIEXPORT ArrayOfItems tc_partsUpstream(long o);
/*! 
 \brief Get all DNA parts downstream of the given part
 \ingroup Get items
*/
TCAPIEXPORT ArrayOfItems tc_partsDownstream(long o);
/*! 
 \brief Align the given DNA parts in the order given
 \ingroup Get and set position
*/
TCAPIEXPORT void tc_alignParts(ArrayOfItems a);

/*! 
 \brief initialize grouping
 \ingroup init
*/
TCAPIEXPORT void tc_AutoGeneRegulatoryTool_api(
		ArrayOfItems (*f1)(long), ArrayOfItems (*f2)(long), ArrayOfItems (*f3)(long), void (*f4)(ArrayOfItems)
	);

END_C_DECLS

#endif
