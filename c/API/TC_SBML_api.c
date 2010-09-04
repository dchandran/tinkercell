#include "TC_SBML_api.h"

void (*_tc_exportSBML)(const char*) = 0;
/*!
 \brief save sbml format to a file
 \param const char* file name
 \ingroup Export
*/
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
*/
void tc_importSBML(const char* s)
{
	if (_tc_importSBML)
		_tc_importSBML(s);
}

TableOfReals (*_tc_simulateODE)(double, double) = 0;
/*!
 \brief simulate the current model
 \param double total time for simulation
 \param double time increment (step size)
 \ingroup Simulation
*/
TableOfReals tc_simulateODE(double a, double b)
{
	if (_tc_simulateODE)
		return _tc_simulateODE(a,b);
	return (newMatrix(0,0));
}

TableOfReals (*_tc_simulateSSA)(double) = 0;
/*!
 \brief load sbml model as string
 \param double total time for simulation
 \ingroup Simulation
*/
TableOfReals tc_simulateSSA(double t)
{
	if (_tc_simulateSSA)
		return _tc_simulateSSA(t);
	return (newMatrix(0,0));
}

/*!
 \brief initializing function
 \ingroup init
*/
void tc_SBML_api(
	void (*exportSBML)(const char*),
	void (*importSBML)(const char*),
	TableOfReals (*simulateODE)(double, double),
	TableOfReals (*simulateSSA)(double))
{
	_tc_exportSBML = exportSBML;
	_tc_importSBML = importSBML;
	_tc_simulateODE = simulateODE;
	_tc_simulateSSA = simulateSSA;
}

