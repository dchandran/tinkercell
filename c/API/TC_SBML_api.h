#ifndef TINKERCELL_TC_SBML_API_H
#define TINKERCELL_TC_SBML_API_H

#include "TCstructs.h"
BEGIN_C_DECLS

/*!
 \brief save sbml format to a file
 \param const char* file name
 \ingroup Export
*/
TCAPIEXPORT void tc_exportSBML(const char* file);

/*!
 \brief load sbml model as string
 \param const char* sbml model file or string
 \ingroup Export
*/
TCAPIEXPORT void tc_importSBML(const char* file);

/*!
 \brief simulate the current model
 \param double total time for simulation
 \param double time increment (step size)
 \ingroup Simulation
*/
TCAPIEXPORT TableOfReals tc_simulateODE(double time, double step_size);

/*!
 \brief load sbml model as string
 \param const char* sbml model file or string
 \ingroup Simulation
*/
TCAPIEXPORT TableOfReals tc_simulateSSA(double time);

/*!
 \brief initializing function
 \ingroup init
*/
TCAPIEXPORT void tc_SBML_api(
	void (*exportSBML)(const char*),
	void (*importSBML)(const char*),
	TableOfReals (*simulateODE)(double, double),
	TableOfReals (*simulateSSA)(double));

END_C_DECLS
#endif

