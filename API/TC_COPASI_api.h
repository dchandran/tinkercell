#ifndef TINKERCELL_TC_COPASI_API_H
#define TINKERCELL_TC_COPASI_API_H

#include "TC_structs.h"
BEGIN_C_DECLS


/*! 
 \brief simulate using LSODA numerical integrator
  \param double start time
 \param double end time
 \param int number of steps in the output
 \return tc_matrix matrix of concentration or particles
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix tc_simulateDeterministic(double startTime, double endTime, int numSteps);
/*! 
 \brief simulate using exact stochastic algorithm
  \param double start time
 \param double end time
 \param int number of steps in the output
 \return tc_matrix matrix of concentration or particles
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix tc_simulateStochastic(double startTime, double endTime, int numSteps);
/*! 
 \brief simulate using Hybrid algorithm/deterministic algorithmparam double start time
 \param double end time
 \param int number of steps in the output
 \return tc_matrix matrix of concentration or particles
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix tc_simulateHybrid(double startTime, double endTime, int numSteps);
/*! 
 \brief simulate using Tau Leap stochastic algorithm
 \param double start time
 \param double end time
 \param int number of steps in the output
 \return tc_matrix matrix of concentration or particles
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix tc_simulateTauLeap(double startTime, double endTime, int numSteps);

/*! 
 \brief bring the system to steady state
 \return tc_matrix matrix with 1 row and n columns, where n = number of species
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix tc_getSteadyState();

/*! 
 \brief calculate steady state for each value of a parameter
 \param char * parameter name
  \param double start value
 \param double end value
 \param int number of steps in the output
 \return tc_matrix matrix of concentration or particles
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix tc_steadyStateScan(const char * param, double start, double end, int numSteps);

/*! 
 \brief calculate steady state for each value of two parameters
 \param char * first parameter name
  \param double start value for parameter 1
 \param double end value for parameter 1
  \param int number of steps in parameter 1
  \param char * second parameter name
  \param double start value for parameter 2
 \param double end value for parameter 2
 \param int number of steps in parameter 2
 \return tc_matrix matrix of concentration or particles
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix tc_steadyStateScan2D(const char * param1, double start1, double end1, int numSteps1,const char * param2, double start2, double end2, int numSteps2);

/*! 
 \brief get the Jacobian at the current state
 \return tc_matrix matrix with n rows and n columns, where n = number of species
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix tc_getJacobian();
/*! 
 \brief get the eigenvalues of the Jacobian at the current state
 \return tc_matrix matrix with 1 row and n columns, each containing an eigenvalue
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix tc_getEigenvalues();

/*! 
 \brief unscaled elasticities
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix tc_getUnscaledElasticities();

/*! unscaled concentration control coefficients
 \brief unscaled elasticities
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix tc_getUnscaledConcentrationCC();

/*! 
 \brief unscaled flux control coefficients
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix tc_getUnscaledFluxCC();

/*! 
 \brief scaled elasticities
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix tc_getScaledElasticities();

/*! 
 \brief scaled concentration control coefficients
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix tc_getScaledConcentrationCC();

/*! 
 \brief scaled flux control coefficients
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix tc_getScaledFluxCC();

/*! 
 \brief reduced stoichiometry
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix tc_reducedStoichiometry();

/*! 
 \brief elementary flux modes
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix tc_elementaryFluxModes();

/*! 
 \brief left nullspace of the stoichiometry matrix
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix tc_LMatrix();

/*! 
 \brief right nullspace of the stoichiometry matrix
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix tc_KMatrix();

/*! 
 \brief update the model parameters just for simulation purposes, i.e. not the actual model itself
            this function will be much faster than using tc_setParameters
\param const char * formula to maximize or filename with data (csv or tab-delimited)
 \return tc_matrix a population of parameters
 \ingroup Simulation
*/
TCAPIEXPORT void tc_updateParameters(tc_matrix params);

/*! 
 \brief Maximize the given formula or fit the data is the given filename, depending on whether or not the input is a filename.
            The optimization is done using genetic algorithms, so a distribution of optimal parameters is generated.
            All parameters in the model will be used where the parameter's min and max values are different (i.e. parameter is variable)
\param const char * formula to maximize or filename with data (csv or tab-delimited)
 \return tc_matrix a population of parameters
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix tc_optimize(const char * formulaOrFile);

/*!
 \brief initializing function
 \ingroup init
*/
TCAPIEXPORT void tc_COPASI_api(
tc_matrix (*simulateDeterministic)(double startTime, double endTime, int numSteps),
tc_matrix (*simulateStochastic)(double startTime, double endTime, int numSteps),
tc_matrix (*simulateHybrid)(double startTime, double endTime, int numSteps),
tc_matrix (*simulateTauLeap)(double startTime, double endTime, int numSteps),
tc_matrix (*getSteadyState)(),
tc_matrix (*steadyStateScan)(const char * param, double start, double end, int numSteps),
tc_matrix (*steadyStateScan2D)(const char * param1, double start1, double end1, int numSteps1,const char * param2, double start2, double end2, int numSteps2),
tc_matrix (*getJacobian)(),
tc_matrix (*getEigenvalues)(),
tc_matrix (*getUnscaledElasticities)(),
tc_matrix (*getUnscaledConcentrationCC)(),
tc_matrix (*getUnscaledFluxCC)(),
tc_matrix (*getScaledElasticities)(),
tc_matrix (*getScaledConcentrationCC)(),
tc_matrix (*getScaledFluxCC)(),
tc_matrix (*tc_reducedStoichiometry)(),
tc_matrix (*tc_emf)(),
tc_matrix (*tc_Lmat)(),
tc_matrix (*tc_Kmat)(),
tc_matrix (*gaoptim)(const char *),
void (*update)(tc_matrix)
);

END_C_DECLS
#endif

