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
void (*_tc_exportText)(const char*) = 0;
/*!
 \brief save text format to a file
 \param const char* file name
 \\ingroup Export/Import
*/ TCAPIEXPORT 
void tc_exportText(const char* s)
{
	if (_tc_exportText)
		_tc_exportText(s);
}

void (*_tc_importText)(const char*) = 0;
/*!
 \brief load text model as string
 \param const char* text model file or string
 \\ingroup Export/Import
*/ TCAPIEXPORT 
void tc_importText(const char* s)
{
	if (_tc_importText)
		_tc_importText(s);
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
	void (*exportMath)(const char*))
{
	_tc_exportSBML = exportSBML;
	_tc_importSBML = importSBML;
	_tc_exportText = exportText;
	_tc_importText = importText;
	_tc_exportMath = exportMath;
}

