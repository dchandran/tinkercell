#ifndef TINKERCELL_TC_SBML_API_H
#define TINKERCELL_TC_SBML_API_H

#include "TC_structs.h"
BEGIN_C_DECLS

/*!
 \brief save sbml format to a file
 \param const char* file name
 \ingroup Import/Export
*/
TCAPIEXPORT void tc_exportSBML(const char* file);

/*!
 \brief load sbml model as string
 \param const char* sbml model file or string
 \ingroup Import/Export
*/
TCAPIEXPORT void tc_importSBML(const char* file);

/*!
 \brief save model as string
 \param const char* text model file or string
 \ingroup Import/Export
*/
TCAPIEXPORT void tc_exportText(const char* file);

/*!
 \brief load model as string
 \param const char* text model file or string
 \ingroup Import/Export
*/
TCAPIEXPORT void tc_importText(const char* file);

/*!
 \brief initializing function
 \ingroup init
*/
TCAPIEXPORT void tc_SBML_api(
	void (*exportSBML)(const char*),
	void (*importSBML)(const char*),
	void (*exportText)(const char*),
	void (*importText)(const char*));

END_C_DECLS
#endif

