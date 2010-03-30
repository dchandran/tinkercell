#ifndef TINKERCELL_TC_ANTIMONY_API_H
#define TINKERCELL_TC_ANTIMONY_API_H

#include "../TCstructs.h"

/*!
 \brief load sbml model as string
 \param char* sbml model
 \ingroup Antimony
*/
void tc_loadSBMLString(const char * sbml);

/*!
 \brief load antimony model as string
 \param char* antimony model
 \ingroup Antimony
*/
void tc_loadAntimonyString(const char * antimony);
/*!
 \brief load sbml model in file
 \param char* file name
 \ingroup Antimony
*/
void tc_loadSBMLFile(const char * sbml);
/*!
 \brief load antimony model in file
 \param char* file name
 \ingroup Antimony
*/
void tc_loadAntimonyFile(const char * antimony);
/*!
 \brief get sbml model as a string
 \param ArrayOfItems the subset of items in the model (use tc_allItems() for complete model)
 \return char* sbml model
 \ingroup Antimony
*/
const char* tc_getSBMLString(ArrayOfItems items);
/*!
 \brief get antimony model as a string
 \param ArrayOfItems the subset of items in the model (use tc_allItems() for complete model)
 \return char* antimony model
 \ingroup Antimony
*/
const char* tc_getAntimonyString(ArrayOfItems items);
/*!
 \brief write sbml model to file
 \param ArrayOfItems the subset of items in the model (use tc_allItems() for complete model)
 \param char* file name
 \ingroup Antimony
*/
void tc_writeSBMLFile(ArrayOfItems items,const char* file);
/*!
 \brief write antimony model to file
 \param ArrayOfItems the subset of items in the model (use tc_allItems() for complete model)
 \param char* file name
 \ingroup Antimony
*/
void tc_writeAntimonyFile(ArrayOfItems items,const char* file);

/*!
 \brief initializing function
 \ingroup init
*/
void tc_Antimony_api(
	void (*loadAntimonyString)(const char *),
	void (*loadSBMLString)(const char *),
	void (*loadSBMLFile)(const char *),
	void (*loadAntimonyFile)(const char *),
	const char* (*getSBMLString)(ArrayOfItems),
	const char* (*getAntimonyString)(ArrayOfItems),
	void (*writeSBMLFile)(ArrayOfItems,const char*),
	void (*writeAntimonyFile)(ArrayOfItems,const char*));
#endif
