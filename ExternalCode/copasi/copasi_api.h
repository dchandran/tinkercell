#ifndef COPASI_SIMPLE_API
#define COPASI_SIMPLE_API

#include "TC_structs.h"

/*!\brief this struct is used to contain a pointer to an instance of a COPASI class*/
typedef struct  { 	void * CopasiModelPtr;  void * CopasiDataModelPtr; } copasi_model;

/*!\brief this struct is used to contain a pointer to an instance of a COPASI class*/
typedef struct  { 	void * CopasiSpeciesPtr;  void * CopasiModelPtr;  } copasi_species;

/*!\brief this struct is used to contain a pointer to an instance of a COPASI class*/
typedef struct  { 	void * CopasiReactionPtr; void * CopasiModelPtr;  } copasi_reaction;

/*!\brief this struct is used to contain a pointer to an instance of a COPASI class*/
typedef struct  { 	void * CopasiCompartmentPtr; void * CopasiModelPtr;  } copasi_compartment;

BEGIN_C_DECLS

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
 \brief create a model
 \param char* model name
 \return copasi_model a new model
 \ingroup copasi
*/
TCAPIEXPORT copasi_model createCopasiModel();
/*! 
 \brief add a compartment to the model
 \param copasi_model model
 \param char* compartment name
 \param double volume
 \return copasi_species a new compartment
 \ingroup copasi
*/
TCAPIEXPORT copasi_compartment createCompartment(copasi_model model, const char* name, double volume);
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
 \param char* custom formula or SBO name, e.g. "Mass action (irreversible)"
 \return int success=0 failure=-1
 \ingroup copasi
*/
TCAPIEXPORT int setReactionRate(copasi_reaction reaction, const char * formula);
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
 \brief set the value of an existing global parameter or create a new global parameter
 \param copasi_model model
 \param char* parameter name
 \param double value
 \ingroup copasi
*/
TCAPIEXPORT void setGlobalParameter(copasi_model model, const char * name, double value);
/*! 
 \brief set the assignment rule for a species (automatically assumes boundary species)
 \param copasi_model model
 \param char* formula, use 0 to remove assignment rule
 \ingroup copasi
*/
TCAPIEXPORT void setAssignmentRule(copasi_species species, const char * formula);

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
/*! 
 \brief get steady state
 \param copasi_model model
 \param const char* parameter name
 \param double start value
 \param double end value
 \return tc_matrix matrix with parameter as the first column
 \ingroup copasi
*
TCAPIEXPORT tc_matrix parameterScan(copasi_model model, const char * parameter, double startvalue, double endvalue);
/*! 
 \brief get steady state
 \param copasi_model model
 \param const char* parameter name
 \param double start value
 \param double end value
 \return tc_matrix matrix of concentration and flux values as the first row
 \ingroup copasi
*
TCAPIEXPORT tc_matrix getSteadyState(copasi_model model, const char * parameter, double startvalue, double endvalue);
*/
/*! 
 \brief Compile the model. This function is called internally by the simulate functions. This function is only needed for calling COPASI methods not found in this library.
 \param copasi_model model
 \ingroup copasi
*/
TCAPIEXPORT void compileCopasiModel(copasi_model model);
END_C_DECLS
#endif

