#ifndef COPASI_SIMPLE_API
#define COPASI_SIMPLE_API


/*! 
 \brief create a model
 \param char* model name
 \return copasi_model a new model
 \ingroup copasi
*/
copasi_model * createCopasiModel(const char* name);
/*! 
 \brief add a species to the model
 \param copasi_model* model
 \param char* species name
 \return copasi_species a new species
 \ingroup copasi
*/
copasi_species * createSpecies(copasi_model * model, const char* name);
/*! 
 \brief set a species as boundary or floating (will remove any assignment rules)
 \param copasi_model* model
 \param char* species name
 \param int boundary = 1, floating = 0 (default)
 \ingroup copasi
*/
void setBoundarySpecies(copasi_species * species, int isBoundary);
/*! 
 \brief set the assignment rule for a species (automatically assumes boundary species)
 \param copasi_model* model
 \param char* formula, use 0 to remove assignment rule
 \ingroup copasi
*/
void setAssignmentRule(copasi_species * species, const char * formula);
/*!
 \brief add a species or set an existing species as fixed
 \param copasi_model* model
 \param char* species name
 \return copasi_reaction a new reaction
 \ingroup copasi
*/
copasi_reaction * createReaction(copasi_model * model, const char* name);
/*! 
 \brief add a reactant to a reaction
 \param copasi_reaction reaction
 \param copasi_species reactant
 \param double stoichiometry
 \ingroup copasi
*/
void addReactant(copasi_reaction * reaction, copasi_species * species, double stoichiometry);
/*! 
 \brief add a product to a reaction
 \param copasi_reaction reaction
 \param copasi_species product
 \param double stoichiometry
 \ingroup copasi
*/
void addProduct(copasi_reaction * reaction, copasi_species * species, double stoichiometry);
/*! 
 \brief set reaction rate equation
 \param copasi_reaction reaction
 \param char* formula
 \ingroup copasi
*/
void setReactionRate(copasi_reaction * reaction, const char * formula);

/*! 
 \brief simulate using ODEs
 \param copasi_model* model
 \param double end time
 \param double step size
 \return tc_matrix matrix of concentration and flux values
 \ingroup copasi
*/
tc_matrix simulateODE(copasi_model * model, double endtime, double dt);
/*! 
 \brief simulate using Tau Leap (stochastic) algorithm
 \param copasi_model* model
 \param double end time
 \param double step size
 \return tc_matrix matrix of concentration and flux values
 \ingroup copasi
*/
tc_matrix simulateTauLeap(copasi_model * model, double endtime, double dt);
/*! 
 \brief simulate using exact stochastic method (Gillespie)
 \param copasi_model* model
 \param double end time
 \return tc_matrix matrix of concentration and flux values
 \ingroup copasi
*/
tc_matrix simulateGillespie(copasi_model * model, double endtime);
/*! 
 \brief get steady state
 \param copasi_model* model
 \param const char* parameter name
 \param double start value
 \param double end value
 \return tc_matrix matrix with parameter as the first column
 \ingroup copasi
*/
tc_matrix parameterScan(copasi_model * model, const char * parameter, double startvalue, double endvalue);
/*! 
 \brief get steady state
 \param copasi_model* model
 \param const char* parameter name
 \param double start value
 \param double end value
 \return tc_matrix matrix of concentration and flux values as the first row
 \ingroup copasi
*/
tc_matrix getSteadyState(copasi_model * model, const char * parameter, double startvalue, double endvalue);


#endif

