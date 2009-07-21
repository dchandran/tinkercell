#ifndef TINKERCELL_TC_AUTOGENEREGULATORYTOOL_API_H
#define TINKERCELL_TC_AUTOGENEREGULATORYTOOL_API_H

#include "../TCstructs.h"

/*! 
 \brief Get all DNA parts inside the given container or module
 \ingroup Get items
*/
Array (*tc_partsIn)(OBJ);
/*! 
 \brief Get all DNA parts upstream of the given part
 \ingroup Get items
*/
Array (*tc_partsUpstream)(OBJ);
/*! 
 \brief Get all DNA parts downstream of the given part
 \ingroup Get items
*/
Array (*tc_partsDownstream)(OBJ);
/*! 
 \brief initialize grouping
 \ingroup init
*/
void tc_AutoGeneRegulatoryTool_api(
		Array (*f1)(OBJ), Array (*f2)(OBJ), Array (*f3)(OBJ)
	)
{
	tc_partsIn = f1;
	tc_partsUpstream = f2;
	tc_partsDownstream = f3;
}

#endif
