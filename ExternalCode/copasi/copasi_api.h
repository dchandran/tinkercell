/*! \mainpage Simplifed API for the COPASI Library
 *
 * \section intro_sec Introduction
 *
 * The developers of COPASI provide COPASI as a reusable library as well as
 * the well known COPASI user interface. The library however has a fairly
 * complex API and can take some time getting used. We have therefore layered 
 * on top of the COPASI library a new C based API that we feel is much simpler
 * to use. For example, to run a simple SBML model and generate time series data
 * we would call:
 *  
 @code
 copasi_model m;
 tc_matrix output;
  
 m = cReadSBMLFile ("mymodel.xml");
  
 output = cSimulationDeterministic (m, 0, 10, 100); 
 @endcode
 
 More complex example:
 
 @code
 #include <stdlib.h>
 #include <stdio.h>
 #include "copasi_api.h"

 int main(int nargs, char** argv)
 {
        tc_matrix efm, output, params;
        copasi_model m1, m2;
        
        if (nargs < 2)
        {
            m1 = model1();
        }
        else
        {
            printf("loading model file %s\n", argv[1]);
            m1 = cReadSBMLFile(argv[1]);        
        }

        cWriteAntimonyFile(m1, "model.txt");
        
        printf("Antimony file written to model.txt\nSimulating...\n");  
        output = cSimulateDeterministic(m1, 0, 100, 1000);  //model, start, end, num. points
        printf("output.tab has %i rows and %i columns\n",output.rows, output.cols);
        tc_printMatrixToFile("output.tab", output);
        tc_deleteMatrix(output);
                 
        cRemoveModel(m1);
        copasi_end();
        return 0;
 }
 @endcode
 * \section install_sec Installation
 *
 * Installation documentation is provided in the main google code page.
 */

#ifndef COPASI_SIMPLE_C_API
#define COPASI_SIMPLE_C_API
 /**
  * @file    copasi_api.h
  * @brief   Simple C API for the Copasi C++ library

This is a C API for the COPASI C++ library. Rate equations in COPASI require the "complete name",   
e.g. instead of X, the rate must specify <model.compartment.X>. In this C API, those complete names
are stored in a hash table. The API replaces the simple strings, i.e. "C", with the complete names by
using the hash-table. This is mainly for speed; otherwise, every cSetReactionRate would be searching
through the entire model for each of its variables. The hash-table idea is used for functions such
as cSetValue, which can set the value of a parameter or that of a molecular species. Again, it uses the
hash table to identify what a variable is. 

The C API hides the C++ classes by casting some of the main classes into void pointers inside
C structs. 

std::map is used for performing the hashing (it is not a real hash-table, but close enough).
boost::regex is used for string substitutions.
*/

#include "TC_structs.h"

/*!\brief This struct is used to contain a pointer to an instance of a COPASI object*/
typedef struct  
{ 
	void * CopasiModelPtr;
	void * CopasiDataModelPtr;
	void * qHash;
	char * errorMessage;
	char * warningMessage;
} copasi_model;

/*!\brief This struct is used to contain a pointer to an instance of a COPASI reaction object*/
typedef struct  
{
	void * CopasiReactionPtr;
	void * CopasiModelPtr;
	void * qHash; 
} copasi_reaction;

/*!\brief This struct is used to contain a pointer to an instance of a COPASI compartment object*/
typedef struct  
{
	void * CopasiCompartmentPtr;
	void * CopasiModelPtr; 
	void * qHash; 
} copasi_compartment;

BEGIN_C_DECLS

/**
  * @name Memory management
  */
/** \{*/

/*! 
 \brief destroy copasi -- MUST BE CALLED at the end of program
 \ingroup Simulation
*/
TCAPIEXPORT void copasi_end();

/*! 
 \brief remove a model
 \ingroup Simulation
*/
TCAPIEXPORT void cRemoveModel(copasi_model);

/** \} */
/**
  * @name Read and write models
  */
/** \{ */

/*! 
 \brief Create a model from an Antimony, see antimony.sf.net for details of Antimony syntax
 \param char* file name
 \return copasi_model Copasi model of the Antimony file
 \ingroup Simulation
*/
TCAPIEXPORT copasi_model cReadAntimonyFile(const char * filename);


/*! 
 \brief Create a model from an Antimony string
 \param char* Antimony string
 \return copasi_model Copasi model of the Antimony string
 \ingroup Simulation
*/
TCAPIEXPORT copasi_model cReadAntimonyString(const char * sbml);


/*! 
 \brief Create a model from an SBML file
 \param char* file name
 \return copasi_model Copasi model of the SBML file
 \ingroup Simulation
*/
TCAPIEXPORT copasi_model cReadSBMLFile(const char * filename);


/*! 
 \brief Create a model from an SBML string
 \param char* SBML string
 \return copasi_model Copasi model of the SBML string
 \ingroup Simulation
*/
TCAPIEXPORT copasi_model cReadSBMLString(const char * sbml);


/*! 
 \brief Save a model as an SBML file
 \param copasi_model copasi model
 \param char* file name
 \ingroup Simulation
*/
TCAPIEXPORT void cWriteSBMLFile(copasi_model model, const char * filename);


/*! 
 \brief Save a model as an Antimony file, see antimony.sf.net for details of Antimony syntax
 \param copasi_model copasi model
 \param char* file name
 \ingroup Simulation
*/
TCAPIEXPORT void cWriteAntimonyFile(copasi_model model, const char * filename);


/** \} */
/**
  * @name Create model group
  */
/** \{ */

/*! 
 \brief Create a model
 \param char* model name
 \return copasi_model a new copasi model
 \ingroup Simulation
*/
TCAPIEXPORT copasi_model cCreateModel(const char * name);


/*! 
 \brief This function is only needed for calling COPASI methods not found in this library. This function compiles the COPASI model; it is called internally by the simulate and other anlysis functions. 
 \param copasi_model model
 \param int substitute nested assignments
 \ingroup Simulation
*/
TCAPIEXPORT void cCompileModel(copasi_model model, int substitute_nested_assignments);


/*! 
 \brief Create compartment
 \param char* compartment name
 \param double volume
 \return copasi_compartment a new compartment
 \ingroup Simulation
*/
TCAPIEXPORT copasi_compartment cCreateCompartment(copasi_model model, const char* name, double volume);


/*! 
 \brief Set a volume of compartment
 \param copasi_model model
 \param char * compartment name
 \param double volume
 \ingroup Simulation
*/
TCAPIEXPORT void cSetVolume(copasi_model, const char * compartment, double volume);


/*! 
 \brief Set the concentration of a species, volume of a compartment, or value of a parameter
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
 \brief Add a species to the model
 \param copasi_compartment model
 \param char* species name
 \param double initial value (concentration or count, depending on the model)
 \ingroup Simulation
*/
TCAPIEXPORT void cCreateSpecies(copasi_compartment compartment, const char* name, double initialValue);


/*! 
 \brief Set a species as boundary or floating (will remove any assignment rules)
 \param copasi_model model
  \param char * name
 \param int boundary = 1, floating = 0 (default)
 \ingroup Simulation
*/
TCAPIEXPORT void cSetSpeciesType(copasi_model model, const char * species, int isBoundary);


/*! 
 \brief Set a species concentration
 \param copasi_model model
 \param char * species name
 \param double concentration or count  <- what does this mean?
 \ingroup Simulation
*/
TCAPIEXPORT void cSetConcentration(copasi_model, const char * species, double value);


/*! 
 \brief Set the assignment rule for a species (automatically assumes boundary species)
 \param copasi_model model
 \param char * species name
 \param char* formula, use 0 to remove assignment rule
 \return int 0=failed 1=success
 
 @code
 result = cSetAssignmentRule (m, "S1", "sin (time*k1)");
 @endcode
 \ingroup Simulation
*/
TCAPIEXPORT int cSetAssignmentRule(copasi_model model, const char * species, const char * formula);


/*! 
 \brief Set the value of an existing global parameter or create a new global parameter
 \param copasi_model model
 \param char* parameter name
 \param double value
  \return int 0=new value created 1=found existing value
 \ingroup Simulation
*/
TCAPIEXPORT int cSetGlobalParameter(copasi_model model, const char * name, double value);


/*! 
 \brief Create a new variable that is not a constant by a formula
 \param copasi_model model
 \param char* name of new variable
 \param char* formula
 \return int 0=failed 1=success
 \ingroup Simulation
*/
TCAPIEXPORT int cCreateVariable(copasi_model model, const char * name, const char * formula);


/*! 
 \brief Add a trigger and a response, where the response is defined by a target variable and an assignment formula
 \param copasi_model model
 \param char * event name
 \param char * trigger formula
 \param char * response: name of variable or species
 \param char* response: assignment formula
 \return int 0=failed 1=success
 
 Example Usage. The following code will create an event where the parameter k1 is halved when time > 10.
 @code
 result = cCreateEvent (m, "myEvent", "time > 10", "k1", "k1/2");
 @endcode
 \ingroup Simulation
*/
TCAPIEXPORT int cCreateEvent(copasi_model model, const char * name, const char * trigger, const char * variable, const char * formula);


/*!
 \brief Create a new reaction with a given name
 \param copasi_model model
 \param char* reaction name
 \return copasi_reaction a new reaction
 
 @code
 r = cCreateReaction (m, "J1")
 @endcode
 \ingroup Simulation
*/
TCAPIEXPORT copasi_reaction cCreateReaction(copasi_model model, const char* name);


/*! 
 \brief Add a reactant to a reaction
 \param copasi_reaction reaction
 \param char * reactant
 \param double stoichiometry
 
 @code
 cCreateReaction (m, "S1", 1);
 @endcode
 \ingroup Simulation
*/
TCAPIEXPORT void cAddReactant(copasi_reaction reaction, const char * species, double stoichiometry);

/*! 
 \brief Add a product to a reaction
 \param copasi_reaction reaction
 \param char * product
 \param double stoichiometry
 
 Create a reaction J1: 2 A -> B + C
 @code
 r = cCreateReaction (m, "J1");
 cAddReactant (r, "A", 2);
 cAddProduct (r, "B", 1);
 cAddProduct (r, "C", 1);
 @endcode

 \ingroup Simulation
*/
TCAPIEXPORT void cAddProduct(copasi_reaction reaction, const char * species, double stoichiometry);

/*! 
 \brief Set reaction rate equation
 \param copasi_reaction reaction
 \param char* custom formula
 \return int success=1 failure=0
 
 @code
 result = cSetReactionRate (r, "k1*S1");
 @endcode
 
 \ingroup Simulation
*/
TCAPIEXPORT int cSetReactionRate(copasi_reaction reaction, const char * formula);

/** \} */
/**
  * @name Time course simulation
  */
/** \{ */


/*! 
 \brief Simulate using LSODA numerical integrator
 \param copasi_model model
  \param double start time
 \param double end time
 \param int number of steps in the output
 \return tc_matrix matrix of concentration or particles
 
 @code
 result = cvSimulateDeterministic (m, 0.0, 10.0, 100);
 @endcode
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cSimulateDeterministic(copasi_model model, double startTime, double endTime, int numSteps);

/*! 
 \brief Simulate using exact stochastic algorithm
 \param copasi_model model
 \param double start time
 \param double end time
 \param int number of steps in the output
 \return tc_matrix matrix of concentration or particles
 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cSimulateStochastic(copasi_model model, double startTime, double endTime, int numSteps);

/*! 
 \brief Simulate using Hybrid algorithm/deterministic algorithm
 \param copasi_model model
  \param double start time
 \param double end time
 \param int number of steps in the output
 \return tc_matrix matrix of concentration or particles
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cSimulateHybrid(copasi_model model, double startTime, double endTime, int numSteps);

/*! 
 \brief Simulate using Tau Leap stochastic algorithm
 \param copasi_model model
  \param double start time
 \param double end time
 \param int number of steps in the output
 \return tc_matrix matrix of concentration or particles
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cSimulateTauLeap(copasi_model model, double startTime, double endTime, int numSteps);

/** \} */
/**
  * @name Steady state analysis 
  */
/** \{ */


/*! 
 \brief Bring the system to steady state by solving for the zeros of the ODE's. 
             Performs an initial simulation before solving.
 \param copasi_model model
 \return tc_matrix steady state matrix of species with 1 row and n columns, where n = number of species
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetSteadyState(copasi_model model);

/*! 
 \brief Bring the system to steady state by doing repeated simulations.
             Use this is cGetSteadyState
 \param copasi_model model
 \param int max iterations (each iteration doubles the time duration)
 \return tc_matrix steady state matrix of species with 1 row and n columns, where n = number of species
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetSteadyStateUsingSimulation(copasi_model model, int iter);

/*! 
 \brief Get the full Jacobian at the current state
 \param copasi_model model
 \return tc_matrix matrix with n rows and n columns, where n = number of species
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetJacobian(copasi_model model);
/*! 
 \brief Get the eigenvalues of the Jacobian at the current state
 \param copasi_model model
 \return tc_matrix matrix with 1 row and n columns, each containing an eigenvalue
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetEigenvalues(copasi_model model);

/** \} */
/**
  * @name Metabolic control analysis (MCA)
  */
/** \{ */


/*! 
 \brief Compute the unscaled flux control coefficients
 \param copasi_model model
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetUnscaledFluxControlCoeffs(copasi_model model);


/*! 
 \brief Compute the scaled flux control coefficients
 \param copasi_model model
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetScaledFluxControlCoeffs(copasi_model model);


/*!
 \brief Compute the unscaled concentration control coefficients
 \param copasi_model model
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetUnscaledConcentrationControlCoeffs(copasi_model model);


/*! 
 \brief Compute the scaled concentration control coefficients
 \param copasi_model model
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetScaledConcentrationConcentrationCoeffs(copasi_model model);


/*! 
 \brief Compute the unscaled elasticities
 \param copasi_model model
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetUnscaledElasticities(copasi_model model);


/*! 
 \brief Compute the scaled elasticities
 \param copasi_model model
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetScaledElasticities(copasi_model model);


/** \} */
/**
  * @name Stoichiometry matrix and matrix analysis
  */
/** \{ */


/*! 
 \brief Return the full stoichiometry matrix, N
 \param copasi_model model
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetFullStoichiometryMatrix(copasi_model model);


/*! 
 \brief Return the reduced stoichiometry matrix, Nr
 \param copasi_model model
 \return tc_matrix 
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetReducedStoichiometryMatrix(copasi_model model);


/*! 
 \brief Compute the elementary flux modes
 \param copasi_model model
 \return tc_matrix matrix with reactions as rows (with rownames) and flux modes as columns (no column names)
 \ingroup Simulation
*/
TCAPIEXPORT tc_matrix cGetElementaryFluxModes(copasi_model model);


/*! 
 \brief Compute the Gamma matrix (i.e. conservation laws)
 \param copasi_model model
 \return tc_matrix 
 \ingroup Structural
*/
TCAPIEXPORT tc_matrix cGetGammaMatrix(copasi_model model);


/*! 
 \brief Compute the K matrix (right nullspace)
 \param copasi_model model
 \return tc_matrix 
 \ingroup Structural
*/
TCAPIEXPORT tc_matrix cGetKMatrix(copasi_model model);


/*! 
 \brief Compute the K0 matrix
 \param copasi_model model
 \return tc_matrix 
 \ingroup Structural
*/
TCAPIEXPORT tc_matrix cGetK0Matrix(copasi_model model);


/*! 
 \brief Compute the L matrix (link matrix, left nullspace)
 \param copasi_model model
 \return tc_matrix 
 \ingroup Structural
*/
TCAPIEXPORT tc_matrix cGetLinkMatrix(copasi_model model);


/*! 
 \brief Compute the L0 matrix
 \param copasi_model model
 \return tc_matrix 
 \ingroup Structural
*/
TCAPIEXPORT tc_matrix cGetL0Matrix(copasi_model model);


/** \} */
/**
  * @name Optimization (incomplete)
  */
/** \{ */


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

