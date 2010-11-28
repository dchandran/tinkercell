/**
* This is an example on how to build models with the COPASI backend API.
*/
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <iostream>
#define COPASI_MAIN

#include "copasi_api.h"
#include "copasi/copasi.h"
#include "copasi/report/CCopasiRootContainer.h"
#include "copasi/CopasiDataModel/CCopasiDataModel.h"
#include "copasi/model/CModel.h"
#include "copasi/model/CCompartment.h"
#include "copasi/model/CMetab.h"
#include "copasi/model/CReaction.h"
#include "copasi/model/CChemEq.h"
#include "copasi/model/CModelValue.h"
#include "copasi/function/CFunctionDB.h"
#include "copasi/function/CFunction.h"
#include "copasi/function/CEvaluationTree.h"
#include "copasi/report/CCopasiRootContainer.h"
#include "copasi/report/CReport.h"
#include "copasi/report/CReportDefinition.h"
#include "copasi/report/CReportDefinitionVector.h"
#include "copasi/trajectory/CTrajectoryTask.h"
#include "copasi/trajectory/CTrajectoryMethod.h"
#include "copasi/trajectory/CTrajectoryProblem.h"
#include "copasi/scan/CScanTask.h"
#include "copasi/scan/CScanMethod.h"
#include "copasi/scan/CScanProblem.h"

void copasi_init()
{
	CCopasiRootContainer::init(0, NULL);
}

void copasi_end()
{
	CCopasiRootContainer::destroy();
}

copasi_model createCopasiModel()
{
	CCopasiDataModel* pDataModel = CCopasiRootContainer::addDatamodel();
	CModel* pModel = pDataModel->getModel();
	pModel->setTimeUnit(CModel::s);
	//pModel->setVolumeUnit(CModel::microl);
	//pModel->setQuantityUnit(CModel::nMol);
	copasi_model m = { (void*)(pModel)};
	return m;
}

copasi_species createSpecies(copasi_compartment compartment, const char* name, double iv)
{
	CModel* pModel = (CModel*)(compartment.CopasiModelPtr);
	CCompartment* pCompartment = (CCompartment*)(compartment.CopasiCompartmentPtr);
	CMetab* pSpecies = pModel->createMetabolite(name, pCompartment->getObjectName(), iv, CMetab::REACTIONS);
	copasi_species s = { (void*)(pSpecies), (void*)(pModel) };
	return s;
}

copasi_compartment createCompartment(copasi_model model, const char* name, double volume)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCompartment* pCompartment = pModel->createCompartment(name, volume);
	copasi_compartment c = { (void*)(pCompartment), (void*)(pModel) };
	return c;
}

void setBoundarySpecies(copasi_species species, int isBoundary)
{
	CMetab* pSpecies = (CMetab*)(species.CopasiSpeciesPtr);
}

void setAssignmentRule(copasi_species * species, const char * formula)
{
}

copasi_reaction * createReaction(copasi_model * model, const char* name)
{
}

void addReactant(copasi_reaction * reaction, copasi_species * species, double stoichiometry)
{
}

void addProduct(copasi_reaction * reaction, copasi_species * species, double stoichiometry)
{
}

void setReactionRate(copasi_reaction * reaction, const char * formula)
{
}

tc_matrix simulateODE(copasi_model * model, double endtime, double dt, int returnConcOrFlux)
{
}

tc_matrix simulateTauLeap(copasi_model * model, double endtime, double dt, int returnConcOrFlux);
/*! 
 \brief simulate using exact stochastic method (Gillespie)
 \param copasi_model* model
 \param double end time
 \param int 0=return concentration and flux, 1=return concentration, 2=return flux
 \return tc_matrix matrix of concentration and/or flux values
 \ingroup copasi
*/
tc_matrix simulateGillespie(copasi_model * model, double endtime, int returnConcOrFlux);
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

