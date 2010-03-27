#include "TC_Antimony_api.h"

void (*_tc_loadSBMLString)(const char *) = 0;
/*!
 \brief load sbml model as string
 \param char* sbml model
 \ingroup Antimony
*/
void tc_loadSBMLString(const char * sbml)
{
	if (_tc_loadSBMLString)
		_tc_loadSBMLString(sbml);
}

void (*_tc_loadAntimonyString)(const char *) = 0;
/*!
 \brief load antimony model as string
 \param char* antimony model
 \ingroup Antimony
*/
void tc_loadAntimonyString(const char * antimony)
{
	if (_tc_loadAntimonyString)
		_tc_loadAntimonyString(antimony);
}

void (*_tc_loadSBMLFile)(const char *) = 0;
/*!
 \brief load sbml model in file
 \param char* file name
 \ingroup Antimony
*/
void tc_loadSBMLFile(const char * sbml)
{
	if (_tc_loadSBMLFile)
		_tc_loadSBMLFile(sbml);
}

void (*_tc_loadAntimonyFile)(const char *) = 0;
/*!
 \brief load antimony model in file
 \param char* file name
 \ingroup Antimony
*/
void tc_loadAntimonyFile(const char * antimony)
{
	if (_tc_loadAntimonyFile)
		_tc_loadAntimonyFile(antimony);
}

const char* (*_tc_getSBMLString)(ArrayOfItems) = 0;
/*!
 \brief get sbml model as a string
 \param ArrayOfItems the subset of items in the model (use tc_allItems() for complete model)
 \return char* sbml model
 \ingroup Antimony
*/
const char* tc_getSBMLString(ArrayOfItems items)
{
	if (_tc_getSBMLString)
		return _tc_getSBMLString(items);
	return 0;
}

const char* (*_tc_getAntimonyString)(ArrayOfItems) = 0;
/*!
 \brief get antimony model as a string
 \param ArrayOfItems the subset of items in the model (use tc_allItems() for complete model)
 \return char* antimony model
 \ingroup Antimony
*/
const char* tc_getAntimonyString(ArrayOfItems items)
{
	if (_tc_getAntimonyString)
		return _tc_getAntimonyString(items);
	return 0;
}

void (*_tc_writeSBMLFile)(ArrayOfItems,const char*) = 0;
/*!
 \brief write sbml model to file
 \param ArrayOfItems the subset of items in the model (use tc_allItems() for complete model)
 \param char* file name
 \ingroup Antimony
*/
void tc_writeSBMLFile(ArrayOfItems items,const char* file)
{
	if (_tc_writeSBMLFile)
		_tc_writeSBMLFile(items,file);
}

void (*_tc_writeAntimonyFile)(ArrayOfItems,const char*) = 0;
/*!
 \brief write antimony model to file
 \param ArrayOfItems the subset of items in the model (use tc_allItems() for complete model)
 \param char* file name
 \ingroup Antimony
*/
void tc_writeAntimonyFile(ArrayOfItems items,const char* file)
{
	if (_tc_writeAntimonyFile)
		_tc_writeAntimonyFile(items,file);
}

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
	void (*writeAntimonyFile)(ArrayOfItems,const char*))
{
	_tc_loadAntimonyString = loadAntimonyString;
	_tc_loadSBMLString = loadSBMLString;
	_tc_loadSBMLFile = loadSBMLFile;
	_tc_loadAntimonyFile = loadAntimonyFile;
	_tc_getSBMLString = getSBMLString;
	_tc_getAntimonyString = getAntimonyString;
	_tc_writeSBMLFile = writeSBMLFile;
	_tc_writeAntimonyFile = writeAntimonyFile;
}

