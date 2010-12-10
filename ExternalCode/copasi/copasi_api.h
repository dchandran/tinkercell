#ifndef COPASI_SIMPLE_API
#define COPASI_SIMPLE_API

#include "TC_structs.h"

/*!\brief this struct is used to contain a pointer to an instance of a COPASI class*/
typedef struct  { void * CopasiModelPtr;  void * CopasiDataModelPtr; void * qHash; } copasi_model;

/*!\brief this struct is used to contain a pointer to an instance of a COPASI class*/
typedef struct  { void * CopasiSpeciesPtr;  void * CopasiModelPtr;  } copasi_species;

/*!\brief this struct is used to contain a pointer to an instance of a COPASI class*/
typedef struct  { void * CopasiReactionPtr; void * CopasiModelPtr; void * qHash; } copasi_reaction;

/*!\brief this struct is used to contain a pointer to an instance of a COPASI class*/
typedef struct  { void * CopasiCompartmentPtr; void * CopasiModelPtr; void * qHash; } copasi_compartment;

/*!\brief this struct is used to contain a pointer to an instance of a COPASI class*/
typedef struct  { void * CopasiParameterPtr; void * CopasiModelPtr;  } copasi_parameter;

BEGIN_C_DECLS


/** @name Setup \{ */

/*! 
 \brief initialize copasi -- MUST BE CALLED before calling any other functions
 \ingroup copasi
*/
TCAPIEXPORT void copasi_init();
/*! 
 \brief destroy copasi -- MUST BE CALLED at the end of program
 \ingroup copasi
*/
TCAPIEXPORT void copasi_end();
/*! 
 \brief This function is only needed for calling COPASI methods not found in this library. This function compiles the COPASI model; it is called internally by the simulate and other anlysis functions. 
 \param copasi_model model
 \ingroup copasi
*/
TCAPIEXPORT void compileCopasiModel(copasi_model model);

/** \} @name Creating Model \{ */
/*! 
 \brief create a model
 \param char* model name
 \return copasi_model a new copasi model
 \ingroup copasi
*/
TCAPIEXPORT copasi_model createCopasiModel(const char * name);
/*! 
 \brief create a model from an Antimony or SBML file
 \param char* model name
 \return copasi_model a new copasi model
 \ingroup copasi
*/
TCAPIEXPORT copasi_model loadModelFile(const char * filename);
/*! 
 \brief add a compartment to the model
 \param copasi_model model
 \param char* compartment name
 \param double volume
 \return copasi_species a new compartment
 \ingroup copasi
*/
TCAPIEXPORT copasi_compartment createCompartment(copasi_model model, const char* name, double volume);

/** \} @name Species \{ */
/*! 
 \brief add a species to the model
 \param copasi_model model
 \param char* species name
 \param double initial value (concentration or count, depending on the model)
 \return copasi_species a new species
 \ingroup copasi
*/
TCAPIEXPORT copasi_species createSpecies(copasi_compartment model, const char* name, double initialValue);
/*! 
 \brief get the copasi species name, which is required for use in equations and other formulas
 \param copasi_species species
 \param int * (return value) size of the returned string
 \return const char* copasi ID i.e. getCN()
 \ingroup copasi
*/
TCAPIEXPORT const char * getCopasiSpeciesID(copasi_species species, int * stringSize );
/*! 
 \brief set a species as boundary or floating (will remove any assignment rules)
 \param copasi_model model
  \param copasi_species species
 \param int boundary = 1, floating = 0 (default)
 \ingroup copasi
*/
TCAPIEXPORT void setBoundarySpecies(copasi_species species, int isBoundary);
/*! 
 \brief set a species as boundary or floating (will remove any assignment rules)
 \param copasi_model model
 \param copasi_species species
 \param double concentration
 \ingroup copasi
*/
TCAPIEXPORT void setConcentration(copasi_species species, double initialValue);
/*! 
 \brief set the assignment rule for a species (automatically assumes boundary species)
 \param copasi_species species
 \param char* formula, use 0 to remove assignment rule
 \ingroup copasi
*/
TCAPIEXPORT void setAssignmentRule(copasi_species species, const char * formula);

/** \} @name Parameters and formulas\{ */

/*! 
 \brief set the value of an existing global parameter or create a new global parameter
 \param copasi_model model
 \param char* parameter name
 \param double value
 \return copasi_parameter parameter
 \ingroup copasi
*/
TCAPIEXPORT copasi_parameter setGlobalParameter(copasi_model model, const char * name, double value);
/*! 
 \brief get the copasi parameter name, which is required for use in equations and other formulas
 \param copasi_parameter parameter
 \param int * (return value) size of the returned string
 \return const char* copasi ID i.e. getCN()
 \ingroup copasi
*/
TCAPIEXPORT const char * getCopasiParameterID(copasi_parameter param, int * stringSize );
/*! 
 \brief create a new variable that is not a constant by a formula
 \param copasi_model model
 \param char* name of new variable
 \param char* formula
 \return copasi_parameter the new variable
 \ingroup copasi
*/
TCAPIEXPORT copasi_parameter createVariable(copasi_model model, const char * name, const char * formula);

/** \} @name Events\{ */
/*! 
 \brief add a trigger and a response, where the response is defined by a target variable and an assignment formula
 \param copasi_model model
 \param char * event name
 \param char * trigger
 \param char * response: name of variable or species
 \param char* response: assignment formula
 \ingroup copasi
*/
TCAPIEXPORT void createEvent(copasi_model model, const char * name, const char * trigger, const char * variable, const char * formula);

/** \} @name Reaction \{ */

/*!
 \brief add a species or set an existing species as fixed
 \param copasi_model model
 \param char* species name
 \return copasi_reaction a new reaction
 \ingroup copasi
*/
TCAPIEXPORT copasi_reaction createReaction(copasi_model model, const char* name);
/*! 
 \brief add a reactant to a reaction
 \param copasi_reaction reaction
 \param copasi_species reactant
 \param double stoichiometry
 \ingroup copasi
*/
TCAPIEXPORT void addReactant(copasi_reaction reaction, copasi_species species, double stoichiometry);
/*! 
 \brief add a product to a reaction
 \param copasi_reaction reaction
 \param copasi_species product
 \param double stoichiometry
 \ingroup copasi
*/
TCAPIEXPORT void addProduct(copasi_reaction reaction, copasi_species species, double stoichiometry);
/*! 
 \brief set reaction rate equation
 \param copasi_reaction reaction
 \param char* custom formula
 \return int success=0 failure=-1
 \ingroup copasi
*/
TCAPIEXPORT int setReactionRate(copasi_reaction reaction, const char * formula);

/** \} @name Time-Course Simulation \{ */

/*! 
 \brief simulate using LSODA numerical integrator
 \param copasi_model model
  \param double start time
 \param double end time
 \param int number of steps in the output
 \return tc_matrix matrix of concentration or particles
 \ingroup copasi
*/
TCAPIEXPORT tc_matrix simulateDeterministic(copasi_model model, double startTime, double endTime, int numSteps);
/*! 
 \brief simulate using exact stochastic algorithm
 \param copasi_model model
  \param double start time
 \param double end time
 \param int number of steps in the output
 \return tc_matrix matrix of concentration or particles
 \ingroup copasi
*/
TCAPIEXPORT tc_matrix simulateStochastic(copasi_model model, double startTime, double endTime, int numSteps);
/*! 
 \brief simulate using Hybrid algorithm/deterministic algorithm
 \param copasi_model model
  \param double start time
 \param double end time
 \param int number of steps in the output
 \return tc_matrix matrix of concentration or particles
 \ingroup copasi
*/
TCAPIEXPORT tc_matrix simulateHybrid(copasi_model model, double startTime, double endTime, int numSteps);
/*! 
 \brief simulate using Tau Leap stochastic algorithm
 \param copasi_model model
  \param double start time
 \param double end time
 \param int number of steps in the output
 \return tc_matrix matrix of concentration or particles
 \ingroup copasi
*/
TCAPIEXPORT tc_matrix simulateTauLeap(copasi_model model, double startTime, double endTime, int numSteps);

/** \} @name Steady State Analysis \{ */

/*! 
 \brief get steady states for a range of parameter values
 \param copasi_model model
 \param const char* parameter name
 \param double start value
 \param double end value
 \return tc_matrix matrix with parameter as the first column
 \ingroup copasi
*/
TCAPIEXPORT tc_matrix getSteadyStates(copasi_model model, const char * parameter, double startvalue, double endvalue);
/*! 
 \brief bring the system to steady state
 \param copasi_model model
 \return tc_matrix matrix with 1 row and n columns, where n = number of species
 \ingroup copasi
*/
TCAPIEXPORT tc_matrix getSteadyState(copasi_model model);
/*! 
 \brief get the Jacobian at the current state
 \param copasi_model model
 \return tc_matrix matrix with n rows and n columns, where n = number of species
 \ingroup copasi
*/
TCAPIEXPORT tc_matrix getJacobian(copasi_model model);
/*! 
 \brief get the eigenvalues of the Jacobian at the current state
 \param copasi_model model
 \return tc_matrix matrix with 1 row and n columns, each containing an eigenvalue
 \ingroup copasi
*/
TCAPIEXPORT tc_matrix getEigenvalues(copasi_model model);

/** \} @name Metabolic Control Analysis \{ */

/*! 
 \brief unscaled elasticities
 \param copasi_model model
 \return tc_matrix 
 \ingroup copasi
*/
TCAPIEXPORT tc_matrix getUnscaledElasticities(copasi_model model);

/*! unscaled concentration control coefficients
 \brief unscaled elasticities
 \param copasi_model model
 \return tc_matrix 
 \ingroup copasi
*/
TCAPIEXPORT tc_matrix getUnscaledConcentrationCC(copasi_model model);

/*! 
 \brief unscaled flux control coefficients
 \param copasi_model model
 \return tc_matrix 
 \ingroup copasi
*/
TCAPIEXPORT tc_matrix getUnscaledFluxCC(copasi_model model);

/*! 
 \brief scaled elasticities
 \param copasi_model model
 \return tc_matrix 
 \ingroup copasi
*/
TCAPIEXPORT tc_matrix getScaledElasticities(copasi_model model);

/*! 
 \brief scaled concentration control coefficients
 \param copasi_model model
 \return tc_matrix 
 \ingroup copasi
*/
TCAPIEXPORT tc_matrix getScaledConcentrationCC(copasi_model model);

/*! 
 \brief scaled flux control coefficients
 \param copasi_model model
 \return tc_matrix 
 \ingroup copasi
*/
TCAPIEXPORT tc_matrix getScaledFluxCC(copasi_model model);

/** \} */

END_C_DECLS
#endif

