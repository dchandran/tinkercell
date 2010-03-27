#ifndef TINKERCELL_TC_AUTOGENEREGULATORYTOOL_API_H
#define TINKERCELL_TC_AUTOGENEREGULATORYTOOL_API_H

#include "../TCstructs.h"

/*! 
 \brief Get all DNA parts inside the given container or module
 \ingroup Get items
*/
ArrayOfItems tc_partsIn(void* o);
/*! 
 \brief Get all DNA parts upstream of the given part
 \ingroup Get items
*/
ArrayOfItems tc_partsUpstream(void* o);
/*! 
 \brief Get all DNA parts downstream of the given part
 \ingroup Get items
*/
ArrayOfItems tc_partsDownstream(void* o);
/*! 
 \brief Align the given DNA parts in the order given
 \ingroup Get and set position
*/
void tc_alignParts(ArrayOfItems a);

/*! 
 \brief initialize grouping
 \ingroup init
*/
void tc_AutoGeneRegulatoryTool_api(
		ArrayOfItems (*f1)(void*), ArrayOfItems (*f2)(void*), ArrayOfItems (*f3)(void*), void (*f4)(ArrayOfItems)
	);

#endif
