#ifndef EVOLVENETWORK_SIMULATE_SBML_H
#define EVOLVENETWORK_SIMULATE_SBML_H

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include "muParserDef.h"
#include "muParser.h"
#include "muParserInt.h"
#include "sbml/SBMLReader.h"
#include "sbml/SBMLDocument.h"
#include "sbml/Species.h"
#include "sbml/SpeciesReference.h"
#include "sbml/ListOf.h"
#include "sbml/Model.h"
#include "sbml/Rule.h"
//#include <boost/algorithm/string.hpp>
extern "C"
{
	#include "cvodesim.h"
	#include "ssa.h"
}

void sbml_rates_function(double t, double * y, double * rates, void * data);
int sbml_event_function(int i, double t, double * y, void * data);
void sbml_response_function(int i, double * y, void * data);


/*! \brief Simulation class for simulating SBML models (deterministic and stochastic). 
  Supports events. Functions are available for getting the assignment values, rate values, 
  or species values at any point during simulation or after simulation. Species and parameter
  setting functions are also available.
 \ingroup simulation
*/
class SBML_sim
{
public:
	
	/*! \brief make a simulation
	 * \param char* model file or model string
	 * \param bool if first argument is file name, then use true (default).
	 			   if first argument is model string, then use false
	*/
	SBML_sim(std::string sbml_file, bool isFile=true);

	/*! \brief deterministic simulation of the model
	 * \param double end time
	 * \return vector<vector<double> > result of simulation
	*/
	std::vector< std::vector<double> > simulate(double time, double stepSize) const;

	/*! \brief deterministic steady state of the model
	 * \return vector<double> 
	*/
	std::vector<double> steadyState() const;
	
	/*! \brief stochastic simulation of the model
	 * \param double end time
	 * \return vector<vector<double> > result of simulation
	*/
	std::vector< std::vector<double> > ssa(double time) const;

	/*! \brief set initial values
	 * \param vector<double> values
	*/
	void setVariableValues( const std::vector<double> & );
	
	/*! \brief set parameter values
	 * \param vector<double> values
	*/
	void setParameters( const std::vector<double> & );
	
	/*! \brief get variable names
	 * \return vector<string> names
	*/
	std::vector< std::string > getVariableNames() const;

	/*! \brief get parameter names
	 * \return vector<string> names
	*/
	std::vector< std::string > getParameterNames() const;	
	
	/*! \brief get variable values
	 * \return vector<double> 
	*/
	std::vector< double > getVariableValues() const;
	
	/*! \brief get rate values
	 * \return vector<double> 
	*/
	std::vector< double > getRateValues() const;

	/*! \brief get parameter values
	 * \return vector<double> 
	*/
	std::vector< double > getParameterValues() const;
	
private:

	std::vector<std::string> reactionNames;
	std::vector<double> rateValues;
	std::vector<mu::Parser> rateEqns;
	
	std::vector<std::string> assignmentVariables;
	std::vector<double> assignmentValues;
	std::vector<mu::Parser> assignmentEqns;

	std::vector<mu::Parser> triggerEqns;
	std::vector< std::vector<mu::Parser> > responseEqns;
	
	std::vector<std::string> variableNames;	
	std::vector<double> variableValues;
	
	std::vector<std::string> parameterNames;
	std::vector<double> parameterValues;
	
	double * stoichiometryMatrix;
	
	friend void sbml_rates_function(double t, double * y, double * rates, void * data);
	friend int sbml_event_function(int i, double t, double * y, void * data);
	friend void sbml_response_function(int i, double * y, void * data);
};

#endif

