//std
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <iostream>

//Qt lib
#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QHash>
#include <QList>
#include <QPair>
#include <QFile>

//copasi
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
#include "copasi/trajectory/CTimeSeries.h"
#include "copasi/steadystate/CSteadyStateTask.h"
#include "copasi/steadystate/CMCATask.h"
#include "copasi/steadystate/CMCAMethod.h"
#include "copasi/commandline/COptions.h"
#include "copasi/report/CCopasiContainer.h"
#include "copasi/parameterFitting/CFitTask.h"
#include "copasi/parameterFitting/CFitMethod.h"
#include "copasi/parameterFitting/CFitProblem.h"
#include "copasi/parameterFitting/CFitItem.h"
#include "copasi/parameterFitting/CExperimentSet.h"
#include "copasi/parameterFitting/CExperiment.h"
#include "copasi/parameterFitting/CExperimentObjectMap.h"
#include "copasi/report/CKeyFactory.h"

//GAlib (genetic algorithm)
#include "GASStateGA.h"
#include "GA1DArrayGenome.h"

//Antimony lib
extern "C"
{
	#define LIB_EXPORTS 1
	#include "src/antimony_api.h"
}

struct CopasiPtr 
{ 
	QString name;
	QString key;
	CMetab * species; 
	CCompartment * compartment;
	CReaction * reaction;
	CModelValue * param;
};

typedef QHash< QString, CopasiPtr > CQHash;
static void substituteString(QString& target, const QString& oldname,const QString& newname0);
static QList< CQHash* > hashTablesToCleanup;

void copasi_init()
{
	CCopasiRootContainer::init(0, NULL);
}

void copasi_end()
{
	CCopasiRootContainer::destroy();
	for (int i=0; i < hashTablesToCleanup.size(); ++i)
		delete hashTablesToCleanup[i];
}

void removeCopasiModel(copasi_model model)
{
	CCopasiRootContainer::removeDatamodel((CCopasiDataModel*)model.CopasiDataModelPtr);
}

void clearCopasiModel(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	CQHash * hash = (CQHash*)(model.qHash);
	
	if (!pModel || !pDataModel || !hash) return;
	
	CopasiPtr p;
	QStringList keys( hash->keys() );
	for (int i=0; i < keys.size(); ++i)
	{
		p = hash->value(keys[i]);
		if (p.species)
			pModel->remove(p.species);
		else
		if (p.param)
			pModel->remove(p.param);
		else
		if (p.compartment)
			pModel->remove(p.compartment);
		else
		if (p.reaction)
			pModel->remove(p.reaction);
	}
	
	hash->clear();
}

copasi_model createCopasiModel(const char * name)
{
	copasi_init();

	CCopasiDataModel* pDataModel = CCopasiRootContainer::addDatamodel();
	CModel* pModel = pDataModel->getModel();
	CQHash * qHash = new CQHash();
	
	hashTablesToCleanup += qHash;
	
	copasi_model m = { (void*)(pModel) , (void*)(pDataModel), (void*)(qHash) };
	pModel->setSBMLId( std::string(name) );
	pModel->setObjectName( std::string(name) );
	//pModel->setTimeUnit(CModel::s);
	//pModel->setVolumeUnit(CModel::microl);
	//pModel->setQuantityUnit(CModel::nMol);
	pModel->setTimeUnit(CModel::dimensionlessTime);
	pModel->setVolumeUnit(CModel::dimensionlessVolume);
	pModel->setQuantityUnit(CModel::dimensionlessQuantity);
	
	return m;
}

void createSpecies(copasi_compartment compartment, const char* name, double iv)
{
	CModel* pModel = (CModel*)(compartment.CopasiModelPtr);
	CCompartment* pCompartment = (CCompartment*)(compartment.CopasiCompartmentPtr);
	CQHash * hash = (CQHash*)(compartment.qHash);
	CMetab* pSpecies;
	
	if (!pModel || !hash || !pCompartment) return;
	
	pSpecies = pModel->createMetabolite(name, pCompartment->getObjectName(), iv, CMetab::REACTIONS);

	CopasiPtr copasiPtr = { 
			QString(pSpecies->getCN().c_str()),
			QString(pSpecies->getKey().c_str()),
			pSpecies,
			0,
			0,
			0};

	hash->insert(
				QString(pCompartment->getObjectName().c_str()) + QString("_") + QString(name),
				copasiPtr
				);

	hash->insert(QString(name), copasiPtr); //for speedy lookup
}

copasi_compartment createCompartment(copasi_model model, const char* name, double volume)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCompartment* pCompartment = pModel->createCompartment(name, volume);
	CQHash * hash = (CQHash*)(model.qHash);
	copasi_compartment c = { 0, 0 , 0};
	
	if (!pModel) return c;
	
	c.CopasiCompartmentPtr = (void*)(pCompartment);
	c.CopasiModelPtr = (void*)(pModel);
	c.qHash = model.qHash;
	
	CopasiPtr copasiPtr = { 
			QString(pCompartment->getCN().c_str()),
			QString(pCompartment->getKey().c_str()),
			0,
			pCompartment,
			0,
			0};

	hash->insert(QString(name),copasiPtr); //for speedy lookup
	
	return c;
}

int setValue(copasi_model model, const char * name, double value)
{
	CQHash * hash = (CQHash*)(model.qHash);
	QString s(name);
	
	if (!hash) return 0;
	
	if (!hash->contains(s))
	{
		setGlobalParameter(model,name,value);
		return 0;
	}

	CopasiPtr p = hash->value(s);
	
	if (p.compartment)
	{
		p.compartment->setInitialValue(value);
		p.compartment->setValue(value);
		return 1;
	}
	else
	if (p.species)
	{
		p.species->setConcentration(value);
		p.species->setValue(value);
		p.species->setInitialValue(value);
		p.species->setInitialConcentration(value);
		return 1;
	}
	else
	if (p.param)
	{
		p.param->setInitialValue(value);
		p.param->setValue(value);
		return 1;
	}
	
	setGlobalParameter(model,name,value);
	return 0;
}

void setVolume(copasi_model model, const char * name, double vol)
{
	CQHash * hash = (CQHash*)(model.qHash);
	QString s(name);
	CCompartment* pVol = 0;
	
	if (!hash) return;
	
	if (hash->contains(s) && 
		(pVol = hash->value(s).compartment))
	{
		pVol->setInitialValue(vol);
		pVol->setValue(vol);
	}
}

void setConcentration(copasi_model model, const char * name, double conc)
{
	CQHash * hash = (CQHash*)(model.qHash);
	QString s(name);
	CMetab* pSpecies = 0;
	
	if (!hash) return;
	
	if (hash->contains(s) && 
		(pSpecies = hash->value(s).species))
	{
		pSpecies->setConcentration(conc);
		pSpecies->setValue(conc);
		pSpecies->setInitialValue(conc);
		pSpecies->setInitialConcentration(conc);
	}
}

int setGlobalParameter(copasi_model model, const char * name, double value)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CQHash * hash = (CQHash*)(model.qHash);
	QString s(name);
	CModelValue * pValue = 0;
	
	if (!hash || !pModel) return 0;
	
	if (hash->contains(s) && 
		(pValue = hash->value(s).param))
	{
		pValue->setInitialValue(value);
		pValue->setValue(value);
		return 1;
	}

	//parameter not found, so create it
	if (!pValue)
	{
		pValue = pModel->createModelValue(std::string(name),value);
		pValue->setInitialValue(value);
	
		CopasiPtr copasiPtr = {
				QString(pValue->getCN().c_str()),
				QString(pValue->getKey().c_str()),
				0,
				0,
				0,
				pValue};

		hash->insert(s, copasiPtr); //for speedy lookup
	}
	
	return 0;
}

void setBoundarySpecies(copasi_model model, const char * name, int isBoundary)
{
	CQHash * hash = (CQHash*)(model.qHash);
	QString s(name);
	CMetab* pSpecies = 0;
	
	if (!hash) return;
	
	if (hash->contains(s) && 
		(pSpecies = hash->value(s).species))
	{
		if (isBoundary)
			pSpecies->setStatus(CModelEntity::FIXED);
		else
			pSpecies->setStatus(CModelEntity::REACTIONS);
	}
}

int setAssignmentRule(copasi_model model, const char * name, const char * formula)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CQHash * hash = (CQHash*)(model.qHash);
	QString s(name);
	CMetab* pSpecies = 0;
	int i;
	bool retval=true;
	
	if (!hash) return 0;
	
	if (hash->contains(s) && 
		(pSpecies = hash->value(s).species))
	{
		if (formula)
		{
			pSpecies->setStatus(CModelEntity::ASSIGNMENT);
			CFunction pFunction;
			QString qFormula(formula);
			if (pFunction.setInfix(std::string(formula)))
			{
				CFunctionParameters& variables = pFunction.getVariables();
				CFunctionParameter* pParam;

				for (i=0; i < variables.size(); ++i)
				{
					pParam = variables[i];

					QString s0(pParam->getObjectName().c_str());
					
					if (s0 == QString("time") || 
						  s0 == QString("Time") ||
				     	  s0 == QString("TIME"))
					{
						QString s1("<");
							s1 += QString(pModel->getValueReference()->getCN().c_str());
							s1 += QString(">");
						substituteString(qFormula,s0,s1);
					}
					else
					if (hash->contains(s0))
					{
					 	QString s1("<");
							s1 += hash->value(s0).name;
							s1 += QString(">");
						substituteString(qFormula,s0,s1);
					}
				}
			}

			std::string sFormula( qFormula.toAscii().data() );
			retval = retval & pSpecies->setExpression(sFormula);
		}
		else
			pSpecies->setStatus(CModelEntity::REACTIONS);
	}
	else
		return 0;
	
	return (int)retval;
}

int createVariable(copasi_model model, const char * name, const char * formula)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CQHash * hash = (CQHash*)(model.qHash);
	
	if (!hash || !pModel) return 0;

	CModelValue* pModelValue;
	QString qname(name);

	if (hash->contains(qname))
	{
			CopasiPtr ptr = hash->value(qname);
			if (ptr.species)
				return setAssignmentRule(model, name, formula);
			if (ptr.param)
				pModelValue = ptr.param;
			else
				return 0;	
	}
	else
	{
		pModelValue = pModel->createModelValue(std::string(name), 0.0);
	}
	pModelValue->setStatus(CModelValue::ASSIGNMENT);
	int i;
	bool retval = true;

	CFunction pFunction;
	QString qFormula(formula);

	if (pFunction.setInfix(std::string(formula)))
	{
		CFunctionParameters& variables = pFunction.getVariables();
		CFunctionParameter* pParam;

		for (i=0; i < variables.size(); ++i)
		{
			pParam = variables[i];

			QString s0(pParam->getObjectName().c_str());
			if (s0 == QString("time") || 
				  s0 == QString("Time") ||
		     	  s0 == QString("TIME"))
			{
				QString s1("<");
					s1 += QString(pModel->getValueReference()->getCN().c_str());
					s1 += QString(">");
				substituteString(qFormula,s0,s1);
			}
			else
			if (hash->contains(s0))
			{
			 	QString s1("<");
					s1 += hash->value(s0).name;
					s1 += QString(">");
				substituteString(qFormula,s0,s1);
			}
		}
	}

	std::string sFormula( qFormula.toAscii().data() );
	retval = retval & pModelValue->setInitialExpression(sFormula);
	retval = retval & pModelValue->setExpression(sFormula);
	
	CopasiPtr copasiPtr = { 
			QString(pModelValue->getCN().c_str()),
			QString(pModelValue->getKey().c_str()),
			0,
			0,
			0,
			pModelValue};

	hash->insert(qname, copasiPtr); //for speedy lookup
	
	return (int)retval;
}

int createEvent(copasi_model model, const char * name, const char * trigger, const char * variable, const char * formula)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CQHash * hash = (CQHash*)(model.qHash);
	
	if (!hash || !pModel)
	{
		std::cout << "no model" << std::endl;
		return 0;
	}
	
	int i;
	bool retval = true;

	if (!hash->contains(QString(variable)))
	{
		std::cout << "no hash val : " << variable << std::endl;
		return 0;
	}

	CopasiPtr ptr = hash->value(QString(variable));
	
	if (!ptr.species && !ptr.param)
	{
		std::cout << "no ptr" << std::endl;
		return 0;
	}

	CEvent * pEvent = pModel->createEvent(std::string(name));

	CFunction pFunction;
	QString qFormula(trigger);

	if (pFunction.setInfix(std::string(trigger)))  //parse trigger
	{
		CFunctionParameters& variables = pFunction.getVariables();
		CFunctionParameter* pParam;

		for (i=0; i < variables.size(); ++i)
		{
			pParam = variables[i];

			QString s0(pParam->getObjectName().c_str());
			if (s0 == QString("time") || 
				  s0 == QString("Time") ||
		     	  s0 == QString("TIME"))
			{
				QString s1("<");
					s1 += QString(pModel->getValueReference()->getCN().c_str());
					s1 += QString(">");
				substituteString(qFormula,s0,s1);
			}
			else
			if (hash->contains(s0))
			{
			 	QString s1("<");
					s1 += hash->value(s0).name;
					s1 += QString(">");
				substituteString(qFormula,s0,s1);
			}
		}
	}
	else
	{
		retval = false;
	}
	
	CExpression * expression = new CExpression(name,pModel);
	retval = retval & expression->setInfix(std::string( qFormula.toAscii().data() ));
	pEvent->setTriggerExpressionPtr(expression);   //set trigger
	
	qFormula = QString(formula);
	
	if (pFunction.setInfix(std::string(formula)))   //parse response expression
	{
		CFunctionParameters& variables = pFunction.getVariables();
		CFunctionParameter* pParam;

		for (i=0; i < variables.size(); ++i)
		{
			pParam = variables[i];

			QString s0(pParam->getObjectName().c_str());
			if (s0 == QString("time") || 
				  s0 == QString("Time") ||
			 	  s0 == QString("TIME"))
			{
				QString s1("<");
					s1 += QString(pModel->getValueReference()->getCN().c_str());
					s1 += QString(">");
				substituteString(qFormula,s0,s1);
			}
			else
			if (hash->contains(s0))
			{
			 	QString s1("<");
					s1 += hash->value(s0).name;
					s1 += QString(">");
				substituteString(qFormula,s0,s1);
			}
		}
	}
	else
	{
		return 0;
	}
	
	CCopasiVectorN< CEventAssignment > & assignments = pEvent->getAssignments();
	CEventAssignment * assgn = new CEventAssignment;
	if (ptr.species)
		retval = retval & assgn->setTargetKey(ptr.species->getCN());   //set target
	else
		retval = retval & assgn->setTargetKey(ptr.param->getCN());

	retval = retval & assgn->setExpression(std::string( qFormula.toAscii().data() ));   //set expression
	assignments.add(assgn); 
	
	return (int)retval;
}

copasi_reaction createReaction(copasi_model model, const char* name)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CQHash * hash = (CQHash*)(model.qHash);
	
	if (!pModel || !hash)
	{
		copasi_reaction r = { 0, 0 };
		return r;
	}
	
	CReaction* pReaction = pModel->createReaction(name);
	copasi_reaction r = { (void*)(pReaction), (void*)(pModel), (void*)hash };
	
	CopasiPtr copasiPtr = { 
			QString(pReaction->getCN().c_str()),
			QString(pReaction->getKey().c_str()),
			0,
			0,
			pReaction,
			0};

	hash->insert(QString(name), copasiPtr); //for speedy lookup

	return r;
}

void addReactant(copasi_reaction reaction, const char * species, double stoichiometry)
{
	CReaction* pReaction = (CReaction*)(reaction.CopasiReactionPtr);
	CQHash * hash = (CQHash*)(reaction.qHash);
	
	if (!pReaction || !hash)
	{
		std::cout << ("React is null\n");
		return;
	}

	CMetab* pSpecies = 0;
	
	QString s(species);
	if (hash->contains(s) && (pSpecies = hash->value(s).species))
	{
		CChemEq* pChemEq = &pReaction->getChemEq();
		pChemEq->addMetabolite(pSpecies->getKey(), stoichiometry, CChemEq::SUBSTRATE);
	}
	else
		std::cout << ("No reactant\n");
}

void addProduct(copasi_reaction reaction, const char * species, double stoichiometry)
{
	CReaction* pReaction = (CReaction*)(reaction.CopasiReactionPtr);
	CQHash * hash = (CQHash*)(reaction.qHash);
	CMetab* pSpecies = 0;
	
	if (!pReaction || !hash) return;
	
	QString s(species);
	if (hash->contains(s) && (pSpecies = hash->value(s).species))
	{
		CChemEq* pChemEq = &pReaction->getChemEq();
		pChemEq->addMetabolite(pSpecies->getKey(), stoichiometry, CChemEq::PRODUCT);
	}
}

int setReactionRate(copasi_reaction reaction, const char * formula)
{
	int i,j,k;
	CReaction* pReaction = (CReaction*)(reaction.CopasiReactionPtr);
	CQHash * hash = (CQHash*)(reaction.qHash);
	CModel* pModel = (CModel*)(reaction.CopasiModelPtr);
	CFunctionDB* pFunDB = CCopasiRootContainer::getFunctionList();
	
	if (!pReaction || !pModel || !hash) return 0;
	
	if (pFunDB)
	{
		std::string rateLawName(pReaction->getObjectName() + std::string("_rate_law")); //existing rate law
		
		CFunction * pFunction = dynamic_cast<CFunction*>(pFunDB->findFunction(rateLawName));
		if (pFunction)
			return (int)(pReaction->setFunction(pFunction)) - 1;

		CKinFunction* pKinFunction = new CKinFunction(rateLawName);
		pFunDB->add(pKinFunction, true);
		pFunction = pKinFunction;//dynamic_cast<CFunction*>(pFunDB->findFunction(rateLawName));
		
		if (!pFunction)
			return 0;
		
		pFunction->setReversible(TriFalse);
		
		bool ok;
		int retval = 0;

		if (pFunction->setInfix(std::string(formula)))
		{
			retval = (int)(pReaction->setFunction(pFunction));
			CFunctionParameters& variables = pFunction->getVariables();
			CFunctionParameter* pParam;

			for (i=0; i < variables.size(); ++i)
			{
				ok = false;
				pParam = variables[i];
				if (pParam->getObjectName().compare(std::string("time"))==0 ||
				     pParam->getObjectName().compare(std::string("Time"))==0 ||
				     pParam->getObjectName().compare(std::string("TIME"))==0)
				{
					pParam->setUsage(CFunctionParameter::TIME);
					ok = true;
				}
				if (ok) continue;
				
				QString s(pParam->getObjectName().c_str());
				
				if (hash->contains(s))
				{
					CopasiPtr p = hash->value(s);
					if (p.compartment)
					{
						pParam->setUsage(CFunctionParameter::VOLUME);
						pReaction->setParameterMapping(pParam->getObjectName(), p.compartment->getKey());
					}
					else
					if (p.species)
					{
						pParam->setUsage(CFunctionParameter::MODIFIER);
						const CCopasiVector < CChemEqElement > & substrates = pReaction->getChemEq().getSubstrates();
						for (k =0; k < substrates.size(); ++k)
							if (substrates[k]->getMetabolite() == p.species)
							{
								pParam->setUsage(CFunctionParameter::SUBSTRATE);
								break;
							}
						pReaction->setParameterMapping(pParam->getObjectName(), p.species->getKey());
					}
					else
					if (p.param)
					{
						pParam->setUsage(CFunctionParameter::PARAMETER);
						pReaction->setParameterMapping(pParam->getObjectName(), p.param->getKey());
					}
				}
			}

			pFunction->compile();
			
			return retval;
		}
	}

	return 0;
}

void compileCopasiModel(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	
	if (!pModel) return;
	
	CCopasiVectorNS < CCompartment > & compartments = pModel->getCompartments();
	CCopasiVector< CMetab > & species = pModel->getMetabolites();
	CCopasiVectorN< CModelValue > & params = pModel->getModelValues();
	const CCopasiObject* pObject = 0;
	std::set<const CCopasiObject*> changedObjects;
	
	for (int i=0; i < compartments.size(); ++i)
		if (compartments[i])
		{
			pObject = compartments[i]->getObject(CCopasiObjectName("Reference=InitialVolume"));
			if (pObject)
				changedObjects.insert(pObject);
		}

	for (int i=0; i < species.size(); ++i)
		if (species[i])
		{
			pObject = species[i]->getObject(CCopasiObjectName("Reference=InitialConcentration"));
			if (pObject)
				changedObjects.insert(pObject);
		}

	for (int i=0; i < params.size(); ++i)
		if (params[i])
		{
			pObject = params[i]->getObject(CCopasiObjectName("Reference=Value"));
			if (pObject)
				changedObjects.insert(pObject);
			
			pObject = params[i]->getObject(CCopasiObjectName("Reference=InitialValue"));
			if (pObject)
				changedObjects.insert(pObject);
		}

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
}

tc_matrix simulate(copasi_model model, double startTime, double endTime, int numSteps, CCopasiMethod::SubType method)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);

	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	compileCopasiModel(model);
	
	// get the task list
	CCopasiVectorN< CCopasiTask > & TaskList = * pDataModel->getTaskList();
	// get the trajectory task object
	CTrajectoryTask* pTask = dynamic_cast<CTrajectoryTask*>(TaskList["Time-Course"]);
	// if there isn’t one
	if (pTask == NULL)
	{
		// create a new one
		pTask = new CTrajectoryTask();
		// remove any existing trajectory task just to be sure since in
		// theory only the cast might have failed above
		TaskList.remove("Time-Course");
		// add the new time course task to the task list
		TaskList.add(pTask, true);
	}
	
	CCopasiMessage::clearDeque();
	
	if (startTime >= endTime)
		endTime += startTime;
	
	if (pTask && pTask->setMethodType(method))
	{
		//set the start and end time, number of steps, and save output in memory
		CTrajectoryProblem* pProblem=(CTrajectoryProblem*)pTask->getProblem();
		pProblem->setModel(pModel);
		pTask->setScheduled(true);
		pProblem->setStepNumber(numSteps);
		pProblem->setDuration(endTime-startTime);
		pDataModel->getModel()->setInitialTime(startTime);
		pProblem->setTimeSeriesRequested(true);
		try
		{
			pTask->initialize(CCopasiTask::ONLY_TIME_SERIES, pDataModel, NULL);
			pTask->process(true);
			pTask->restore();
		}
		catch(...)
		{
			std::cerr << "Error. Running the simulation failed." << std::endl;
			// check if there are additional error messages
			if (CCopasiMessage::size() > 0)
			{
				// print the messages in chronological order
				std::cerr << CCopasiMessage::getAllMessageText(true);
			}
			pTask = 0;
		}
	}
	
	if (pTask)
	{
		const CTimeSeries & timeSeries = pTask->getTimeSeries();
		int rows = timeSeries.getRecordedSteps(), 
			  cols =  (1+pModel->getNumMetabs());//timeSeries.getNumVariables();
		int i,j;
	
		tc_matrix output = tc_createMatrix(rows, cols);
	
		for (j=0; j < cols; ++j)
			tc_setColumnName( output, j, timeSeries.getTitle(j).c_str()  );
	
		for (i=0; i < rows; ++i)
			for (j=0; j < cols; ++j)
				tc_setMatrixValue( output, i, j, timeSeries.getConcentrationData(i,j) );
	
		return output;
	}
	return tc_createMatrix(0,0);
}

tc_matrix simulateDeterministic(copasi_model model, double startTime, double endTime, int numSteps)
{
	return simulate(model,startTime,endTime,numSteps,CCopasiMethod::deterministic);
}

tc_matrix simulateTauLeap(copasi_model model, double startTime, double endTime, int numSteps)
{
	return simulate(model,startTime,endTime,numSteps,CCopasiMethod::tauLeap);
}

tc_matrix simulateStochastic(copasi_model model, double startTime, double endTime, int numSteps)
{
	return simulate(model,startTime,endTime,numSteps,CCopasiMethod::directMethod);
}

tc_matrix simulateHybrid(copasi_model model, double startTime, double endTime, int numSteps)
{
	return simulate(model,startTime,endTime,numSteps,CCopasiMethod::hybridLSODA);
}

copasi_model loadModelFile(const char * filename)
{
	loadFile(filename); //load Antimony or SBML (from antimony_api.h)
	char * s = getSBMLString("__main");  //Antimony -> SBML  (from antimony_api.h)
	CCopasiDataModel* pDataModel = CCopasiRootContainer::addDatamodel();
	pDataModel->importSBMLFromString(s); //SBML -> COPASI
	CModel* pModel = pDataModel->getModel();
	copasi_model m = { (void*)(pModel) , (void*)(pDataModel) };
	freeAll(); //free Antimony  (from antimony_api.h)
	return m;
}

tc_matrix getJacobian(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	compileCopasiModel(model);
	
	// get the task list
	CCopasiVectorN< CCopasiTask > & TaskList = * pDataModel->getTaskList();
	// get the steady state task object
	CSteadyStateTask* pTask = dynamic_cast<CSteadyStateTask*>(TaskList["Steady-State"]);
	// if there isn’t one
	if (pTask == NULL)
	{
		// create a new one
		pTask = new CSteadyStateTask();
		// remove any existing steady state task just to be sure since in
		// theory only the cast might have failed above
		TaskList.remove("Steady-State");
		// add the new time course task to the task list
		TaskList.add(pTask, true);
	}
	
	CCopasiMessage::clearDeque();
	
	try
	{
		// initialize the trajectory task
		// we want complete output (HEADER, BODY and FOOTER)
		pTask->initialize(CCopasiTask::OUTPUT_COMPLETE, pDataModel, NULL);
		// now we run the actual trajectory
		pTask->process(true);
	}
	catch (...)
	{
		std::cerr << "Error when computing steady state." << std::endl;
		return tc_createMatrix(0,0);
	}
	
	const CArrayAnnotation* pAJ = pTask->getJacobianAnnotated();
	//const CEigen & getEigenValues() const;
	
	if (pAJ && pAJ->dimensionality() == 2)
	{
		std::vector<unsigned int> index(2);
		const std::vector<std::string>& annotations = pAJ->getAnnotationsString(1);
		
		int n = annotations.size();
		tc_matrix J = tc_createMatrix(n,n);
		
		for (int i=0; i < J.rows; ++i)
		{
			tc_setRowName(J, i, annotations[i].c_str());
			tc_setColumnName(J, i, annotations[i].c_str());
		}
		
		for (int i=0; i < n; ++i)
		{
			index[0] = i;
			for (int j=0; j < n; ++j)
			{
				index[1] = j;
				tc_setMatrixValue(J, i, j, (*pAJ->array())[index]);
			}
		}
		
		return J;
	}

	return tc_createMatrix(0,0);
}

tc_matrix getSteadyState(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	compileCopasiModel(model);
	
	// get the task list
	CCopasiVectorN< CCopasiTask > & TaskList = * pDataModel->getTaskList();
	// get the steady state task object
	CSteadyStateTask* pTask = dynamic_cast<CSteadyStateTask*>(TaskList["Steady-State"]);
	// if there isn’t one
	if (pTask == NULL)
	{
		// create a new one
		pTask = new CSteadyStateTask();
		// remove any existing steady state task just to be sure since in
		// theory only the cast might have failed above
		TaskList.remove("Steady-State");
		// add the new time course task to the task list
		TaskList.add(pTask, true);
	}
	
	CCopasiMessage::clearDeque();
	
	try
	{
		// initialize the trajectory task
		// we want complete output (HEADER, BODY and FOOTER)
		pTask->initialize(CCopasiTask::OUTPUT_COMPLETE, pDataModel, NULL);
		// now we run the actual trajectory
		pTask->process(true);
	}
	catch (...)
	{
		std::cerr << "Error when computing steady state." << std::endl;
		return tc_createMatrix(0,0);
	}
	
	const CArrayAnnotation* pAJ = pTask->getJacobianAnnotated();
	const CState* state = pTask->getState();
	
	if (!state)
		return tc_createMatrix(0,0);
	
	const C_FLOAT64 * pIndep = state->beginIndependent(), 
								  * pDep = state->beginDependent();

	C_INT32 numIndep = state->getNumIndependent(),
					 numDep = state->getNumDependent();

	if (pAJ && state && pAJ->dimensionality() == 2)
	{
		std::vector<unsigned int> index(2);
		const std::vector<std::string>& annotations = pAJ->getAnnotationsString(1);
		
		int n = annotations.size();
		int i;
		
		if (n != (numIndep + numDep)) return tc_createMatrix(0,0);
		
		tc_matrix SS = tc_createMatrix(n,1);
		tc_setColumnName(SS, 0, "steady-state");
		
		for (i=0; i < SS.rows; ++i)
			tc_setRowName(SS, i, annotations[i].c_str());

		for (i=0; i < numIndep; ++i)
			tc_setMatrixValue(SS, i, 0, pIndep[i]);
		
		for (i=0; i < numDep; ++i)
			tc_setMatrixValue(SS, i+numIndep, 0, pDep[i]);
		
		return SS;
	}

	return tc_createMatrix(0,0);
}

tc_matrix getEigenvalues(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	compileCopasiModel(model);
	
	// get the task list
	CCopasiVectorN< CCopasiTask > & TaskList = * pDataModel->getTaskList();
	// get the steady state task object
	CSteadyStateTask* pTask = dynamic_cast<CSteadyStateTask*>(TaskList["Steady-State"]);
	// if there isn’t one
	if (!pTask)
	{
		// create a new one
		pTask = new CSteadyStateTask();
		// remove any existing steady state task just to be sure since in
		// theory only the cast might have failed above
		TaskList.remove("Steady-State");
		// add the new time course task to the task list
		TaskList.add(pTask, true);
	}
	
	CCopasiMessage::clearDeque();
	
	try
	{
		// initialize the trajectory task
		// we want complete output (HEADER, BODY and FOOTER)
		pTask->initialize(CCopasiTask::OUTPUT_COMPLETE, pDataModel, NULL);
		// now we run the actual trajectory
		pTask->process(true);
	}
	catch (...)
	{
		std::cerr << "Error when computing steady state." << std::endl;
		return tc_createMatrix(0,0);
	}

	const CEigen & eigen = pTask->getEigenValues();
	const CVector< C_FLOAT64 > & im = eigen.getI(), 
													& re = eigen.getR();

	tc_matrix E = tc_createMatrix(im.size(),2);
	
	tc_setColumnName(E, 0, "real\0");
	tc_setColumnName(E, 1, "imaginary\0");
	for (int i=0; i < im.size() && i < re.size(); ++i)
	{
		tc_setMatrixValue(E, i,0,re[i]);
		tc_setMatrixValue(E, i,1,im[i]);
	}
	
	return E;
}

tc_matrix getUnscaledElasticities(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	compileCopasiModel(model);
	
	// get the task list
	CCopasiVectorN< CCopasiTask > & TaskList = * pDataModel->getTaskList();
	// get the MCA task object
	CMCATask* pTask = dynamic_cast<CMCATask*>(TaskList["MCA"]);
	// if there isn’t one
	if (!pTask)
	{
		// create a new one
		pTask = new CMCATask();
		// remove any existing steady state task just to be sure since in
		// theory only the cast might have failed above
		TaskList.remove("MCA");
		// add the new time course task to the task list
		TaskList.add(pTask, true);
	}
	
	CCopasiMessage::clearDeque();
	
	try
	{
		// initialize the trajectory task
		// we want complete output (HEADER, BODY and FOOTER)
		pTask->initialize(CCopasiTask::OUTPUT_COMPLETE, pDataModel, NULL);
		pTask->process(true);
	}
	catch (...)
	{
		std::cerr << "Error when performing MCA" << std::endl;
		return tc_createMatrix(0,0);
	}
	
	CMCAMethod * mcaMethod = dynamic_cast<CMCAMethod*>(pTask->getMethod());
	
	if (!mcaMethod) return tc_createMatrix(0,0);
	
	const CMatrix<C_FLOAT64> & cmatrix = mcaMethod->getUnscaledElasticities();
	const CArrayAnnotation * annot = mcaMethod->getUnscaledElasticitiesAnn();
	const std::vector<std::string>& rownames = annot->getAnnotationsString(1),
												   & colnames = annot->getAnnotationsString(2);

	int rows = cmatrix.numRows(), cols = cmatrix.numCols();
	if (rows > rownames.size()) rows = rownames.size();
	if (cols > colnames.size()) cols = colnames.size();

	tc_matrix M = tc_createMatrix(rows, cols);
	
	for (int i=0; i < rows; ++i)
		tc_setRowName(M, i, rownames[i].c_str());
	
	for (int i=0; i < cols; ++i)
		tc_setColumnName(M, i, colnames[i].c_str());
	
	for (int i=0; i < rows; ++i)
		for (int j=0; i < cols; ++j)
		{
			tc_setMatrixValue(M, i, j, cmatrix(i,j));
		}
	return M;
}

tc_matrix getUnscaledConcentrationCC(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	compileCopasiModel(model);
	
	// get the task list
	CCopasiVectorN< CCopasiTask > & TaskList = * pDataModel->getTaskList();
	// get the MCA task object
	CMCATask* pTask = dynamic_cast<CMCATask*>(TaskList["MCA"]);
	// if there isn’t one
	if (!pTask)
	{
		// create a new one
		pTask = new CMCATask();
		// remove any existing steady state task just to be sure since in
		// theory only the cast might have failed above
		TaskList.remove("MCA");
		// add the new time course task to the task list
		TaskList.add(pTask, true);
	}
	
	CCopasiMessage::clearDeque();
	
	try
	{
		// initialize the trajectory task
		// we want complete output (HEADER, BODY and FOOTER)
		pTask->initialize(CCopasiTask::OUTPUT_COMPLETE, pDataModel, NULL);
		pTask->process(true);
	}
	catch (...)
	{
		std::cerr << "Error when performing MCA" << std::endl;
		return tc_createMatrix(0,0);
	}
	
	CMCAMethod * mcaMethod = dynamic_cast<CMCAMethod*>(pTask->getMethod());
	
	if (!mcaMethod) return tc_createMatrix(0,0);
	
	const CMatrix<C_FLOAT64> & cmatrix = mcaMethod->getUnscaledConcentrationCC();
	const CArrayAnnotation * annot = mcaMethod->getUnscaledConcentrationCCAnn();
	const std::vector<std::string>& rownames = annot->getAnnotationsString(1),
												   & colnames = annot->getAnnotationsString(2);

	int rows = cmatrix.numRows(), cols = cmatrix.numCols();
	if (rows > rownames.size()) rows = rownames.size();
	if (cols > colnames.size()) cols = colnames.size();

	tc_matrix M = tc_createMatrix(rows, cols);
	
	for (int i=0; i < rows; ++i)
		tc_setRowName(M, i, rownames[i].c_str());
	
	for (int i=0; i < cols; ++i)
		tc_setColumnName(M, i, colnames[i].c_str());
	
	for (int i=0; i < rows; ++i)
		for (int j=0; i < cols; ++j)
		{
			tc_setMatrixValue(M, i, j, cmatrix(i,j));
		}
	return M;
}

tc_matrix getUnscaledFluxCC(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	compileCopasiModel(model);
	
	// get the task list
	CCopasiVectorN< CCopasiTask > & TaskList = * pDataModel->getTaskList();
	// get the MCA task object
	CMCATask* pTask = dynamic_cast<CMCATask*>(TaskList["MCA"]);
	// if there isn’t one
	if (!pTask)
	{
		// create a new one
		pTask = new CMCATask();
		// remove any existing steady state task just to be sure since in
		// theory only the cast might have failed above
		TaskList.remove("MCA");
		// add the new time course task to the task list
		TaskList.add(pTask, true);
	}
	
	CCopasiMessage::clearDeque();
	
	try
	{
		// initialize the trajectory task
		// we want complete output (HEADER, BODY and FOOTER)
		pTask->initialize(CCopasiTask::OUTPUT_COMPLETE, pDataModel, NULL);
		pTask->process(true);
	}
	catch (...)
	{
		std::cerr << "Error when performing MCA" << std::endl;
		return tc_createMatrix(0,0);
	}
	
	CMCAMethod * mcaMethod = dynamic_cast<CMCAMethod*>(pTask->getMethod());
	
	if (!mcaMethod) return tc_createMatrix(0,0);
	
	const CMatrix<C_FLOAT64> & cmatrix = mcaMethod->getUnscaledFluxCC();
	const CArrayAnnotation * annot = mcaMethod->getUnscaledFluxCCAnn();
	const std::vector<std::string>& rownames = annot->getAnnotationsString(1),
												   & colnames = annot->getAnnotationsString(2);

	int rows = cmatrix.numRows(), cols = cmatrix.numCols();
	if (rows > rownames.size()) rows = rownames.size();
	if (cols > colnames.size()) cols = colnames.size();

	tc_matrix M = tc_createMatrix(rows, cols);
	
	for (int i=0; i < rows; ++i)
		tc_setRowName(M, i, rownames[i].c_str());
	
	for (int i=0; i < cols; ++i)
		tc_setColumnName(M, i, colnames[i].c_str());
	
	for (int i=0; i < rows; ++i)
		for (int j=0; i < cols; ++j)
		{
			tc_setMatrixValue(M, i, j, cmatrix(i,j));
		}
	return M;
}

tc_matrix getScaledElasticities(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	compileCopasiModel(model);
	
	// get the task list
	CCopasiVectorN< CCopasiTask > & TaskList = * pDataModel->getTaskList();
	// get the MCA task object
	CMCATask* pTask = dynamic_cast<CMCATask*>(TaskList["MCA"]);
	// if there isn’t one
	if (!pTask)
	{
		// create a new one
		pTask = new CMCATask();
		// remove any existing steady state task just to be sure since in
		// theory only the cast might have failed above
		TaskList.remove("MCA");
		// add the new time course task to the task list
		TaskList.add(pTask, true);
	}
	
	CCopasiMessage::clearDeque();
	
	try
	{
		// initialize the trajectory task
		// we want complete output (HEADER, BODY and FOOTER)
		pTask->initialize(CCopasiTask::OUTPUT_COMPLETE, pDataModel, NULL);
		pTask->process(true);
	}
	catch (...)
	{
		std::cerr << "Error when performing MCA" << std::endl;
		return tc_createMatrix(0,0);
	}
	
	CMCAMethod * mcaMethod = dynamic_cast<CMCAMethod*>(pTask->getMethod());
	
	if (!mcaMethod) return tc_createMatrix(0,0);
	
	const CMatrix<C_FLOAT64> & cmatrix = mcaMethod->getScaledElasticities();
	const CArrayAnnotation * annot = mcaMethod->getScaledElasticitiesAnn();
	const std::vector<std::string>& rownames = annot->getAnnotationsString(1),
												   & colnames = annot->getAnnotationsString(2);

	int rows = cmatrix.numRows(), cols = cmatrix.numCols();
	if (rows > rownames.size()) rows = rownames.size();
	if (cols > colnames.size()) cols = colnames.size();

	tc_matrix M = tc_createMatrix(rows, cols);
	
	for (int i=0; i < rows; ++i)
		tc_setRowName(M, i, rownames[i].c_str());
	
	for (int i=0; i < cols; ++i)
		tc_setColumnName(M, i, colnames[i].c_str());
	
	for (int i=0; i < rows; ++i)
		for (int j=0; i < cols; ++j)
		{
			tc_setMatrixValue(M, i, j, cmatrix(i,j));
		}
	return M;
}

tc_matrix getScaledConcentrationCC(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	compileCopasiModel(model);
	
	// get the task list
	CCopasiVectorN< CCopasiTask > & TaskList = * pDataModel->getTaskList();
	// get the MCA task object
	CMCATask* pTask = dynamic_cast<CMCATask*>(TaskList["MCA"]);
	// if there isn’t one
	if (!pTask)
	{
		// create a new one
		pTask = new CMCATask();
		// remove any existing steady state task just to be sure since in
		// theory only the cast might have failed above
		TaskList.remove("MCA");
		// add the new time course task to the task list
		TaskList.add(pTask, true);
	}
	
	CCopasiMessage::clearDeque();
	
	try
	{
		// initialize the trajectory task
		// we want complete output (HEADER, BODY and FOOTER)
		pTask->initialize(CCopasiTask::OUTPUT_COMPLETE, pDataModel, NULL);
		pTask->process(true);
	}
	catch (...)
	{
		std::cerr << "Error when performing MCA" << std::endl;
		return tc_createMatrix(0,0);
	}
	
	CMCAMethod * mcaMethod = dynamic_cast<CMCAMethod*>(pTask->getMethod());
	
	if (!mcaMethod) return tc_createMatrix(0,0);
	
	const CMatrix<C_FLOAT64> & cmatrix = mcaMethod->getScaledConcentrationCC();
	const CArrayAnnotation * annot = mcaMethod->getScaledConcentrationCCAnn();
	const std::vector<std::string>& rownames = annot->getAnnotationsString(1),
												   & colnames = annot->getAnnotationsString(2);

	int rows = cmatrix.numRows(), cols = cmatrix.numCols();
	if (rows > rownames.size()) rows = rownames.size();
	if (cols > colnames.size()) cols = colnames.size();

	tc_matrix M = tc_createMatrix(rows, cols);
	
	for (int i=0; i < rows; ++i)
		tc_setRowName(M, i, rownames[i].c_str());
	
	for (int i=0; i < cols; ++i)
		tc_setColumnName(M, i, colnames[i].c_str());
	
	for (int i=0; i < rows; ++i)
		for (int j=0; i < cols; ++j)
		{
			tc_setMatrixValue(M, i, j, cmatrix(i,j));
		}
	return M;
}

tc_matrix getScaledFluxCC(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	compileCopasiModel(model);
	
	// get the task list
	CCopasiVectorN< CCopasiTask > & TaskList = * pDataModel->getTaskList();
	// get the MCA task object
	CMCATask* pTask = dynamic_cast<CMCATask*>(TaskList["MCA"]);
	// if there isn’t one
	if (!pTask)
	{
		// create a new one
		pTask = new CMCATask();
		// remove any existing steady state task just to be sure since in
		// theory only the cast might have failed above
		TaskList.remove("MCA");
		// add the new time course task to the task list
		TaskList.add(pTask, true);
	}
	
	CCopasiMessage::clearDeque();
	
	try
	{
		// initialize the trajectory task
		// we want complete output (HEADER, BODY and FOOTER)
		pTask->initialize(CCopasiTask::OUTPUT_COMPLETE, pDataModel, NULL);
		pTask->process(true);
	}
	catch (...)
	{
		std::cerr << "Error when performing MCA" << std::endl;
		return tc_createMatrix(0,0);
	}
	
	CMCAMethod * mcaMethod = dynamic_cast<CMCAMethod*>(pTask->getMethod());
	
	if (!mcaMethod) return tc_createMatrix(0,0);
	
	const CMatrix<C_FLOAT64> & cmatrix = mcaMethod->getScaledFluxCC();
	const CArrayAnnotation * annot = mcaMethod->getScaledFluxCCAnn();
	const std::vector<std::string>& rownames = annot->getAnnotationsString(1),
												   & colnames = annot->getAnnotationsString(2);

	int rows = cmatrix.numRows(), cols = cmatrix.numCols();
	if (rows > rownames.size()) rows = rownames.size();
	if (cols > colnames.size()) cols = colnames.size();

	tc_matrix M = tc_createMatrix(rows, cols);
	
	for (int i=0; i < rows; ++i)
		tc_setRowName(M, i, rownames[i].c_str());
	
	for (int i=0; i < cols; ++i)
		tc_setColumnName(M, i, colnames[i].c_str());
	
	for (int i=0; i < rows; ++i)
		for (int j=0; i < cols; ++j)
		{
			tc_setMatrixValue(M, i, j, cmatrix(i,j));
		}
	return M;
}

/** Sloten from TinkerCell  **/

static void substituteString(QString& target, const QString& oldname,const QString& newname0)
{
	if (oldname == newname0) return;
	QString newname = newname0;
	newname.replace(QRegExp("[^A-Za-z0-9_]"),QString("_@@@_"));

	QRegExp regexp1(QString("^") + oldname + QString("$")),  //just old name
		regexp2(QString("^") + oldname + QString("([^A-Za-z0-9_])")),  //oldname+(!letter/num)
		regexp3(QString("([^A-Za-z0-9_.])") + oldname + QString("$")), //(!letter/num)+oldname
		regexp4(QString("([^A-Za-z0-9_.])") + oldname + QString("([^A-Za-z0-9_])")); //(!letter/num)+oldname+(!letter/num)
	int n = regexp1.indexIn(target);
	while (n != -1)
	{
		target.replace(oldname,newname);
		n = regexp1.indexIn(target);
	}
	n = regexp2.indexIn(target);
	while (n != -1)
	{
		target.replace(regexp2,newname+QString("\\1"));
		n = regexp2.indexIn(target);
	}
	n = regexp3.indexIn(target);
	while (n != -1)
	{
		target.replace(regexp3,QString("\\1")+newname);
		n = regexp3.indexIn(target);
	}
	n = regexp4.indexIn(target);
	while (n != -1)
	{
		target.replace(regexp4,QString("\\1")+newname+QString("\\2"));
		n = regexp4.indexIn(target);
	}
	target.replace(newname,newname0);
}

tc_matrix getReducedStoichiometryMatrix(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	compileCopasiModel(model);
	
	CCopasiVector< CMetab > & species = pModel->getMetabolitesX();
	CCopasiVectorNS < CReaction > & reacs = pModel->getReactions();
	CMatrix < C_FLOAT64 > stoi = pModel->getRedStoi();

	tc_matrix N = tc_createMatrix( stoi.numRows(), stoi.numCols() );

	for  (int i=0; i < N.rows && i < species.size(); ++i)
		if (species[i])
			tc_setRowName(N, i, species[i]->getObjectName().c_str());

	for  (int i=0; i < N.cols && i < reacs.size(); ++i)
		if (reacs[i])
			tc_setColumnName(N, i, reacs[i]->getObjectName().c_str());

	for  (int i=0; i < N.rows; ++i)
		for  (int j=0; j < N.cols; ++j)
			tc_setMatrixValue(N, i, j, stoi(i,j));

	return N;
}

void fitModelToData(copasi_model model, const char * filename, tc_matrix params, const char * method)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	CQHash * hash = (CQHash*)(model.qHash);

	if (!pModel || !pDataModel || !hash) return;

	QFile file(filename);
	QStringList words;
	int numlines=1;
	
	if (file.open(QFile::ReadOnly | QFile::Text))
	{
		QString line(file.readLine());
		line.remove("#");

		if (line.contains("\t"))
			words = line.trimmed().split("\t");
		else
		if (line.contains(","))
			words = line.trimmed().split("\t");
		else
		if (line.contains(";"))
			words = line.trimmed().split(";");
		else
		if (line.contains(" "))
			words = line.trimmed().split(" ");
		else
			return; //no valid delimiter

		while (!file.atEnd())
		{
			file.readLine();
			++numlines;
		}
	}
	
	//find the species from the header of the data file
	QList< QPair<int, CMetab*> > targetSpecies;
	CopasiPtr copasiPtr;
	for (int i=0; i < words.size(); ++i)
	{
		if (hash->contains(words[i]))
		{
			copasiPtr = hash->value(words[i]);
			if (copasiPtr.species)
				targetSpecies << QPair<int,CMetab*>(i, copasiPtr.species);
		}
	}
	
	//get the target parameters
	QList< CModelValue* > targetParams;
	for (int i=0; i < params.rows; ++i)
	{
		QString rowname(tc_getRowName(params, i));
		if (hash->contains(rowname))
		{
			copasiPtr = hash->value(rowname);
			if (copasiPtr.param && copasiPtr.param->getStatus() != CModelValue::ASSIGNMENT)
				targetParams << copasiPtr.param;
		}
	}
	
	// get the task object
	CCopasiVectorN< CCopasiTask > & TaskList = * pDataModel->getTaskList();


	// get the optim task object
	CFitTask* pFitTask = dynamic_cast<CFitTask*>(TaskList["Parameter Estimation"]);

	// if there isn’t one
	if (pFitTask == NULL)
	{
		// create a new one
		pFitTask = new CFitTask();
		// remove any existing task just to be sure since in
		// theory only the cast might have failed above
		TaskList.remove("Parameter Estimation");
		// add the new task to the task list
		TaskList.add(pFitTask, true);
	}
	
	//set method
	QString sMethod(method);
	if (sMethod == QString("GeneticAlgorithm"))
		pFitTask->setMethodType(CCopasiMethod::GeneticAlgorithm);
	else
	if (sMethod == QString("SimulatedAnnealing"))
		pFitTask->setMethodType(CCopasiMethod::SimulatedAnnealing);
	else
	if (sMethod == QString("LevenbergMarquardt"))
		pFitTask->setMethodType(CCopasiMethod::LevenbergMarquardt);
	else
	if (sMethod == QString("NelderMead"))
		pFitTask->setMethodType(CCopasiMethod::NelderMead);
	else
	if (sMethod == QString("SRES"))
		pFitTask->setMethodType(CCopasiMethod::SRES);
	else
	if (sMethod == QString("ParticleSwarm"))
		pFitTask->setMethodType(CCopasiMethod::ParticleSwarm);
	else
	if (sMethod == QString("SteepestDescent"))
		pFitTask->setMethodType(CCopasiMethod::SteepestDescent);
	else
	if (sMethod == QString("RandomSearch"))
		pFitTask->setMethodType(CCopasiMethod::SteepestDescent);

	// the method in a fit task is an instance of COptMethod or a subclass
	COptMethod* pFitMethod = dynamic_cast<COptMethod*>(pFitTask->getMethod());
	// the object must be an instance of COptMethod or a subclass 
	CFitProblem* pFitProblem = dynamic_cast<CFitProblem*>(pFitTask->getProblem());
	CExperimentSet* pExperimentSet = dynamic_cast<CExperimentSet*>(pFitProblem->getParameter("Experiment Set"));
	// first experiment (we only have one here)
	CExperiment* pExperiment = new CExperiment(pDataModel);
	// tell COPASI where to find the data
	// reading data from string is not possible with the current C++ API
	pExperiment->setFileName(filename);
	// we have to tell COPASI that the data for the experiment is ...
	// separated list (the default is TAB separated)
	//pExperiment->setSeparator(","); //use default
	pExperiment->setFirstRow(1);
	pExperiment->setLastRow(numlines);
	pExperiment->setHeaderRow(1);
	pExperiment->setExperimentType(CCopasiTask::timeCourse);
	assert(pExperiment->getExperimentType() == CCopasiTask::timeCourse);
	pExperiment->setNumColumns(targetSpecies.size() + 1);
	CExperimentObjectMap* pObjectMap = &pExperiment->getObjectMap();

	//assign index for time
	pObjectMap->setNumCols(targetSpecies.size() + 1);
	pObjectMap->setRole(0, CExperiment::time);
	const CCopasiObject* pTimeReference = pModel->getObject(CCopasiObjectName("Reference=Time"));
	pObjectMap->setObjectCN(0, pTimeReference->getCN());
	
	// now we tell COPASI which column contain the concentrations of metabolites and belong to dependent variables	
	int k;
	CMetab * pMetab;
	for (int i=0; i < targetSpecies.size(); ++i)
	{
		k = targetSpecies[i].first;
		pMetab = targetSpecies[i].second;
		pObjectMap->setRole( k , CExperiment::dependent );
		const CCopasiObject* pParticleReference = pMetab->getObject(CCopasiObjectName("Reference=Concentration"));
		pObjectMap->setObjectCN(k, pParticleReference->getCN());
	}

	pExperimentSet->addExperiment(*pExperiment);
	// addExperiment makes a copy, so we need to get the added experiment
	delete pExperiment;
	pExperiment = pExperimentSet->getExperiment(0);
	// assign optimization parameters
	// get the list where we have to add the fit items
	CCopasiParameterGroup* pOptimizationItemGroup = dynamic_cast<CCopasiParameterGroup*>(pFitProblem->getParameter("OptimizationItemList"));

	// define CFitItem for each param
	for (int i=0; i < targetParams.size(); ++i)
	{
		const CCopasiObject* pParameterReference = targetParams[i]->getObject(CCopasiObjectName("Reference=Value"));
		CFitItem* pFitItem = new CFitItem(pDataModel);
		pFitItem->setObjectCN(pParameterReference->getCN());
		pFitItem->setStartValue(tc_getMatrixValue(params,i,0));
		pFitItem->setLowerBound(CCopasiObjectName(std::string(QString::number(tc_getMatrixValue(params,i,1)).toAscii().data())));
		pFitItem->setUpperBound(CCopasiObjectName(std::string(QString::number(tc_getMatrixValue(params,i,2)).toAscii().data())));
		pOptimizationItemGroup->addParameter(pFitItem);
	}
	
	try
	{
		// initialize the fit task
		// we want complete output (HEADER, BODY and FOOTER)
		bool result = pFitTask->initialize(CCopasiTask::OUTPUT_COMPLETE, pDataModel, NULL);
		if (result == true)
		{
			// running the task for this example will probably take some time
			result = pFitTask->process(true);
		}
	}
	catch (...)
	{
		// failed
		return;
	}
	pFitTask->restore();
	
	//assign optimized values back into the model
	for (int i=0; i < params.rows && i < pFitProblem->getSolutionVariables().size(); ++i)
	{
		setValue(model, tc_getRowName(params,i), pFitProblem->getSolutionVariables()[i]);
	}
}

tc_matrix getParameterDistribution(copasi_model model, const char * objective, tc_matrix input)
{
	return tc_createMatrix(0,0);
}

