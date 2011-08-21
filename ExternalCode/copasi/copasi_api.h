#ifndef COPASI_SIMPLE_C_API
#define COPASI_SIMPLE_C_API

/*
This is a C API for the COPASI C++ library. Rate equations in COPASI require the "complete name",   
e.g. instead of X, the rate must specify <model.compartment.X>. In this C API, those complete names
are stored in a hash table. The API replaces the simple strings, i.e. "C", with the complete names by
using the hash-table. This is mainly for speed; otherwise, every cSetReactionRate would be searching
through the entire model for each of its variables. The hash-table idea is used for functions such
as cSetValue, which can set the value of a parameter or that of a molecular species. Again, it uses the
hash table to identify what a variable is. 

The C API hides the C++ classes by casting some of the main classes into void pointers inside
C structs. 

QtCore is used for providing the hash-table feature and regular expression (QHash and QRegExp). This
should be replaced with the boost library at some point. 
*/

#include "TC_structs.h"

/*!\brief this struct is used to contain a pointer to an instance of a COPASI class*/
typedef struct  
{ 
	void * CopasiModelPtr;
	void * CopasiDataModelPtr;
	void * qHash;
	char * errorMessage; 
} copasi_model;

/*!\brief this struct is used to contain a pointer to an instance of a COPASI class*/
typedef struct  
{
	void * CopasiReactionPtr;
	void * CopasiModelPtr;
	void * qHash; 
} copasi_reaction;

/*!\brief this struct is used to contain a pointer to an instance of a COPASI class*/
typedef struct  
{
	void * CopasiCompartmentPtr;
	void * CopasiModelPtr; 
	void * qHash; 
} copasi_compartment;

BEGIN_C_DECLS

/*! 
 \brief initialize copasi -- MUST BE CALLED before calling any other functions
 \ingroup Simulation
*/
//TCAPIEXPORT void copasi_init();
/*! 
 \brief destroy copasi -- MUST BE CALLED at the end of program
 \ingroup Simulation
*/
TCAPIEXPORT void copasi_end();
/*! 
 \brief create a model
 \param char* model name
 \return copasi_model a new copasi model
 \ingroup Simulation
*/
TCAPIEXPORT copasi_model cCreateModel(const char * name);
/*! 
 \brief remove a model
 \ingroup Simulation
*/
TCAPIEXPORT void cRemoveModel(copasi_model);
/*! 
 \brief clear all contents of a model
 \ingroup Simulation
*/
//TCAPIEXPORT void clearCopasiModel(copasi_model);
/*! 
 \brief This function is only needed for calling COPASI methods not found in this library. This function compiles the COPASI model; it is called internally by the simulate and other anlysis functions. 
 \param copasi_model model
 \param int substitute nested assignments
 \ingroup Simulation
*/
TCAPIEXPORT void cCompileModel(copasi_model model, int substitute_nested_assignments);
/*! 
 \brief create a model from an Antimony or SBML file
 \param char* file name
 \return copasi_model a new copasi model
 \ingroup Simulation
*/
TCAPIEXPORT copasi_model cReadAntimonyFile(const char * filename);
/*! 
 \brief create a model from an SBML file
 \param char* file name
 \return copasi_model a new copasi model
 \ingroup Simulation
*/
TCAPIEXPORT copasi_model cReadSBMLFile(const char * filename);
/*! 
 \brief create a model from an SBML string
 \param char* SBML string
 \return copasi_model a new copasi model
 \ingroup Simulation
*/
TCAPIEXPORT copasi_model cReadSBMLString(const char * sbml);
/*! 
 \brief save a model as an SBML file
 \param copasi_model copasi model
 \param char* file name
 \ingroup Simulation
*/
TCAPIEXPORT void cWriteSBMLFile(copasi_model model, const char * filename);
/*! 
 \brief add a compartment to the model
 \param copasi_model model/*! 
 \brief scaled flux control coefficients
 \param copasi_model model
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetScaledFluxControlCoeffs(copasi_model model);
/*! 
 \brief create compartment
 \param char* compartment name
 \param double volume
 \return copasi_compartment a new compartment
 \ingroup Simulation
*/
TCAPIEXPORT copasi_compartment cCreateCompartment(copasi_model model, const char* name, double volume);
/*! 
 \brief set a volume of compartment
 \param copasi_model model
 \param char * compartment name
 \param double volume
 \ingroup Simulation
*/
TCAPIEXPORT void cSetVolume(copasi_model, const char * compartment, double volume);
/*! 
 \brief set the concentration of a species, volume of a compartment, or value of a parameter
      The function will figure out which using the name (fast lookup using hashtables).
      If the name does not exist in the model, a new global parameter will be created.
 \param copasi_model model
 \param char * name
 \param double value
 \return 0 if new variable was created. 1 if existing variable was found
 \ingroup Simulation
*/
TCAPIEXPORT int cSetValue(copasi_model, const char * name, double value);

/*! 
 \brief add a species to the model
 \param copasi_compartment model
 \param char* species name
 \param double initial value (concentration or count, depending on the model)
 \ingroup Simulation
*/
TCAPIEXPORT void cCreateSpecies(copasi_compartment compartment, const char* name, double initialValue);

/*! 
 \brief set a species as boundary or floating (will remove any assignment rules)
 \param copasi_model model
  \param char * name
 \param int boundary = 1, floating = 0 (default)
 \ingroup Simulation
*/
TCAPIEXPORT void cSetBoundarySpecies(copasi_model model, const char * species, int isBoundary);
/*! 
 \brief set a species as boundary or floating (will remove any assignment rules)
 \param copasi_model model
 \param char * species name
 \param double concentration or count
 \ingroup Simulation
*/
TCAPIEXPORT void cSetConcentration(copasi_model, const char * species, double value);
/*! 
 \brief set the assignment rule for a species (automatically assumes boundary species)
 \param copasi_model model
 \param char * species name
 \param char* formula, use 0 to remove assignment rule
 \return int 0=failed 1=success
 \ingroup Simulation
*/
TCAPIEXPORT int cSetAssignmentRule(copasi_model model, const char * species, const char * formula);

/*! 
 \brief set the value of an existing global parameter or create a new global parameter
 \param copasi_model model
 \param char* parameter name
 \param double value
  \return int 0=new value created 1=found existing value
 \ingroup Simulation
*/
TCAPIEXPORT int cSetGlobalParameter(copasi_model model, const char * name, double value);

/*! 
 \brief create a new variable that is not a constant by a formula
 \param copasi_model model
 \param char* name of new variable
 \param char* formula
 \return int 0=failed 1=success
 \ingroup Simulation
*/
TCAPIEXPORT int cCreateVariable(copasi_model model, const char * name, const char * formula);

/*! 
 \brief add a trigger and a response, where the response is defined by a target variable and an assignment formula
 \param copasi_model model
 \param char * event name
 \param char * trigger
 \param char * response: name of variable or species
 \param char* response: assignment formula
 \return int 0=failed 1=success
 \ingroup Simulation
*/
TCAPIEXPORT int cCreateEvent(copasi_model model, const char * name, const char * trigger, const char * variable, const char * formula);

/*!
 \brief add a species or set an existing species as fixed
 \param copasi_model model
 \param char* species name
 \return copasi_reaction a new reaction
 \ingroup Simulation
*/
TCAPIEXPORT copasi_reaction cCreateReaction(copasi_model model, const char* name);
/*! 
 \brief add a reactant to a reaction
 \param copasi_reaction reaction
 \param char * reactant
 \param double stoichiometry
 \ingroup Simulation
*/
TCAPIEXPORT void cAddReactant(copasi_reaction reaction, const char * species, double stoichiometry);
/*! 
 \brief add a product to a reaction
 \param copasi_reaction reaction
 \param char * product
 \param double stoichiometry
 \ingroup Simulation
*/
TCAPIEXPORT void cAddProduct(copasi_reaction reaction, const char * species, double stoichiometry);
/*! 
 \brief set reaction rate equation
 \param copasi_reaction reaction
 \param char* custom formula
 \return int success=1 failure=0
 \ingroup Simulation
*/
TCAPIEXPORT int cSetReactionRate(copasi_reaction reaction, const char * formula);

/*! 
 \brief simulate using LSODA numerical integrator
 \param copasi_model model
  \param double start time
 \param double end time
 \param int number of steps in the output
 \return tc_matrix matrix of concentration or particles
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cSimulateDeterministic(copasi_model model, double startTime, double endTime, int numSteps);
/*! 
 \brief simulate using exact stochastic algorithm
 \param copasi_model model
  \param double start time
 \param double end time
 \param int number of steps in the output
 \return tc_matrix matrix of concentration or particles
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cSimulateStochastic(copasi_model model, double startTime, double endTime, int numSteps);
/*! 
 \brief simulate using Hybrid algorithm/deterministic algorithm
 \param copasi_model model
  \param double start time
 \param double end time
 \param int number of steps in the output
 \return tc_matrix matrix of concentration or particles
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cSimulateHybrid(copasi_model model, double startTime, double endTime, int numSteps);
/*! 
 \brief simulate using Tau Leap stochastic algorithm
 \param copasi_model model
  \param double start time
 \param double end time
 \param int number of steps in the output
 \return tc_matrix matrix of concentration or particles
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cSimulateTauLeap(copasi_model model, double startTime, double endTime, int numSteps);

/*! 
 \brief bring the system to steady state
 \param copasi_model model
 \return tc_matrix matrix with 1 row and n columns, where n = number of species
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetSteadyState(copasi_model model);
/*! 
 \brief bring the system to steady state using normal simulation
 \param copasi_model model
 \param int max iterations (each iteration doubles the time duration)
 \return tc_matrix matrix with 1 row and n columns, where n = number of species
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetSteadyState2(copasi_model model, int iter);

/*! 
 \brief get the Jacobian at the current state
 \param copasi_model model
 \return tc_matrix matrix with n rows and n columns, where n = number of species
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetJacobian(copasi_model model);
/*! 
 \brief get the eigenvalues of the Jacobian at the current state
 \param copasi_model model
 \return tc_matrix matrix with 1 row and n columns, each containing an eigenvalue
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetEigenvalues(copasi_model model);

/*! 
 \brief unscaled elasticities
 \param copasi_model model
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetUnscaledElasticities(copasi_model model);

/*! unscaled concentration control coefficients
 \brief unscaled elasticities
 \param copasi_model model
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetUnscaledConcentrationControlCoeffs(copasi_model model);

/*! 
 \brief unscaled flux control coefficients
 \param copasi_model model
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetUnscaledFluxControlCoeffs(copasi_model model);

/*! 
 \brief scaled elasticities
 \param copasi_model model
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetScaledElasticities(copasi_model model);

/*! 
 \brief scaled concentration control coefficients
 \param copasi_model model
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetScaledConcentrationConcentrationCoeffs(copasi_model model);

/*! 
 \brief scaled flux control coefficients
 \param copasi_model model
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetScaledFluxControlCoeffs(copasi_model model);

/*! 
 \brief full stoichiometry matrix
 \param copasi_model model
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetFullStoichiometryMatrix(copasi_model model);

/*! 
 \brief reduced stoichiometry matrix
 \param copasi_model model
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetReducedStoichiometryMatrix(copasi_model model);

/*! 
 \brief elementary flux modes
 \param copasi_model model
 \return tc_matrix matrix with reactions as rows (with rownames) and flux modes as columns (no column names)
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetElementaryFluxModes(copasi_model model);

/*! 
 \brief get Gamma matrix (i.e. conservation laws)
 \param copasi_model model
 \return tc_matrix 
 \ingroup Structural
*/
TCAPIEXPORT tc_matrix cGetGammaMatrix(copasi_model model);

/*! 
 \brief get K matrix (right nullspace)
 \param copasi_model model
 \return tc_matrix 
 \ingroup Structural
*/
TCAPIEXPORT tc_matrix cGetKMatrix(copasi_model model);

/*! 
 \brief get K0 matrix
 \param copasi_model model
 \return tc_matrix 
 \ingroup Structural
*/
TCAPIEXPORT tc_matrix cGetK0Matrix(copasi_model model);

/*! 
 \brief get L matrix (left nullspace)
 \param copasi_model model
 \return tc_matrix 
 \ingroup Structural
*/
TCAPIEXPORT tc_matrix cGetLinkMatrix(copasi_model model);

/*! 
 \brief get L0 matrix
 \param copasi_model model
 \return tc_matrix 
 \ingroup Structural
*/
TCAPIEXPORT tc_matrix cGetL0Matrix(copasi_model model);


/*! 
 \brief fit the model parameters to time-series data
 \param copasi_model model
 \param char * filename (tab separated)
 \param tc_matrix parameters to optimize. rownames should contain parameter names, column 1 contains parameter min-values, and column 2 contains parameter max values
 \param char * pick method. Use of of the following: "GeneticAlgorithm", "LevenbergMarquardt", "SimulatedAnnealing", "NelderMead", "SRES", "ParticleSwarm", "SteepestDescent", "RandomSearch"
 \ingroup Simulation
*/
//TCAPIEXPORT void cFitModelToData(copasi_model model, const char * filename, tc_matrix params, const char * method);

/*! 
 \brief use genetic algorithms to generate a distribution of parameter values that satisfy an objective function or fit a data file
 \param copasi_model model
 \param char * objective function or filename
 \param tc_matrix parameter initial values and min and max values (3 columns)
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cOptimize(copasi_model model, const char * objective, tc_matrix input);

TCAPIEXPORT void cSetOptimizerIterations(int);

TCAPIEXPORT void cSetOptimizerSize(int);

TCAPIEXPORT void cSetOptimizerMutationRate(double);

TCAPIEXPORT void cSetOptimizerCrossoverRate(double);

END_C_DECLS
#endif

