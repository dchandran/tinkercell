#ifndef TINKERCELL_TC_ANTIMONY_API_H
#define TINKERCELL_TC_ANTIMONY_API_H

#include "../TCstructs.h"

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
void tc_loadAntimonyString)(const char * antimony)
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

char* (*_tc_getSBMLString)(Array) = 0;
/*!
 \brief get sbml model as a string
 \param Array the subset of items in the model (use tc_allItems() for complete model)
 \return char* sbml model
 \ingroup Antimony
*/
char* tc_getSBMLString(Array items)
{
	if (_tc_getSBMLString)
		return _tc_getSBMLString(items);
	return 0;
}

char* (*_tc_getAntimonyString)(Array) = 0;
/*!
 \brief get antimony model as a string
 \param Array the subset of items in the model (use tc_allItems() for complete model)
 \return char* antimony model
 \ingroup Antimony
*/
char* tc_getAntimonyString(Array items)
{
	if (_tc_getAntimonyString)
		return _tc_getAntimonyString(items);
	return 0;
}

void (*_tc_writeSBMLFile)(Array,const char*) = 0;
/*!
 \brief write sbml model to file
 \param Array the subset of items in the model (use tc_allItems() for complete model)
 \param char* file name
 \ingroup Antimony
*/
void tc_writeSBMLFile(Array items,const char* file)
{
	if (_tc_writeSBMLFile)
		_tc_writeSBMLFile(items,file);
}

void (*_tc_writeAntimonyFile)(Array,const char*) = 0;
/*!
 \brief write antimony model to file
 \param Array the subset of items in the model (use tc_allItems() for complete model)
 \param char* file name
 \ingroup Antimony
*/
void tc_writeAntimonyFile(Array items,const char* file)
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
	void (*loadSBMLFile)(const char *),
	void (*loadAntimonyFile)(const char *),
	char* (*getSBMLString)(Array),
	char* (*getAntimonyString)(Array),
	void (*writeSBMLFile)(Array,const char*),
	void (*writeAntimonyFile)(Array,const char*))
{
	_tc_loadAntimonyString = loadAntimonyString;
	_tc_loadSBMLFile = loadSBMLFile;
	_tc_loadAntimonyFile = loadAntimonyFile;
	_tc_getSBMLString = getSBMLString;
	_tc_getAntimonyString = getAntimonyString;
	_tc_writeSBMLFile = writeSBMLFile;
	_tc_writeAntimonyFile = writeAntimonyFile;
}

#endif
