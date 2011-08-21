//std
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <iostream>
#include <limits> //get max and min for double

//Qt lib
#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QHash>
#include <QList>
#include <QPair>
#include <QFile>

//copasi
#define COPASI_MAIN 1
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
#include "copasi/elementaryFluxModes/CFluxMode.h"
#include "copasi/elementaryFluxModes/CEFMTask.h"
#include "copasi/elementaryFluxModes/CEFMProblem.h"
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

//genetic algorithm (used for optimization)
#include "GASStateGA.h"
#include "GA1DArrayGenome.h"

//libstruct
#include "libstructural.h"
#include "matrix.h"

//parse math (used for optimization)
#include "muParserDef.h"
#include "muParser.h"
#include "muParserInt.h"
extern "C"
{
	#include "mtrand.h"
}

using namespace LIB_STRUCTURAL;
using namespace LIB_LA;

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
	QString assignmentRule;
};

typedef QHash< QString, CopasiPtr > CQHash;
static int substituteString(QString& target, const QString& oldname,const QString& newname0);
static QList< CQHash* > hashTablesToCleanup;
static QList< copasi_model > copasiModelsToCleanup;

static QRegExp stupidPowFunction("pow\\s*\\(\\s*([^,]+)\\s*,\\s*([^,]+)\\s*\\)");

void copasi_init()
{
	CCopasiRootContainer::init(0, NULL);
}

void copasi_end()
{
	for (int i=0; i < hashTablesToCleanup.size(); ++i)
		delete hashTablesToCleanup[i];

	QList< copasi_model > models = copasiModelsToCleanup;
	copasiModelsToCleanup.clear();
	
	for (int i=0; i < models.size(); ++i)
		cRemoveModel(models[i]);

	CCopasiRootContainer::destroy();
}

int cSetAssignmentRuleHelper(copasi_model , CMetab * , const char * );

int copasi_cleanup_assignments(copasi_model model, bool doWhile=false)
{
	CQHash * hash = (CQHash*)(model.qHash);
	if (!hash) return 0;
	
	CMetab* pSpecies = 0;
	QStringList names, assignments;
	QStringList keys = hash->keys();		
	QList<CopasiPtr> values = hash->values();

	for (int i=0; i < keys.size() && i < values.size(); ++i)
		if (values[i].species && !values[i].assignmentRule.isEmpty())
		{
			names << keys[i];
			assignments << values[i].assignmentRule;
		}
		else
		{
			names << QString();
			assignments << QString();
		}

	int retval = 1;
	bool replace_needed = doWhile;
	
	while (replace_needed)
	{
		replace_needed = false;
		for (int i=0; i < values.size(); ++i)
			if (values[i].species && !values[i].assignmentRule.isEmpty())
			{
				for (int j=0; j < names.size(); ++j)
					if (!names[j].isNull() && !names.isEmpty() && i != j && values[i].assignmentRule.contains(names[j]))
					{
						if (substituteString(values[i].assignmentRule, names[j], assignments[j]))
							replace_needed = true;
					}
			}
	}

	for (int i=0; i < values.size(); ++i)
		if (values[i].species && !values[i].assignmentRule.isEmpty())
		{
			retval = retval * cSetAssignmentRuleHelper(model, values[i].species, values[i].assignmentRule.toAscii().data());
		}
	return retval;
}

void cRemoveModel(copasi_model model)
{
	//remove from list
	for (int i=0; i < copasiModelsToCleanup.size(); ++i)
		if (copasiModelsToCleanup[i].CopasiDataModelPtr == model.CopasiDataModelPtr)
		{
			copasi_model m = { (void*)NULL, (void*)NULL, (void*)NULL, (char*)NULL };
			copasiModelsToCleanup[i] = m;
		}

	//delete model
	if (model.errorMessage)
		free(model.errorMessage);
	if (model.CopasiDataModelPtr)
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

copasi_model cCreateModel(const char * name)
{
	copasi_init();

	CCopasiDataModel* pDataModel = CCopasiRootContainer::addDatamodel();
	CModel* pModel = pDataModel->getModel();
	CQHash * qHash = new CQHash();
	copasi_model m = { (void*)(pModel) , (void*)(pDataModel), (void*)(qHash), (char*)(NULL) };
	
	hashTablesToCleanup += qHash;
	copasiModelsToCleanup += m;

	pModel->setSBMLId( std::string(name) );
	pModel->setObjectName( std::string(name) );
	//pModel->setTimeUnit(CModel::s);
	//pModel->setVolumeUnit(CModel::microl);
	//pModel->setQuantityUnit(CModel::nMol);
	pModel->setTimeUnit(CModel::dimensionlessTime);
	pModel->setVolumeUnit(CModel::dimensionlessVolume);
	pModel->setQuantityUnit(CModel::dimensionlessQuantity);
	
	cCreateVariable(m, "time", "time");
	
	return m;
}

void cCreateSpecies(copasi_compartment compartment, const char* name, double iv)
{
	CModel* pModel = (CModel*)(compartment.CopasiModelPtr);
	CCompartment* pCompartment = (CCompartment*)(compartment.CopasiCompartmentPtr);
	CQHash * hash = (CQHash*)(compartment.qHash);
	CMetab* pSpecies;
	
	if (!pModel || !hash || !pCompartment) return;	
	if (hash->contains(QString(name)))
	{
		pSpecies = hash->value(QString(name)).species;
		if (pSpecies)
		{
			pSpecies->setConcentration(iv);
			pSpecies->setValue(iv);
			pSpecies->setInitialValue(iv);
			pSpecies->setInitialConcentration(iv);
		}
		return;
	}
	
	pSpecies = pModel->createMetabolite(name, pCompartment->getObjectName(), iv, CMetab::REACTIONS);
	pSpecies->setConcentration(iv);
	pSpecies->setValue(iv);
	pSpecies->setInitialValue(iv);
	pSpecies->setInitialConcentration(iv);

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

copasi_compartment cCreateCompartment(copasi_model model, const char* name, double volume)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCompartment* pCompartment = pModel->createCompartment(name, volume);
	CQHash * hash = (CQHash*)(model.qHash);
	copasi_compartment c = { 0, 0 , 0};
	
	if (!pModel || !hash) return c;
	c.CopasiModelPtr = (void*)(pModel);
	c.qHash = model.qHash;
	
	if (hash->contains(QString(name)))
	{
		if (hash->value(QString(name)).compartment)
			c.CopasiCompartmentPtr = (void*)(hash->value(QString(name)).compartment);
		
		return c;
	}
	else
	{
		c.CopasiCompartmentPtr = (void*)(pCompartment);
	}	
	
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

int cSetValue(copasi_model model, const char * name, double value)
{
	CQHash * hash = (CQHash*)(model.qHash);
	QString s(name);
	
	if (!hash) return 0;
	
	if (!hash->contains(s))
	{
		cSetGlobalParameter(model,name,value);
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
	
	cSetGlobalParameter(model,name,value);
	return 0;
}

void cSetVolume(copasi_model model, const char * name, double vol)
{
	CQHash * hash = (CQHash*)(model.qHash);
	QString s(name);
	CCompartment* pVol = NULL;
	
	if (!hash) return;
	
	if (hash->contains(s) && 
		(pVol = hash->value(s).compartment))
	{
		pVol->setInitialValue(vol);
		pVol->setValue(vol);
	}
}

void cSetConcentration(copasi_model model, const char * name, double conc)
{
	CQHash * hash = (CQHash*)(model.qHash);
	QString s(name);
	CMetab* pSpecies = NULL;
	
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

int cSetGlobalParameter(copasi_model model, const char * name, double value)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CQHash * hash = (CQHash*)(model.qHash);
	QString s(name);
	CModelValue * pValue = NULL;
	
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

void cSetBoundarySpecies(copasi_model model, const char * name, int isBoundary)
{
	CQHash * hash = (CQHash*)(model.qHash);
	QString s(name);
	CMetab* pSpecies = NULL;
	
	if (!hash) return;
	
	if (hash->contains(s) && 
		(pSpecies = hash->value(s).species))
	{
		double iv = pSpecies->getInitialConcentration();

		if (isBoundary)
			pSpecies->setStatus(CModelEntity::FIXED);
		else
			pSpecies->setStatus(CModelEntity::REACTIONS);
		
		pSpecies->setConcentration(iv);
		pSpecies->setValue(iv);
		pSpecies->setInitialValue(iv);
		pSpecies->setInitialConcentration(iv);
	}
}

int cSetAssignmentRule(copasi_model model, const char * name, const char * formula)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CQHash * hash = (CQHash*)(model.qHash);
	QString s(name);
	int i;
	bool retval=true;
	
	if (!pModel || !hash) return 0;
	
	if (!hash->contains(s))
	{
		CCopasiVectorNS < CCompartment > & compartments = pModel->getCompartments();
		if (compartments.size() > 0 && compartments[0] != NULL)
		{
			CCompartment* pCompartment = compartments[0];
			if (pCompartment)
			{
				QString s(pCompartment->getObjectName().c_str());
				if	(hash->contains(s) &&
					hash->value(s).compartment)
					{
						copasi_compartment c = { (void*)hash->value(s).compartment, model.CopasiModelPtr, model.qHash };
						cCreateSpecies(c,name,0.0);
					}
			}
		}
	}

	if (hash->contains(s) && hash->value(s).species)
	{
		CopasiPtr & p = (*hash)[s];
		p.assignmentRule = QString(formula);
		p.assignmentRule.replace(stupidPowFunction, QString("((\\1)^(\\2))"));
		//std::cout << p.assignmentRule.toAscii().data() << "\n";
		return 1;
	}
	return 0;
}

int cSetAssignmentRuleHelper(copasi_model model, CMetab* pSpecies, const char * formula)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CQHash * hash = (CQHash*)(model.qHash);
	int i;
	bool retval=true;
	
	if (!pModel || !hash || !pSpecies) return 0;
	
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
				{
					if (!hash->contains(s0))
						cSetGlobalParameter(model,pParam->getObjectName().c_str(),1.0);				
					if (hash->contains(s0))
					{
					 	QString s1("<");
							s1 += hash->value(s0).name;
							s1 += QString(">");
						substituteString(qFormula,s0,s1);
					}
				}
			}
		}

		std::string sFormula( qFormula.toAscii().data() );
		retval = retval & pSpecies->setExpression(sFormula);
	}
	else
		pSpecies->setStatus(CModelEntity::REACTIONS);
	
	return (int)retval;
}

int cCreateVariable(copasi_model model, const char * name, const char * formula)
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
				return cSetAssignmentRule(model, name, formula);
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
			{
				if (!hash->contains(s0))
					cSetGlobalParameter(model,pParam->getObjectName().c_str(),1.0);				
				
				if (hash->contains(s0))
				{
				 	QString s1("<");
						s1 += hash->value(s0).name;
						s1 += QString(">");
					substituteString(qFormula,s0,s1);
				}
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

int cCreateEvent(copasi_model model, const char * name, const char * trigger, const char * variable, const char * formula)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CQHash * hash = (CQHash*)(model.qHash);
	
	if (!hash || !pModel)
	{
		return 0;
	}
	
	int i;
	bool retval = true;

	if (!hash->contains(QString(variable)))
	{
		cSetGlobalParameter(model,variable,1.0);
	}
	
	if (!hash->contains(QString(variable))) return 0;

	CopasiPtr ptr = hash->value(QString(variable));
	
	if (!ptr.species && !ptr.param) return 0;

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
			{
				if (!hash->contains(s0))
					cSetGlobalParameter(model,pParam->getObjectName().c_str(),1.0);
				if (hash->contains(s0))
				{
				 	QString s1("<");
						s1 += hash->value(s0).name;
						s1 += QString(">");
					substituteString(qFormula,s0,s1);
				}
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
			{
				if (!hash->contains(s0))
					cSetGlobalParameter(model,pParam->getObjectName().c_str(),1.0);

				if (hash->contains(s0))
				{
				 	QString s1("<");
						s1 += hash->value(s0).name;
						s1 += QString(">");
					substituteString(qFormula,s0,s1);
				}
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

copasi_reaction cCreateReaction(copasi_model model, const char* name)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CQHash * hash = (CQHash*)(model.qHash);
	
	if (!pModel || !hash)
	{
		copasi_reaction r = { 0, 0, 0 };
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

	QString qname(name);
	hash->insert(qname, copasiPtr); //for speedy lookup

	return r;
}

void cAddReactant(copasi_reaction reaction, const char * species, double stoichiometry)
{
	CReaction* pReaction = (CReaction*)(reaction.CopasiReactionPtr);
	CQHash * hash = (CQHash*)(reaction.qHash);
	
	if (!pReaction || !hash)
	{
		return;
	}

	CMetab* pSpecies = NULL;
	
	QString s(species);
	if (hash->contains(s) && (pSpecies = hash->value(s).species))
	{
		CChemEq* pChemEq = &pReaction->getChemEq();
		pChemEq->addMetabolite(pSpecies->getKey(), stoichiometry, CChemEq::SUBSTRATE);
	}
}

void cAddProduct(copasi_reaction reaction, const char * species, double stoichiometry)
{
	CReaction* pReaction = (CReaction*)(reaction.CopasiReactionPtr);
	CQHash * hash = (CQHash*)(reaction.qHash);
	CMetab* pSpecies = NULL;
	
	if (!pReaction || !hash) return;
	
	QString s(species);
	if (hash->contains(s) && (pSpecies = hash->value(s).species))
	{
		CChemEq* pChemEq = &pReaction->getChemEq();
		pChemEq->addMetabolite(pSpecies->getKey(), stoichiometry, CChemEq::PRODUCT);
	}
}

int cSetReactionRate(copasi_reaction reaction, const char * formula)
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

		int retval = 0;

		QString formula2(formula);
		formula2.replace(stupidPowFunction, QString("((\\1)^(\\2))"));
		//std::cout << formula2.toAscii().data() << "\n";

		if (pFunction->setInfix(std::string(formula2.toAscii().data())))
		{
			retval = (int)(pReaction->setFunction(pFunction));
			CFunctionParameters& variables = pFunction->getVariables();
			CFunctionParameter* pParam;

			for (i=0; i < variables.size(); ++i)
			{
				pParam = variables[i];
				
				QString s(pParam->getObjectName().c_str());
				
				if (s.toLower() == QString("time"))
					s = QString("time");
				
				if (!hash->contains(s))
				{
					copasi_model model = { (void*)(pModel) , (void*)(0), (void*)(hash) };
					cSetGlobalParameter(model,pParam->getObjectName().c_str(),1.0);				
				}
				
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

void cCompileModel(copasi_model model, int subs)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	
	if (!pModel) return;
	copasi_cleanup_assignments(model, (bool)subs);
	
	CCopasiVectorNS < CCompartment > & compartments = pModel->getCompartments();
	CCopasiVector< CMetab > & species = pModel->getMetabolites();
	CCopasiVectorN< CModelValue > & params = pModel->getModelValues();
	const CCopasiObject* pObject = NULL;
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
	cCompileModel(model,0);
	
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
			pTask = NULL;
		}
	}
	
	if (pTask)
	{
		const CTimeSeries & timeSeries = pTask->getTimeSeries();
		int rows = timeSeries.getRecordedSteps(), 
			  cols = (1+pModel->getNumMetabs());//timeSeries.getNumVariables();
		int i,j,k;
	
		tc_matrix output = tc_createMatrix(rows, cols);
		QStringList colnames;

		for (j=1; j < cols; ++j)
			colnames << QString(timeSeries.getTitle(j).c_str());

		colnames.sort();
		colnames.push_front(timeSeries.getTitle(0).c_str());

		for (j=0; j < cols; ++j)
			tc_setColumnName( output, j, colnames[j].toAscii().data() );
	
		for (j=0; j < cols; ++j)
		{
			k = colnames.indexOf(QString(timeSeries.getTitle(j).c_str()));
			for (i=0; i < rows; ++i)
				tc_setMatrixValue( output, i, k, timeSeries.getConcentrationData(i,j) );
		}
		return output;
	}
	return tc_createMatrix(0,0);
}

tc_matrix cSimulateDeterministic(copasi_model model, double startTime, double endTime, int numSteps)
{
	return simulate(model,startTime,endTime,numSteps,CCopasiMethod::deterministic);
}

tc_matrix cSimulateTauLeap(copasi_model model, double startTime, double endTime, int numSteps)
{
	return simulate(model,startTime,endTime,numSteps,CCopasiMethod::tauLeap);
}

tc_matrix cSimulateStochastic(copasi_model model, double startTime, double endTime, int numSteps)
{
	return simulate(model,startTime,endTime,numSteps,CCopasiMethod::stochastic);
}

tc_matrix cSimulateHybrid(copasi_model model, double startTime, double endTime, int numSteps)
{
	return simulate(model,startTime,endTime,numSteps,CCopasiMethod::hybridLSODA);
}

void cWriteSBMLFile(copasi_model model, const char * filename)
{
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	if (pDataModel)
		pDataModel->exportSBML(filename, true, 2, 3);
}

copasi_model cReadAntimonyFile(const char * filename)
{
	copasi_init();
	
	CCopasiDataModel* pDataModel = CCopasiRootContainer::addDatamodel();
	char * error = NULL;

	loadFile(filename); //load Antimony
	//get the error message, if any
	const char * err = getLastError();
	int len = 0;
	for (int i=0; err && err[i]; ++i) ++len;

	if (len > 1)
	{
		error = (char*)malloc((1+len) * sizeof(char));
		if (error)
		{
			for (int i=0; i < len; ++i) error[i] = err[i];
			error[len-1] = 0;
		}
	}
	const char * s = getSBMLString("__main");  //Antimony -> SBML (at worst, an empty model)
	copasi_model m = cReadSBMLString(s);
	freeAll(); //free Antimony
	return m;
}

copasi_model cReadSBMLFile(const char * filename)
{
	copasi_init();
	
	CCopasiDataModel* pDataModel = CCopasiRootContainer::addDatamodel();
	CModel* pModel = 0;
	CQHash * qHash = 0;	
	char * error = NULL;
	std::string s;
	try 
	{
		pDataModel->importSBML(filename); //SBML -> COPASI
		s = CCopasiMessage::getAllMessageText();
		pModel = pDataModel->getModel();
		qHash = new CQHash();	
	}
	catch(...)
	{
		s = CCopasiMessage::getAllMessageText();
	}

	int len = s.length();
	if (len > 1)
	{
		error = (char*)malloc((1+len) * sizeof(char));
		if (error)
		{
			for (int i=0; i < len; ++i) error[i] = s[i];
			error[len-1] = 0;
		}
	}
	copasi_model m = { (void*)(pModel) , (void*)(pDataModel), (void*)(qHash), (char*)(error) };
	if (pModel && qHash)
	{
		hashTablesToCleanup += qHash;
		copasiModelsToCleanup += m;
	}
	return m;
}

copasi_model cReadSBMLString(const char * sbml)
{
	copasi_init();
	
	CCopasiDataModel* pDataModel = CCopasiRootContainer::addDatamodel();
	CModel* pModel = 0;
	CQHash * qHash = 0;	
	char * error = NULL;
	std::string s;
	try 
	{
		pDataModel->importSBMLFromString(sbml); //SBML -> COPASI	
		s = CCopasiMessage::getAllMessageText();
		pModel = pDataModel->getModel();
		qHash = new CQHash();	
	}
	catch(...)
	{
		s = CCopasiMessage::getAllMessageText();
	}

	int len = s.length();
	if (len > 1)
	{
		error = (char*)malloc((1+len) * sizeof(char));
		if (error)
		{
			for (int i=0; i < len; ++i) error[i] = s[i];
			error[len-1] = 0;
		}
	}
	copasi_model m = { (void*)(pModel) , (void*)(pDataModel), (void*)(qHash), (char*)(error) };
	if (pModel && qHash)
	{
		hashTablesToCleanup += qHash;
		copasiModelsToCleanup += m;
	}
	return m;
}

tc_matrix cGetJacobian(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	cCompileModel(model,1);
	
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
	//const CEigen & cGetEigenvalues() const;
	
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

tc_matrix cGetSteadyState2(copasi_model model, int maxiter)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	
	cCompileModel(model,0);

    int iter = 0;
    double err = 2.0, eps = 0.01, time = 10.0;

   	CCopasiVectorN< CCopasiTask > & TaskList = * pDataModel->getTaskList();

    while (iter < maxiter && err > eps)
    {
        ++iter;
        time *= 2.0;

	    CTrajectoryTask* pTask = dynamic_cast<CTrajectoryTask*>(TaskList["Time-Course"]);
	    // if there isn’t one
	    if (pTask == NULL)
	    {
		    pTask = new CTrajectoryTask();
		    TaskList.remove("Time-Course");
		    TaskList.add(pTask, true);
	    }
	
    	CCopasiMessage::clearDeque();

	    if (pTask && pTask->setMethodType(CCopasiMethod::deterministic))
	    {
		    CTrajectoryProblem* pProblem=(CTrajectoryProblem*)pTask->getProblem();
		    pProblem->setModel(pModel);
		    pTask->setScheduled(true);
		    pProblem->setStepNumber(int(time * 2.0));
		    pProblem->setDuration(time);
		    pDataModel->getModel()->setInitialTime(0.0);
		    pProblem->setTimeSeriesRequested(true);
		    try
		    {
			    pTask->initialize(CCopasiTask::ONLY_TIME_SERIES, pDataModel, NULL);
			    pTask->process(true);
                //pTask->restore();
		    }
		    catch(...)
		    {
			    std::cerr << CCopasiMessage::getAllMessageText(true);
			    pTask = NULL;
		    }
	    }

        if (pTask)
        {
            const CTimeSeries & timeSeries = pTask->getTimeSeries();
            int cols = (pModel->getNumMetabs());
            int j = timeSeries.getRecordedSteps() - 1;
            double diff;
            err = 0.0;
            if (j < 1)
                err = eps * 2.0;
            else
            {
                for (int i=1; i <= cols; ++i)
                {
                    diff = timeSeries.getConcentrationData(j,i) - timeSeries.getConcentrationData(j-1,i);
                    err += diff * diff;
                }
                err /= cols;
            }

            if (err < eps)
            {
				QStringList colnames;
				for (int i=0; i < cols; ++i)
                	colnames << QString(timeSeries.getTitle(i+1).c_str());
				colnames.sort();
                tc_matrix output = tc_createMatrix(cols, 1);
				int k;
                for (int i=0; i < cols && i < colnames.size(); ++i)
                {
					k = colnames.indexOf(QString(timeSeries.getTitle(i+1).c_str()));
            		tc_setRowName( output, i, colnames[i].toAscii().data() );
                    tc_setMatrixValue( output, k, 0, timeSeries.getConcentrationData(j,i+1) );
                }
                return output;
            }
        }
	}

    tc_matrix m = tc_createMatrix(0,0);
	return m;
}

tc_matrix cGetSteadyState(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	
	cCompileModel(model,1);

	CCopasiVectorN< CCopasiTask > & TaskList = * pDataModel->getTaskList();
	CSteadyStateTask* pTask = dynamic_cast<CSteadyStateTask*>(TaskList["Steady-State"]);

	if (pTask == NULL)
	{
		pTask = new CSteadyStateTask();
		TaskList.remove("Steady-State");
		TaskList.add(pTask, true);
	}
	
	try
	{
		pTask->initialize(CCopasiTask::OUTPUT, pDataModel, NULL);
		pTask->process(true);
	}
	catch (...)
	{
		std::cerr << "Error when computing steady state." << std::endl;
		return tc_createMatrix(0,0);
	}

	CTrajectoryTask* pTask2 = dynamic_cast<CTrajectoryTask*>(TaskList["Time-Course"]);
	// if there isn’t one
	if (pTask2 == NULL)
	{
		pTask2 = new CTrajectoryTask();
		TaskList.remove("Time-Course");
		TaskList.add(pTask2, true);
	}
	
	CCopasiMessage::clearDeque();
	
	if (pTask2 && pTask2->setMethodType(CCopasiMethod::deterministic))
	{
		//set the start and end time, number of steps, and save output in memory
		CTrajectoryProblem* pProblem=(CTrajectoryProblem*)pTask2->getProblem();
		pProblem->setModel(pModel);
		pTask2->setScheduled(true);
		pProblem->setStepNumber(10);
		pProblem->setDuration(10.0);
		pDataModel->getModel()->setInitialTime(0.0);
		pProblem->setTimeSeriesRequested(true);
		try
		{
			pTask2->initialize(CCopasiTask::ONLY_TIME_SERIES, pDataModel, NULL);
			pTask2->process(true);
			pTask2->restore();
		}
		catch(...)
		{
			std::cerr << CCopasiMessage::getAllMessageText(true);
			pTask2 = NULL;
		}
	}
	
	if (pTask2)
	{
		const CTimeSeries & timeSeries = pTask2->getTimeSeries();
		int rows = (pModel->getNumMetabs());
		int i,j,k;

		tc_matrix output = tc_createMatrix(rows, 1);

		QStringList rownames;
		for (i=0; i < rows; ++i)
        	rownames << QString(timeSeries.getTitle(i+1).c_str());
		rownames.sort();
		j = timeSeries.getRecordedSteps() - 1;	

        for (i=0; i < rows && i < rownames.size(); ++i)
        {
			k = rownames.indexOf(QString(timeSeries.getTitle(i+1).c_str()));
    		tc_setRowName( output, i, rownames[i].toAscii().data() );
            tc_setMatrixValue( output, k, 0, timeSeries.getConcentrationData(j,i+1) );
        }

		return output;
	}

	return cGetSteadyState2(model,10);
}

tc_matrix cGetEigenvalues(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	cCompileModel(model,1);
	
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

tc_matrix cGetUnscaledElasticities(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	cCompileModel(model,1);
	
	// get the task list
	CCopasiVectorN< CCopasiTask > & TaskList = * pDataModel->getTaskList();
	// get the MCA task object
	CMCATask* pTask = dynamic_cast<CMCATask*>(TaskList["Metabolic Control Analysis"]);
	// if there isn’t one
	if (!pTask)
	{
		// create a new one
		pTask = new CMCATask();
		// remove any existing steady state task just to be sure since in
		// theory only the cast might have failed above
		TaskList.remove("Metabolic Control Analysis");
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
												   & colnames = annot->getAnnotationsString(0);

	int rows = cmatrix.numRows(), cols = cmatrix.numCols();
	if (rows > rownames.size()) rows = rownames.size();
	if (cols > colnames.size()) cols = colnames.size();

	tc_matrix M = tc_createMatrix(rows, cols);
	
	for (int i=0; i < rows; ++i)
		tc_setRowName(M, i, rownames[i].c_str());
	
	for (int i=0; i < cols; ++i)
		tc_setColumnName(M, i, colnames[i].c_str());
	
	for (int i=0; i < rows; ++i)
		for (int j=0; j < cols; ++j)
		{
			tc_setMatrixValue(M, i, j, cmatrix(i,j));
		}
	return M;
}

tc_matrix cGetUnscaledConcentrationControlCoeffs(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	cCompileModel(model,1);
	
	// get the task list
	CCopasiVectorN< CCopasiTask > & TaskList = * pDataModel->getTaskList();
	// get the MCA task object
	CMCATask* pTask = dynamic_cast<CMCATask*>(TaskList["Metabolic Control Analysis"]);
	// if there isn’t one
	if (!pTask)
	{
		// create a new one
		pTask = new CMCATask();
		// remove any existing steady state task just to be sure since in
		// theory only the cast might have failed above
		TaskList.remove("Metabolic Control Analysis");
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
												   & colnames = annot->getAnnotationsString(0);

	int rows = cmatrix.numRows(), cols = cmatrix.numCols();
	if (rows > rownames.size()) rows = rownames.size();
	if (cols > colnames.size()) cols = colnames.size();

	tc_matrix M = tc_createMatrix(rows, cols);
	
	for (int i=0; i < rows; ++i)
		tc_setRowName(M, i, rownames[i].c_str());
	
	for (int i=0; i < cols; ++i)
		tc_setColumnName(M, i, colnames[i].c_str());
	
	for (int i=0; i < rows; ++i)
		for (int j=0; j < cols; ++j)
		{
			tc_setMatrixValue(M, i, j, cmatrix(i,j));
		}
	return M;
}

tc_matrix cGetUnscaledFluxControlCoeffs(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	cCompileModel(model,1);
	
	// get the task list
	CCopasiVectorN< CCopasiTask > & TaskList = * pDataModel->getTaskList();
	// get the MCA task object
	CMCATask* pTask = dynamic_cast<CMCATask*>(TaskList["Metabolic Control Analysis"]);
	// if there isn’t one
	if (!pTask)
	{
		// create a new one
		pTask = new CMCATask();
		// remove any existing steady state task just to be sure since in
		// theory only the cast might have failed above
		TaskList.remove("Metabolic Control Analysis");
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
												   & colnames = annot->getAnnotationsString(0);

	int rows = cmatrix.numRows(), cols = cmatrix.numCols();
	if (rows > rownames.size()) rows = rownames.size();
	if (cols > colnames.size()) cols = colnames.size();

	tc_matrix M = tc_createMatrix(rows, cols);
	
	for (int i=0; i < rows; ++i)
		tc_setRowName(M, i, rownames[i].c_str());
	
	for (int i=0; i < cols; ++i)
		tc_setColumnName(M, i, colnames[i].c_str());
	
	for (int i=0; i < rows; ++i)
		for (int j=0; j < cols; ++j)
		{
			tc_setMatrixValue(M, i, j, cmatrix(i,j));
		}
	return M;
}

tc_matrix cGetScaledElasticities(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	cCompileModel(model,1);
	
	// get the task list
	CCopasiVectorN< CCopasiTask > & TaskList = * pDataModel->getTaskList();
	// get the MCA task object
	CMCATask* pTask = dynamic_cast<CMCATask*>(TaskList["Metabolic Control Analysis"]);
	// if there isn’t one
	if (!pTask)
	{
		// create a new one
		pTask = new CMCATask();
		// remove any existing steady state task just to be sure since in
		// theory only the cast might have failed above
		TaskList.remove("Metabolic Control Analysis");
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
												   & colnames = annot->getAnnotationsString(0);

	int rows = cmatrix.numRows(), cols = cmatrix.numCols();
	if (rows > rownames.size()) rows = rownames.size();
	if (cols > colnames.size()) cols = colnames.size();

	tc_matrix M = tc_createMatrix(rows, cols);
	
	for (int i=0; i < rows; ++i)
		tc_setRowName(M, i, rownames[i].c_str());
	
	for (int i=0; i < cols; ++i)
		tc_setColumnName(M, i, colnames[i].c_str());
	
	for (int i=0; i < rows; ++i)
		for (int j=0; j < cols; ++j)
		{
			tc_setMatrixValue(M, i, j, cmatrix(i,j));
		}
	return M;
}

tc_matrix cGetScaledConcentrationConcentrationCoeffs(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	cCompileModel(model,1);
	
	// get the task list
	CCopasiVectorN< CCopasiTask > & TaskList = * pDataModel->getTaskList();
	// get the MCA task object
	CMCATask* pTask = dynamic_cast<CMCATask*>(TaskList["Metabolic Control Analysis"]);
	// if there isn’t one
	if (!pTask)
	{
		// create a new one
		pTask = new CMCATask();
		// remove any existing steady state task just to be sure since in
		// theory only the cast might have failed above
		TaskList.remove("Metabolic Control Analysis");
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
												   & colnames = annot->getAnnotationsString(0);

	int rows = cmatrix.numRows(), cols = cmatrix.numCols();
	if (rows > rownames.size()) rows = rownames.size();
	if (cols > colnames.size()) cols = colnames.size();

	tc_matrix M = tc_createMatrix(rows, cols);
	
	for (int i=0; i < rows; ++i)
		tc_setRowName(M, i, rownames[i].c_str());
	
	for (int i=0; i < cols; ++i)
		tc_setColumnName(M, i, colnames[i].c_str());
	
	for (int i=0; i < rows; ++i)
		for (int j=0; j < cols; ++j)
		{
			tc_setMatrixValue(M, i, j, cmatrix(i,j));
		}
	return M;
}

tc_matrix cGetScaledFluxControlCoeffs(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	cCompileModel(model,1);
	
	// get the task list
	CCopasiVectorN< CCopasiTask > & TaskList = * pDataModel->getTaskList();
	// get the MCA task object
	CMCATask* pTask = dynamic_cast<CMCATask*>(TaskList["Metabolic Control Analysis"]);
	// if there isn’t one
	if (!pTask)
	{
		// create a new one
		pTask = new CMCATask();
		// remove any existing steady state task just to be sure since in
		// theory only the cast might have failed above
		TaskList.remove("Metabolic Control Analysis");
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
												   & colnames = annot->getAnnotationsString(0);

	size_t rows = cmatrix.numRows(), cols = cmatrix.numCols();
	if (rows > rownames.size()) rows = rownames.size();
	if (cols > colnames.size()) cols = colnames.size();
	
	tc_matrix M = tc_createMatrix(rows, cols);

	for (int i=0; i < rows; ++i)
		tc_setRowName(M, i, rownames[i].c_str());
	
	for (int i=0; i < cols; ++i)	
		tc_setColumnName(M, i, colnames[i].c_str());
	
	for (int i=0; i < rows; ++i)
		for (int j=0; j < cols; ++j)
		{
			tc_setMatrixValue(M, i, j, cmatrix(i,j));
		}
	return M;
}

/** Sloten from TinkerCell  **/

static int substituteString(QString& target, const QString& oldname,const QString& newname0)
{
	if (oldname == newname0) return 0;
	QString newname = newname0;
	newname.replace(QRegExp("[^A-Za-z0-9_]"),QString("_@@@_"));

	QRegExp regexp1(QString("^") + oldname + QString("$")),  //just old name
		regexp2(QString("^") + oldname + QString("([^A-Za-z0-9_])")),  //oldname+(!letter/num)
		regexp3(QString("([^A-Za-z0-9_\\.=])") + oldname + QString("$")), //(!letter/num)+oldname
		regexp4(QString("([^A-Za-z0-9_\\.=])") + oldname + QString("([^A-Za-z0-9_])")); //(!letter/num)+oldname+(!letter/num)

	int retval = 0;	
	int n = regexp1.indexIn(target);
	while (n != -1)
	{
		retval = 1;
		target.replace(oldname,newname);
		n = regexp1.indexIn(target);
	}
	n = regexp2.indexIn(target);
	while (n != -1)
	{
		retval = 1;
		target.replace(regexp2,newname+QString("\\1"));
		n = regexp2.indexIn(target);
	}
	n = regexp3.indexIn(target);
	while (n != -1)
	{
		retval = 1;
		target.replace(regexp3,QString("\\1")+newname);
		n = regexp3.indexIn(target);
	}
	n = regexp4.indexIn(target);
	while (n != -1)
	{
		retval = 1;
		target.replace(regexp4,QString("\\1")+newname+QString("\\2"));
		n = regexp4.indexIn(target);
	}
	target.replace(newname,newname0);
	return retval;
}

tc_matrix cGetReducedStoichiometryMatrix(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	cCompileModel(model,0);
	
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

tc_matrix cGetFullStoichiometryMatrix(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	cCompileModel(model,0);
	
	CCopasiVector< CMetab > & species = pModel->getMetabolites();
	CCopasiVectorNS < CReaction > & reacs = pModel->getReactions();
	CMatrix < C_FLOAT64 > stoi = pModel->getStoi();

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

tc_matrix cGetElementaryFluxModes(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	cCompileModel(model,0);
	
	CCopasiVectorN< CCopasiTask > & TaskList = * pDataModel->getTaskList();

	CEFMTask* pTask = 0;
	
	if (TaskList["Elementary Flux Modes"])
		pTask = dynamic_cast<CEFMTask*>(TaskList["Elementary Flux Modes"]);
	
	if (!pTask)
	{
		pTask = new CEFMTask();
		TaskList.remove("Elementary Flux Modes");
		TaskList.add(pTask, true);
	}
	
	CCopasiMessage::clearDeque();
	
	try
	{
		pTask->initialize(CCopasiTask::OUTPUT_COMPLETE, pDataModel, NULL);
		pTask->process(true);
	}
	catch (...)
	{
		std::cerr << "Error when computing EFM" << std::endl;
		return tc_createMatrix(0,0);
	}
	
	const std::vector< CFluxMode > & fluxModes = pTask->getFluxModes();
	CEFMProblem* pProblem = dynamic_cast<CEFMProblem*>(pTask->getProblem());
	
	if (!pProblem)
		return tc_createMatrix(0,0);

	std::vector< const CReaction * > & reactions = pProblem->getReorderedReactions();
	tc_matrix M = tc_createMatrix( reactions.size() , fluxModes.size() );
	for (int i=0; i < reactions.size(); ++i)
		tc_setRowName(M, i, reactions[i]->getObjectName().c_str());
	
	for (int i=0; i < fluxModes.size(); ++i)
	{
		CFluxMode::const_iterator itMode = fluxModes[i].begin();
		CFluxMode::const_iterator endMode = fluxModes[i].end();
		for (; itMode != endMode; ++itMode)
			tc_setMatrixValue( M, itMode->first, i, itMode->second);
	}
	return M;
}

/*
void cFitModelToData(copasi_model model, const char * filename, tc_matrix params, const char * method)
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
		
		file.close();
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
			{
				targetSpecies << QPair<int,CMetab*>(i, copasiPtr.species);
				std::cout << i << "  =  " << words[i].toAscii().data() << std::endl;
			}
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
			{
				targetParams << copasiPtr.param;
				std::cout << "good  " << i << "  =  " << rowname.toAscii().data() << std::endl;
			}
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
	if (sMethod.toLower() == QString("geneticalgorithm"))
		pFitTask->setMethodType(CCopasiMethod::GeneticAlgorithm);
	else
	if (sMethod.toLower() == QString("simulatedannealing"))
		pFitTask->setMethodType(CCopasiMethod::SimulatedAnnealing);
	else
	if (sMethod.toLower() == QString("levenbergmarquardt"))
		pFitTask->setMethodType(CCopasiMethod::LevenbergMarquardt);
	else
	if (sMethod.toLower() == QString("neldermead"))
		pFitTask->setMethodType(CCopasiMethod::NelderMead);
	else
	if (sMethod.toLower() == QString("sres"))
		pFitTask->setMethodType(CCopasiMethod::SRES);
	else
	if (sMethod.toLower() == QString("particleswarm"))
		pFitTask->setMethodType(CCopasiMethod::ParticleSwarm);
	else
	if (sMethod.toLower() == QString("steepestdescent"))
		pFitTask->setMethodType(CCopasiMethod::SteepestDescent);
	else
	if (sMethod.toLower() == QString("randomsearch"))
		pFitTask->setMethodType(CCopasiMethod::RandomSearch);

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
	//assert(pExperiment->getExperimentType() == CCopasiTask::timeCourse);
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
	std::cout <<" num = " << targetSpecies.size() << std::endl;
	for (int i=0; i < targetSpecies.size(); ++i)
	{
		k = targetSpecies[i].first;
		pMetab = targetSpecies[i].second;
		pObjectMap->setRole( k , CExperiment::dependent );
		const CCopasiObject* pParticleReference = pMetab->getObject(CCopasiObjectName("Reference=Concentration"));
		pObjectMap->setObjectCN(k, pParticleReference->getCN());
		std::cout <<" k = " << k << "  => " << pParticleReference->getCN()  << std::endl;
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
		const CCopasiObject * pParameterReference = targetParams[i]->getObject(CCopasiObjectName("Reference=Value"));
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
			std::cout << "result = " << result << "\n";
		}
	}
	catch (...)
	{
		// failed
		std::cout << "failed\n";
		return;
	}
	pFitTask->restore();
	
	//assign optimized values back into the model
	for (int i=0; i < params.rows && i < pFitProblem->getSolutionVariables().size(); ++i)
	{
		double x = pFitProblem->getSolutionVariables()[i];
		cSetValue(model, tc_getRowName(params,i), x);
		tc_setMatrixValue(params, i, 0, x);
		std::cout << tc_getRowName(params,i) << " = " << x << std::endl;
	}
}
*/
/*****************************************************************
   GENETIC ALGORITHM BASED OPTIMIZATION -- HELPER FUNCTIONS
******************************************************************/

typedef GA1DArrayGenome<float> RealGenome;

struct GAData
{
	mu::Parser * parser;
	double * parserValues;
	copasi_model * model;
	tc_matrix * data;
	tc_matrix * params;
};

static void InitializeGenome(GAGenome & x)
{	
	RealGenome & g = (RealGenome &)x;
	GAData * data = (GAData*)(g.geneticAlgorithm()->userData());
	tc_matrix * params = data->params;
	for (int i=0; i < g.size() && i < params->rows; ++i)
		g.gene(i,0) = tc_getMatrixValue(*params,i,1) + mtrand() * (tc_getMatrixValue(*params,i,2) - tc_getMatrixValue(*params,i,1));
}

static float EuclideanDistance(const GAGenome & c1, const GAGenome & c2)
{
  const RealGenome & a = (RealGenome &)c1;
  const RealGenome & b = (RealGenome &)c2;

  float x=0.0;
  for(int i=0; i < b.length() && i < a.length(); ++i)
	  x += (a.gene(i) - b.gene(i))*(a.gene(i) - b.gene(i));

  return (float)(x);
}

static float MatrixDistance(tc_matrix * data1, tc_matrix * data2)
{
  int n = 0;
  float x=0.0, total=0.0;
  for(int i=1; i < data1->cols && i < data2->cols; ++i)
  	for(int j=0; j < data1->rows && j < data2->rows; ++j)
  	{
  	   x = tc_getMatrixValue(*data1,i,j) - tc_getMatrixValue(*data2,i,j);
	   total += (x*x);
	   ++n;
	}

  return total/(float)(n);
}

static float ObjectiveForFittingTimeSeries(GAGenome & x)
{
	RealGenome & g = (RealGenome &)x;
	
	if (!g.geneticAlgorithm())
		return std::numeric_limits<float>::max();
	
	GAData * pData = (GAData*)(g.geneticAlgorithm()->userData());
	copasi_model * model = pData->model;
	tc_matrix * data = pData->data;
	tc_matrix * params = pData->params;
	double retval;
	
	for (int i=0; i < params->rows && i < g.length(); ++i)
		cSetValue( *model, tc_getRowName(*params,i), g.gene(i) );
	
	double start = tc_getMatrixValue(*data, 0, 0),
			   end = tc_getMatrixValue(*data, data->rows-1, 0);
	tc_matrix output = cSimulateDeterministic(*model, start, end, data->rows);
	
	retval = MatrixDistance(data, &output);
	
	tc_deleteMatrix(output);
	
	for (int i=0; i < params->rows; ++i)
	{
		if (g.gene(i) < tc_getMatrixValue(*params, i, 1) || 
			 g.gene(i) > tc_getMatrixValue(*params, i, 2))
		 {
		 	retval = std::numeric_limits<float>::max();
		 	break;
		 }
	}
	return retval;
}

static float ObjectiveForFittingSteadyStateData(GAGenome & x)
{
	RealGenome & g = (RealGenome &)x;
	
	GAData * pData = (GAData*)(g.geneticAlgorithm()->userData());
	copasi_model * model = pData->model;
	tc_matrix * data = pData->data;
	tc_matrix * params = pData->params;
	
	for (int i=0; i < params->rows && i < g.length(); ++i)
		cSetValue( *model, tc_getRowName(*params,i), g.gene(i) );
	
	double retval;

	tc_matrix output = tc_createMatrix(data->rows, data->cols);
	const char * name = tc_getRowName(*data,0);
	
	for (int i=0; i < data->rows; ++i)
	{
		cSetValue(*model, name, tc_getMatrixValue(*data, i, 0));
		tc_setMatrixValue(output, i, 0, tc_getMatrixValue(*data, i, 0));
		tc_matrix ss = cGetSteadyState(*model);
		for (int j=0; j < ss.rows; ++j)
			for (int k=0; k < data->cols; ++k)
				if (QString(tc_getRowName(ss, j)) == QString(tc_getColumnName(*data,k)))
				{
					tc_setMatrixValue(output, i, k+1, tc_getMatrixValue(ss, j, 0));
					break;
				}
		tc_deleteMatrix(ss);
	}
	
	retval = MatrixDistance(data, &output);
	tc_deleteMatrix(output);
	
	for (int i=0; i < params->rows; ++i)
	{
		if (g.gene(i) < tc_getMatrixValue(*params, i, 1) || 
			 g.gene(i) > tc_getMatrixValue(*params, i, 2))
		 {
		 	retval = std::numeric_limits<float>::max();
		 	break;
		 }
	}

	return retval;
}

static float ObjectiveForMaximizingFormula(GAGenome & x)
{
	RealGenome & g = (RealGenome &)x;
	
	GAData * pData = (GAData*)(g.geneticAlgorithm()->userData());
	mu::Parser * parser = pData->parser;
	copasi_model * model = pData->model;
	tc_matrix * data = pData->data;
	tc_matrix * params = pData->params;
	double retval;
	
	for (int i=0; i < params->rows && i < g.length(); ++i)
		cSetValue( *model, tc_getRowName(*params,i), g.gene(i) );
	
	if (parser)
	{
		tc_matrix ss = cGetSteadyState(*model);
		for (int i=0; i < ss.rows; ++i)
			pData->parserValues[i] = tc_getMatrixValue(ss, i, 0);
		
		retval = parser->Eval();
		tc_deleteMatrix(ss);
	}
	else
	{
		retval = std::numeric_limits<float>::max();
	}
	
	for (int i=0; i < params->rows; ++i)
	{
		if (g.gene(i) < tc_getMatrixValue(*params, i, 1) || 
			 g.gene(i) > tc_getMatrixValue(*params, i, 2))
		 {
		 	retval = std::numeric_limits<float>::max();
		 	break;
		 }
	}

	return retval;
}

/**************************************************
   GENETIC ALGORITHM BASED OPTIMIZATION
***************************************************/
static int _OPTIMIZATION_MAX_ITER = 100;
static int _OPTIMIZATION_POPULATION_SIZE = 1000;
static double _OPTIMIZATION_MUTATION_RATE = 0.2;
static double _OPTIMIZATION_CROSSOVER_RATE = 0.8;

tc_matrix cOptimize(copasi_model model, const char * objective, tc_matrix params)
{
	QFile file(objective);
	
	mu::Parser parser;
	GAData pData;
	tc_matrix data;
	
	pData.parser = 0;
	pData.model = &model;
	pData.data = 0;
	pData.params = &params;
	
	QStringList words;
	if (file.open(QFile::ReadOnly | QFile::Text))
	{
		int numlines=0;
		QString delim("\t");
	
		QString line(file.readLine());
		line.remove("#");

		if (line.contains(","))
			delim = QString(",");

		words = line.trimmed().split(delim);
		
		if (!words.isEmpty())
		{
			while (!file.atEnd())
			{
				file.readLine();
				++numlines;
			}
		}
		
		file.close();
		
		if (words.isEmpty() || numlines < 1)
		{
			return tc_createMatrix(0,0);
		}
		else
		{
			if (file.open(QFile::ReadOnly | QFile::Text))
			{
				data = tc_createMatrix(numlines, words.size());
				pData.data = &data;
				QString line(file.readLine());
				int i=0;
				bool ok;
				double d;

				for (i=0; i < words.size(); ++i)
				{
					tc_setColumnName(data, i, words[i].toAscii().data());
				}
				
				i = 0;
				while (!file.atEnd() && i < numlines)
				{
					line = file.readLine();
					words = line.trimmed().split(delim);
					for (int j=0; j < words.size() && j < data.cols; ++j)
					{
						d = words[j].toDouble(&ok);
						if (!ok)
							d = 0.0;
						tc_setMatrixValue(data, i, j, d); //set data
					}
					++i;
				}
				file.close();
			}
		}
	}
	else
	{
		tc_matrix ss = cGetSteadyState(model);
		double * array = new double[ss.rows];
		for (int i=0; i < ss.rows; ++i)
		{
			double * dp = &(array[i]);
			parser.DefineVar(tc_getRowName(ss,i), dp);   //add all the model variables
		}
		pData.parserValues = array;
		parser.SetExpr(objective);
		
		try
		{
			parser.Eval();  //just checking if the eq. is valid
		}
		catch(...)
		{
			delete pData.parserValues;
			return tc_createMatrix(0,0);
		}
	}
	
	GAPopulation pop;	
	
	if (pData.parser)
	{
		RealGenome genome( params.rows , &ObjectiveForMaximizingFormula );
		genome.initializer(&InitializeGenome);
		GASteadyStateGA ga(genome);
		ga.userData(&pData);
		pop = ga.population();
		GASharing dist(EuclideanDistance);
		ga.scaling(dist);
		ga.pReplacement(1.0);
		ga.maximize();
		ga.populationSize(_OPTIMIZATION_POPULATION_SIZE);
		ga.nGenerations(_OPTIMIZATION_MAX_ITER);
		ga.pMutation(_OPTIMIZATION_MUTATION_RATE);
		ga.pCrossover(_OPTIMIZATION_CROSSOVER_RATE);
		ga.initialize();
		int k=0;
		while (ga.done() != gaTrue)
		{
			ga.step();
			//std::cout << "gen " << ++k << "\n";
		}
		//ga.evolve();
		pop = ga.population();
		pop.order(GAPopulation::HIGH_IS_BEST);
		pop.sort(gaTrue);
		delete pData.parserValues;
	}
	else
	if (pData.data)
	{
		if (QString(tc_getColumnName(data,0)).toLower() == QString("time"))
		{
			RealGenome genome( params.rows , &ObjectiveForFittingTimeSeries );
			genome.initializer(&InitializeGenome);
			GASteadyStateGA ga(genome);
			ga.userData(&pData);
			pop = ga.population();
			GASharing dist(EuclideanDistance);
			ga.scaling(dist);
			ga.pReplacement(1.0);
			ga.maximize();
			ga.populationSize(_OPTIMIZATION_POPULATION_SIZE);
			ga.nGenerations(_OPTIMIZATION_MAX_ITER);
			ga.pMutation(_OPTIMIZATION_MUTATION_RATE);
			ga.pCrossover(_OPTIMIZATION_CROSSOVER_RATE);
			ga.minimize();
			ga.initialize();
			int k=0;
			while (ga.done() != gaTrue)
			{
				ga.step();
				std::cout << "\n\ngen " << ++k << "\n";
			}
			pop = ga.population();
			pop.order(GAPopulation::LOW_IS_BEST);
			pop.sort(gaTrue);
		}
		else
		{
			RealGenome genome( params.rows , &ObjectiveForFittingSteadyStateData );
			genome.initializer(&InitializeGenome);
			GASteadyStateGA ga(genome);
			ga.userData(&pData);
			pop = ga.population();
			GASharing dist(EuclideanDistance);
			ga.scaling(dist);
			ga.pReplacement(1.0);
			ga.maximize();
			ga.populationSize(_OPTIMIZATION_POPULATION_SIZE);
			ga.nGenerations(_OPTIMIZATION_MAX_ITER);
			ga.pMutation(_OPTIMIZATION_MUTATION_RATE);
			ga.pCrossover(_OPTIMIZATION_CROSSOVER_RATE);
			ga.minimize();
			ga.initialize();
			int k=0;
			while (ga.done() != gaTrue)
			{
				ga.step();
				std::cout << "gen " << ++k << "\n";
			}
			pop = ga.population();
			pop.order(GAPopulation::LOW_IS_BEST);
			pop.sort(gaTrue);
		}
		tc_deleteMatrix(data);
	}
	else
	{
		return tc_createMatrix(0,0);
	}
	
	tc_matrix result = tc_createMatrix( pop.size(), params.rows );
	
	for (int i=0; i < pop.size(); ++i)
	{
		RealGenome & g = (RealGenome &)(pop.individual(i));
		for (int j=0; j < params.rows; ++j)
			tc_setMatrixValue(result, i, j, g.gene(j));
	}

	return result;
}

void cSetOptimizerSize(int n)
{
	_OPTIMIZATION_POPULATION_SIZE = n;
}

void cSetOptimizerIterations(int n)
{
	_OPTIMIZATION_MAX_ITER = n;
}

void cSetOptimizerMutationRate(double q)
{
	_OPTIMIZATION_MUTATION_RATE = q;
}

void cSetOptimizerCrossoverRate(double q)
{
	_OPTIMIZATION_CROSSOVER_RATE = q;
}

/* LIBSTRUCTURAL */

tc_matrix convertFromDoubleMatrix(DoubleMatrix& matrix, std::vector< std::string > &rowNames, std::vector< std::string > &colNames)
{
	tc_matrix m = tc_createMatrix(matrix.numRows(), matrix.numCols());
	
	for (int i=0; i < m.rows && i < rowNames.size(); ++i)
		tc_setRowName(m, i, rowNames[i].c_str());

	for (int i=0; i < m.cols && i < colNames.size(); ++i)
		tc_setColumnName(m, i, colNames[i].c_str());

	for (int i=0; i < m.rows; ++i)
		for (int j=0; j < m.cols; ++j)
			tc_setMatrixValue(m, i, j, matrix(i,j));
	
	return m;
}

void convertToDoubleMatrix(tc_matrix m, DoubleMatrix & matrix, std::vector< std::string > &rowNames, std::vector< std::string > &colNames)
{
	matrix.resize(m.rows, m.cols);
	
	rowNames.resize(m.rows);
	colNames.resize(m.cols);
	
	for (int i=0; i < m.rows && i < rowNames.size(); ++i)
		rowNames[i] = std::string(tc_getRowName(m, i));

	for (int i=0; i < m.cols && i < colNames.size(); ++i)
		colNames[i] = std::string(tc_getColumnName(m, i));
	
	for (int i=0; i < m.rows; ++i)
		for (int j=0; j < m.cols; ++j)
			matrix(i,j) = tc_getMatrixValue(m, i, j);
}

tc_matrix cGetGammaMatrix(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);

	//get stoichiometry
	tc_matrix tc_N = cGetFullStoichiometryMatrix(model);
	std::vector< std::string > rowNames, colNames;	
	DoubleMatrix N;
	
	convertToDoubleMatrix( tc_N , N, rowNames, colNames );
	
	//use libstructural
	LibStructural * instance = LibStructural::getInstance();
	instance->loadStoichiometryMatrix (N);

	CCopasiVector< CMetab > & metabolites = pModel->getMetabolites();
	std::vector<double> iv(metabolites.size(), 0);  //species concentrations
	for (int i=0; i < metabolites.size(); ++i)
		if (metabolites[i] != NULL)
			iv[i] = metabolites[i]->getInitialConcentration();
	
	instance->loadSpecies (rowNames, iv);
	instance->loadReactionNames (colNames);
	instance->analyzeWithQR();
	
	DoubleMatrix * matrix = instance->getGammaMatrix();
	
	rowNames.clear();
	colNames.clear();
	instance->getGammaMatrixLabels(rowNames, colNames);
	
	//convert
	tc_matrix m = convertFromDoubleMatrix(*matrix, rowNames, colNames);
	
	//cleanup
	//delete matrix;
	tc_deleteMatrix(tc_N);
	
	return m;
}

tc_matrix cGetKMatrix(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);

	//get stoichiometry
	tc_matrix tc_N = cGetFullStoichiometryMatrix(model);
	std::vector< std::string > rowNames, colNames;	
	DoubleMatrix N;
	
	convertToDoubleMatrix( tc_N , N, rowNames, colNames );
	
	//use libstructural
	LibStructural * instance = LibStructural::getInstance();
	instance->loadStoichiometryMatrix (N);

	CCopasiVector< CMetab > & metabolites = pModel->getMetabolites();
	std::vector<double> iv(metabolites.size(), 0);  //species concentrations
	for (int i=0; i < metabolites.size(); ++i)
		if (metabolites[i] != NULL)
			iv[i] = metabolites[i]->getInitialConcentration();
	
	instance->loadSpecies (rowNames, iv);
	instance->loadReactionNames (colNames);
	instance->analyzeWithQR();
	
	DoubleMatrix * matrix = instance->getKMatrix();
	
	rowNames.clear();
	colNames.clear();
	instance->getKMatrixLabels(rowNames, colNames);
	
	//convert
	tc_matrix m = convertFromDoubleMatrix(*matrix, rowNames, colNames);
	
	//cleanup
	//delete matrix;
	tc_deleteMatrix(tc_N);
	
	return m;
}

tc_matrix cGetLinkMatrix(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);

	//get stoichiometry
	tc_matrix tc_N = cGetFullStoichiometryMatrix(model);
	std::vector< std::string > rowNames, colNames;	
	DoubleMatrix N;
	
	convertToDoubleMatrix( tc_N , N, rowNames, colNames );
	
	//use libstructural
	LibStructural * instance = LibStructural::getInstance();
	instance->loadStoichiometryMatrix (N);

	CCopasiVector< CMetab > & metabolites = pModel->getMetabolites();
	std::vector<double> iv(metabolites.size(), 0);  //species concentrations
	for (int i=0; i < metabolites.size(); ++i)
		if (metabolites[i] != NULL)
			iv[i] = metabolites[i]->getInitialConcentration();
	
	instance->loadSpecies (rowNames, iv);
	instance->loadReactionNames (colNames);
	instance->analyzeWithQR();
	
	DoubleMatrix * matrix = instance->getLinkMatrix();
	
	rowNames.clear();
	colNames.clear();
	instance->getLinkMatrixLabels(rowNames, colNames);
	
	//convert
	tc_matrix m = convertFromDoubleMatrix(*matrix, rowNames, colNames);
	
	//cleanup
	//delete matrix;
	tc_deleteMatrix(tc_N);
	
	return m;
}

tc_matrix cGetK0Matrix(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);

	//get stoichiometry
	tc_matrix tc_N = cGetFullStoichiometryMatrix(model);
	std::vector< std::string > rowNames, colNames;	
	DoubleMatrix N;
	
	convertToDoubleMatrix( tc_N , N, rowNames, colNames );
	
	//use libstructural
	LibStructural * instance = LibStructural::getInstance();
	instance->loadStoichiometryMatrix (N);

	CCopasiVector< CMetab > & metabolites = pModel->getMetabolites();
	std::vector<double> iv(metabolites.size(), 0);  //species concentrations
	for (int i=0; i < metabolites.size(); ++i)
		if (metabolites[i] != NULL)
			iv[i] = metabolites[i]->getInitialConcentration();
	
	instance->loadSpecies (rowNames, iv);
	instance->loadReactionNames (colNames);
	instance->analyzeWithQR();
	
	DoubleMatrix * matrix = instance->getK0Matrix();
	
	rowNames.clear();
	colNames.clear();
	instance->getK0MatrixLabels(rowNames, colNames);
	
	//convert
	tc_matrix m = convertFromDoubleMatrix(*matrix, rowNames, colNames);
	
	//cleanup
	//delete matrix;
	tc_deleteMatrix(tc_N);
	
	return m;
}

tc_matrix cGetL0Matrix(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);

	//get stoichiometry
	tc_matrix tc_N = cGetFullStoichiometryMatrix(model);
	std::vector< std::string > rowNames, colNames;	
	DoubleMatrix N;
	
	convertToDoubleMatrix( tc_N , N, rowNames, colNames );
	
	//use libstructural
	LibStructural * instance = LibStructural::getInstance();
	instance->loadStoichiometryMatrix (N);

	CCopasiVector< CMetab > & metabolites = pModel->getMetabolites();
	std::vector<double> iv(metabolites.size(), 0);  //species concentrations
	for (int i=0; i < metabolites.size(); ++i)
		if (metabolites[i] != NULL)
			iv[i] = metabolites[i]->getInitialConcentration();
	
	instance->loadSpecies (rowNames, iv);
	instance->loadReactionNames (colNames);
	instance->analyzeWithQR();
	
	DoubleMatrix * matrix = instance->getL0Matrix();
	
	rowNames.clear();
	colNames.clear();
	instance->getL0MatrixLabels(rowNames, colNames);
	
	//convert
	tc_matrix m = convertFromDoubleMatrix(*matrix, rowNames, colNames);
	
	//cleanup
	//delete matrix;
	tc_deleteMatrix(tc_N);
	
	return m;
}


