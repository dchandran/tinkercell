#include "TC_SBML_api.h"

void (*_tc_exportSBML)(const char*) = 0;
/*!
 \brief save sbml format to a file
 \param const char* file name
 \\ingroup Export/Import
*/ TCAPIEXPORT 
void tc_exportSBML(const char* s)
{
	if (_tc_exportSBML)
		_tc_exportSBML(s);
}

const char * (*_tc_getSBMLString)() = 0;
/*!
 \brief save sbml format to a string
 \param const char* file name
 \\ingroup Export/Import
*/ TCAPIEXPORT 
const char * tc_getSBMLString()
{
	if (_tc_getSBMLString)
		return _tc_getSBMLString();
	return 0;
}

const char * (*_tc_getAntimonyString)() = 0;
/*!
 \brief save sbml format to a string
 \param const char* file name
 \\ingroup Export/Import
*/ TCAPIEXPORT 
const char * tc_getAntimonyString()
{
	if (_tc_getAntimonyString)
		return _tc_getAntimonyString();
	return 0;
}

void (*_tc_importSBML)(const char*) = 0;
/*!
 \brief load sbml model as string
 \param const char* sbml model file or string
 \\ingroup Export/Import
*/ TCAPIEXPORT 
void tc_importSBML(const char* s)
{
	if (_tc_importSBML)
		_tc_importSBML(s);
}

void (*_tc_exportAntimony)(const char*) = 0;
/*!
 \brief save antimony format to a file
 \param const char* file name
 \\ingroup Export/Import
*/ TCAPIEXPORT 
void tc_exportAntimony(const char* s)
{
	if (_tc_exportAntimony)
		_tc_exportAntimony(s);
}

void (*_tc_importAntimony)(const char*) = 0;
/*!
 \brief load text model as string
 \param const char* text model file or string
 \\ingroup Export/Import
*/ TCAPIEXPORT 
void tc_importAntimony(const char* s)
{
	if (_tc_importAntimony)
		_tc_importAntimony(s);
}

void (*_tc_exportMath)(const char*) = 0;
/*!
 \brief save math model
 \param const char* filename
 \\ingroup Export/Import
*/ TCAPIEXPORT 
void tc_exportMatlab(const char* s)
{
	if (_tc_exportMath)
		_tc_exportMath(s);
}

/*!
 \brief initializing function
 \ingroup init
*/ TCAPIEXPORT 
void tc_SBML_api(
	void (*exportSBML)(const char*),
	void (*importSBML)(const char*),
	void (*exportText)(const char*),
	void (*importText)(const char*),
	void (*exportMath)(const char*),
	const char * (*getSBMLString)(),
	const char * (*getAntimonyString)())
{
	_tc_exportSBML = exportSBML;
	_tc_importSBML = importSBML;
	_tc_exportAntimony = exportText;
	_tc_importAntimony = importText;
	_tc_exportMath = exportMath;
	_tc_getSBMLString = getSBMLString;
	_tc_getAntimonyString = getAntimonyString;
}

