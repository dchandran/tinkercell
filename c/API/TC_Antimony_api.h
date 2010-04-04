#ifndef TINKERCELL_TC_ANTIMONY_API_H
#define TINKERCELL_TC_ANTIMONY_API_H

#include "../TCstructs.h"

/*!
 \brief load sbml model as string
 \param char* sbml model
 \ingroup Antimony
*/
TCAPIEXPORT void tc_loadSBMLString(String sbml);

/*!
 \brief load antimony model as string
 \param char* antimony model
 \ingroup Antimony
*/
TCAPIEXPORT void tc_loadAntimonyString(String antimony);
/*!
 \brief load sbml model in file
 \param char* file name
 \ingroup Antimony
*/
TCAPIEXPORT void tc_loadSBMLFile(String sbml);
/*!
 \brief load antimony model in file
 \param char* file name
 \ingroup Antimony
*/
TCAPIEXPORT void tc_loadAntimonyFile(String antimony);
/*!
 \brief get sbml model as a string
 \param ArrayOfItems the subset of items in the model (use tc_allItems() for complete model)
 \return char* sbml model
 \ingroup Antimony
*/
TCAPIEXPORT String tc_getSBMLString(ArrayOfItems items);
/*!
 \brief get antimony model as a string
 \param ArrayOfItems the subset of items in the model (use tc_allItems() for complete model)
 \return char* antimony model
 \ingroup Antimony
*/
TCAPIEXPORT String tc_getAntimonyString(ArrayOfItems items);
/*!
 \brief write sbml model to file
 \param ArrayOfItems the subset of items in the model (use tc_allItems() for complete model)
 \param char* file name
 \ingroup Antimony
*/
TCAPIEXPORT void tc_writeSBMLFile(ArrayOfItems items,String file);
/*!
 \brief write antimony model to file
 \param ArrayOfItems the subset of items in the model (use tc_allItems() for complete model)
 \param char* file name
 \ingroup Antimony
*/
TCAPIEXPORT void tc_writeAntimonyFile(ArrayOfItems items,String file);

/*!
 \brief initializing function
 \ingroup init
*/
TCAPIEXPORT void tc_Antimony_api(
	void (*loadAntimonyString)(String),
	void (*loadSBMLString)(String),
	void (*loadSBMLFile)(String),
	void (*loadAntimonyFile)(String),
	String (*getSBMLString)(ArrayOfItems),
	String (*getAntimonyString)(ArrayOfItems),
	void (*writeSBMLFile)(ArrayOfItems,String),
	void (*writeAntimonyFile)(ArrayOfItems,String));
#endif
