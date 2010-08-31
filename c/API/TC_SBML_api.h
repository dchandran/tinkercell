#ifndef TINKERCELL_TC_SBML_API_H
#define TINKERCELL_TC_SBML_API_H

#include "../TCstructs.h"

/*!
 \brief save sbml format to a file
 \param char* file name
 \ingroup Export
*/
void tc_exportSBML(String file);

/*!
 \brief load sbml model as string
 \param char* sbml model file or string
 \ingroup Export
*/
void tc_importSBML(String file);

/*!
 \brief simulate the current model
 \param double total time for simulation
 \param double time increment (step size)
 \ingroup Simulation
*/
TableOfReals tc_simulateODE(double time, double step_size);

/*!
 \brief load sbml model as string
 \param char* sbml model file or string
 \ingroup Simulation
*/
TableOfReals tc_simulateSSA(double time);

/*!
 \brief initializing function
 \ingroup init
*/
void tc_SBML_api(
	void (*exportSBML)(String),
	void (*importSBML)(String),
	TableOfReals (*simulateODE)(double, double),
	TableOfReals (*simulateSSA)(double));
#endif

