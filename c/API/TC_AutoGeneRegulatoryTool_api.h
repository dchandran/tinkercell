#ifndef TINKERCELL_TC_AUTOGENEREGULATORYTOOL_API_H
#define TINKERCELL_TC_AUTOGENEREGULATORYTOOL_API_H

#include "../TCstructs.h"

/*! 
 \brief Get all DNA parts inside the given container or module
 \ingroup Get items
*/
Array (*tc_partsIn)(OBJ) = 0;
/*! 
 \brief Get all DNA parts upstream of the given part
 \ingroup Get items
*/
Array (*tc_partsUpstream)(OBJ) = 0;
/*! 
 \brief Get all DNA parts downstream of the given part
 \ingroup Get items
*/
Array (*tc_partsDownstream)(OBJ) = 0;
/*! 
 \brief Align the given DNA parts in the order given
 \ingroup Get items
*/
void (*tc_alignParts)(Array) = 0;
/*! 
 \brief initialize grouping
 \ingroup init
*/
void tc_AutoGeneRegulatoryTool_api(
		Array (*f1)(OBJ), Array (*f2)(OBJ), Array (*f3)(OBJ), void (*f4)(Array)
	)
{
	tc_partsIn = f1;
	tc_partsUpstream = f2;
	tc_partsDownstream = f3;
	tc_alignParts = f4;
}

#endif
