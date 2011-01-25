#include "TC_SBML_api.h"

void (*_tc_exportSBML)(const char*) = 0;
/*!
 \brief save sbml format to a file
 \param const char* file name
 \ingroup Export
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
 \ingroup Export
*/ TCAPIEXPORT 
void tc_importSBML(const char* s)
{
	if (_tc_importSBML)
		_tc_importSBML(s);
}
/*!
 \brief initializing function
 \ingroup init
*/ TCAPIEXPORT 
void tc_SBML_api(
	void (*exportSBML)(const char*),
	void (*importSBML)(const char*))
{
	_tc_exportSBML = exportSBML;
	_tc_importSBML = importSBML;
}

