/**
* This is an example on how to build models with the COPASI backend API.
*/
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <iostream>
#define COPASI_MAIN

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

int main()
{
	// initialize the backend library
	// since we are not interested in the arguments
	// that are passed to main, we pass 0 and NULL to
	// init
	CCopasiRootContainer::init(0, NULL);
	assert(CCopasiRootContainer::getRoot() != NULL);
	// create a new datamodel
	CCopasiDataModel* pDataModel = CCopasiRootContainer::addDatamodel();
	assert(CCopasiRootContainer::getDatamodelList()->size() == 1);
	// get the model from the datamodel
	CModel* pModel = pDataModel->getModel();
	assert(pModel != NULL);
	// set the units for the model
	// we want seconds as the time unit
	// microliter as the volume units
	// and nanomole as the substance units
	pModel->setTimeUnit(CModel::s);
	pModel->setVolumeUnit(CModel::microl);
	pModel->setQuantityUnit(CModel::nMol);

	// we have to keep a set of all the initial values that are changed during
	// the model building process
	// They are needed after the model has been built to make sure all initial
	// values are set to the correct initial value
	std::set<const CCopasiObject*> changedObjects;
	// create a compartment with the name cell and an initial volume of 5.0
	// microliter
	CCompartment* pCompartment = pModel->createCompartment("cell", 5.0);
	const CCopasiObject* pObject = pCompartment->getObject(CCopasiObjectName("Reference= InitialVolume"));
	assert(pObject != NULL);
	changedObjects.insert(pObject);
	assert(pCompartment != NULL);
	assert(pModel->getCompartments().size() == 1);

	// create a new metabolite with the name glucose and an inital
	// concentration of 10 nanomol
	// the metabolite belongs to the compartment we created and is is to be
	// fixed
	CMetab* pGlucose = pModel->createMetabolite("glucose", pCompartment->getObjectName(), 10.0, CMetab::FIXED);
	pObject = pGlucose->getObject(CCopasiObjectName("Reference=InitialConcentration"));
	assert(pObject != NULL);
	changedObjects.insert(pObject);
	assert(pCompartment != NULL);
	assert(pGlucose != NULL);
	assert(pModel->getMetabolites().size() == 1);

	// create a second metabolite called glucose-6-phosphate with an initial
	// concentration of 0. This metabolite is to be changed by reactions
	CMetab* pG6P = pModel->createMetabolite("glucose-6-phosphate", pCompartment->getObjectName(), 0.0, CMetab::REACTIONS);
	assert(pG6P != NULL);
	pObject = pG6P->getObject(CCopasiObjectName("Reference=InitialConcentration"));
	assert(pObject != NULL);
	changedObjects.insert(pObject);
	assert(pModel->getMetabolites().size() == 2);

	// another metabolite for ATP, also fixed
	CMetab* pATP = pModel->createMetabolite("ATP", pCompartment->getObjectName(), 10.0, CMetab::FIXED);
	assert(pATP != NULL);
	pObject = pATP->getObject(CCopasiObjectName("Reference=InitialConcentration"));
	assert(pObject != NULL);
	changedObjects.insert(pObject);
	assert(pModel->getMetabolites().size() == 3);
	// and one for ADP
	CMetab* pADP = pModel->createMetabolite("ADP", pCompartment->getObjectName(), 0.0, CMetab::REACTIONS);
	assert(pADP != NULL);
	pObject = pADP->getObject(CCopasiObjectName("Reference=InitialConcentration"));
	assert(pObject != NULL);
	changedObjects.insert(pObject);
	assert(pModel->getMetabolites().size() == 4);
	// now we create a reaction
	CReaction* pReaction = pModel->createReaction("hexokinase");
	assert(pReaction != NULL);
	assert(pModel->getReactions().size() == 1);
	// hexokinase converts glucose and ATP to glucose-6-phosphate and ADP
	// we can set these on the chemical equation of the reaction
	CChemEq* pChemEq = &pReaction->getChemEq();
	// glucose is a substrate with stoichiometry 1
	pChemEq->addMetabolite(pGlucose->getKey(), 1.0, CChemEq::SUBSTRATE);
	// ATP is a substrate with stoichiometry 1
	pChemEq->addMetabolite(pATP->getKey(), 1.0, CChemEq::SUBSTRATE);
	// glucose-6-phosphate is a product with stoichiometry 1
	pChemEq->addMetabolite(pG6P->getKey(), 1.0, CChemEq::PRODUCT);
	// ADP is a product with stoichiometry 1
	pChemEq->addMetabolite(pADP->getKey(), 1.0, CChemEq::PRODUCT);
	assert(pChemEq->getSubstrates().size() == 2);
	assert(pChemEq->getProducts().size() == 2);
	// this reaction is to be irreversible
	pReaction->setReversible(false);
	assert(pReaction->isReversible() == false);

	// now we ned to set a kinetic law on the reaction
	// maybe constant flux would be OK
	// we need to get the function from the function database
	CFunctionDB* pFunDB = CCopasiRootContainer::getFunctionList();
	assert(pFunDB != NULL);
	// it should be in the list of suitable functions
	// lets get all suitable functions for an irreversible reaction with 2 substrates
	// and 2 products
	std::vector<CFunction*> suitableFunctions = pFunDB->suitableFunctions(2, 2, TriFalse);
	assert(!suitableFunctions.empty());
	std::vector<CFunction*>::iterator it = suitableFunctions.begin(), endit = suitableFunctions.end();
	while (it != endit)
	{
		// we just assume that the only suitable function with Constant in
		// it’s name is the one we want
		if ((*it)->getObjectName().find("Constant") != std::string::npos)
		{
			break;
		}
		++it;
	}
	if (it != endit)
	{
	// we set the function
	// the method should be smart enough to associate the reaction entities
	// with the correct function parameters
	pReaction->setFunction(*it);
	assert(pReaction->getFunction() != NULL);
	// constant flux has only one function parameter
	assert(pReaction->getFunctionParameters().size() == 1);
	// so there should be only one entry in the parameter mapping as well
	assert(pReaction->getParameterMappings().size() == 1);
	CCopasiParameterGroup* pParameterGroup = &pReaction->getParameters();
	assert(pParameterGroup->size() == 1);
	CCopasiParameter* pParameter = pParameterGroup->getParameter(0);
	// make sure the parameter is a local parameter
	assert(pReaction->isLocalParameter(pParameter->getObjectName()));
	// now we set the value of the parameter to 0.5
	pParameter->setValue(0.5);
	pObject = pParameter->getObject(CCopasiObjectName("Reference=Value"));
	assert(pObject != NULL);
	changedObjects.insert(pObject);
	}
	else
	{
	std::cerr << "Error. Could not find irreversible michaelis menten." << std::endl;
	return 1;
	}

	// now we also create a separate reaction for the backwards reaction and
	// set the kinetic law to irreversible mass action
	// now we create a reaction

	pReaction = pModel->createReaction("hexokinase-backwards");
	assert(pReaction != NULL);
	assert(pModel->getReactions().size() == 2);
	pChemEq = &pReaction->getChemEq();
	// glucose is a product with stoichiometry 1
	pChemEq->addMetabolite(pGlucose->getKey(), 1.0, CChemEq::PRODUCT);
	// ATP is a product with stoichiometry 1
	pChemEq->addMetabolite(pATP->getKey(), 1.0, CChemEq::PRODUCT);
	// glucose-6-phosphate is a substrate with stoichiometry 1
	pChemEq->addMetabolite(pG6P->getKey(), 1.0, CChemEq::SUBSTRATE);
	// ADP is a substrate with stoichiometry 1

	pChemEq->addMetabolite(pADP->getKey(), 1.0, CChemEq::SUBSTRATE);
	assert(pChemEq->getSubstrates().size() == 2);
	assert(pChemEq->getProducts().size() == 2);
	// this reaction is to be irreversible
	pReaction->setReversible(false);
	assert(pReaction->isReversible() == false);
	// now we ned to set a kinetic law on the reaction
	CFunction* pMassAction = dynamic_cast<CFunction*>(pFunDB->findFunction("irreversible mass action"));
	std::cout << pMassAction << " == mass action\n";
	assert(pMassAction != NULL);

	// we set the function
	// the method should be smart enough to associate the reaction entities
	// with the correct function parameters
	std::cout << "mass action\n";
	pReaction->setFunction(pMassAction);

	std::cout << pReaction->getFunction() << "\n";
	assert(pReaction->getFunction() != NULL);
	assert(pReaction->getFunctionParameters().size() == 2);
	// so there should be two entries in the parameter mapping as well
	assert(pReaction->getParameterMappings().size() == 2);
	// mass action is a special case since the parameter mappings for the
	// substrates (and products) are in a vector
	// Let us create a global parameter that is determined by an assignment
	// and that is used as the rate constant of the mass action kinetics
	// it gets the name rateConstant and an initial value of 1.56
	CModelValue* pModelValue = pModel->createModelValue("rateConstant", 1.56);
	assert(pModelValue != NULL);
	pObject = pModelValue->getObject(CCopasiObjectName("Reference=InitialValue"));
	assert(pObject != NULL);
	changedObjects.insert(pObject);
	assert(pModel->getModelValues().size() == 1);
	// set the status to assignment
	pModelValue->setStatus(CModelValue::ASSIGNMENT);
	// the assignment does not have to make sense
	pModelValue->setExpression("1.0 / 4.0 + 2.0");

	// now we have to adjust the parameter mapping in the reaction so
	// that the kinetic law uses the global parameter we just created instead
	// of the local one that is created by default
	// The first parameter is the one for the rate constant, so we point it to
	// the key of out model value
	std::cout << pReaction->getFunctionParameters().size() << "\n";
	//if (pReaction->getFunctionParameters()[0]->getType() != CFunctionParameter::FLOAT64)
		//std::cout << "Problem\n";
return 0;
	pReaction->setParameterMapping(0, pModelValue->getKey());
	// now we have to set the parameter mapping for the substrates
	pReaction->addParameterMapping("substrate", pG6P->getKey());
	pReaction->addParameterMapping("substrate", pADP->getKey());

	// finally compile the model
	// compile needs to be done before updating all initial values for
	// the model with the refresh sequence

	pModel->compileIfNecessary(NULL);
	// now that we are done building the model, we have to make sure all
	// initial values are updated according to their dependencies
	std::vector<Refresh*> refreshes = pModel->buildInitialRefreshSequence(changedObjects);
	std::vector<Refresh*>::iterator it2 = refreshes.begin(), endit2 = refreshes.end();
	while (it2 != endit2)
	{
		// call each refresh
		(**it2)();
		++it2;
	}

	// save the model to a COPASI file
	// we save to a file named example1.cps, we don’t want a progress report
	// and we want to overwrite any existing file with the same name
	// Default tasks are automatically generated and will always appear in cps
	// file unless they are explicitley deleted before saving.
	pDataModel->saveModel("example1.cps", NULL, true);
	// export the model to an SBML file
	// we save to a file named example1.xml, we want to overwrite any
	// existing file with the same name and we want SBML L2V3
	pDataModel->exportSBML("example1.xml", true, 2, 3);

	// destroy the root container once we are done
	CCopasiRootContainer::destroy();
}

