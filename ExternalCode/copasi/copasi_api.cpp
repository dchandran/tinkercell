//std
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <string>
#include <sstream>
#include <map>
#include <limits> //get max and min for double

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

//regex
#include "boost/regex.hpp"

//parse math (used for optimization)
#include "muParserDef.h"
#include "muParser.h"
#include "muParserInt.h"
extern "C"
{
	#include "mtrand.h"
}

//Antimony lib
extern "C"
{
	#define LIB_EXPORTS 1
	#include "src/antimony_api.h"
}

using namespace LIB_STRUCTURAL;
using namespace LIB_LA;
using namespace std;

struct CopasiPtr 
{ 
	string name;
	string key;
	CMetab * species; 
	CCompartment * compartment;
	CReaction * reaction;
	CModelValue * param;
	string assignmentRule;
};

typedef map< string, CopasiPtr > CCMap;
static int rename(string& target, const string& oldname,const string& newname0);
static int replaceSubstring(string& target, const string& oldname,const string& newname0);
static list< CCMap* > hashTablesToCleanup;
static list< copasi_model > copasiModelsToCleanup;

static boost::regex stupidPowFunction("pow\\s*\\(\\s*([^,]+)\\s*,\\s*([^,]+)\\s*\\)", boost::regex::perl);
//static RegExp stupidPowFunction("pow\\s*\\(\\s*([^,]+)\\s*,\\s*([^,]+)\\s*\\)");

bool contains(CCMap * hash, const string & s)
{
	return hash && (hash->find(s) != hash->end());
}

bool contains(const string& str, const string & s)
{
	return str.find(s) != string::npos;
}

CopasiPtr getHashValue(CCMap * hash, const string & s)
{
	return (*hash)[s];
}

void hashInsert(CCMap * hash, const string & s, CopasiPtr v)
{
	(*hash)[s] = v;
}

int indexOf( list<string>& lst, const string & s)
{
	int k = 0;
	for (list<string>::iterator i=lst.begin(); i != lst.end(); i++, ++k)
		if ((*i) == s)
			return k;
	return -1;
}

int indexOf( vector<string>& lst, const string & s)
{
	int k = 0;
	for (vector<string>::iterator i=lst.begin(); i != lst.end(); i++, ++k)
		if ((*i) == s)
			return k;
	return -1;
}

double string_to_double( const std::string& s )
{
	std::istringstream i(s);
	double x;
	if (!(i >> x))
		return 0;
	return x;
} 

list<string> splitString(const string& seq, const string& _1cdelim);

void copasi_init()
{
	CCopasiRootContainer::init(0, NULL);
}

void copasi_end()
{
	for (list<CCMap*>::iterator i = hashTablesToCleanup.begin(); i != hashTablesToCleanup.end(); i++)
		delete (*i);

	list< copasi_model > models = copasiModelsToCleanup;
	copasiModelsToCleanup.clear();

	for (list<copasi_model>::iterator i = models.begin(); i != models.end(); i++)
		cRemoveModel(*i);

	CCopasiRootContainer::destroy();
}

int cSetAssignmentRuleHelper(copasi_model , CMetab * , const char * );

int copasi_cleanup_assignments(copasi_model model, bool doWhile=false)
{
	CCMap * hash = (CCMap*)(model.qHash);
	if (!hash) return 0;
	
	CMetab* pSpecies = 0;
	vector<string> names, assignments;
	names.reserve(hash->size());
	assignments.reserve(hash->size());

	for (CCMap::iterator i = hash->begin(); i != hash->end(); i++)
		if ( (*i).second.species && !(*i).second.assignmentRule.empty())
		{
			names.push_back( (*i).first );
			assignments.push_back( (*i).second.assignmentRule );
		}
		else
		{
			names.push_back( string() );
			assignments.push_back( string() );
		}

	int retval = 1;
	bool replace_needed = doWhile;
	
	while (replace_needed)
	{
		replace_needed = false;
		for (CCMap::iterator i = hash->begin(); i != hash->end(); i++)
			if ((*i).second.species && !(*i).second.assignmentRule.empty())
			{
				for (int j=0; j < names.size(); ++j)
					if (!names[j].empty() && 
						names[j] != (*i).first && 
						contains((*i).second.assignmentRule, names[j]))
					{
						if (rename((*i).second.assignmentRule, names[j], assignments[j]))
							replace_needed = true;
					}
			}
	}

	for (CCMap::iterator i = hash->begin(); i != hash->end(); i++)
		if ((*i).second.species && !(*i).second.assignmentRule.empty())
		{
			retval = retval * cSetAssignmentRuleHelper(model, (*i).second.species, (*i).second.assignmentRule.c_str());
		}
	return retval;
}

void cRemoveModel(copasi_model model)
{
	//remove from list
	for (list<copasi_model>::iterator i=copasiModelsToCleanup.begin(); i != copasiModelsToCleanup.end(); i++)
		if ((*i).CopasiDataModelPtr == model.CopasiDataModelPtr)
		{
			copasi_model m = { (void*)NULL, (void*)NULL, (void*)NULL, (char*)NULL };
			(*i) = m;
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
	CCMap * hash = (CCMap*)(model.qHash);
	
	if (!pModel || !pDataModel || !hash) return;
	
	CopasiPtr p;

	for (CCMap::iterator i = hash->begin(); i != hash->end(); i++)
	{
		p = (*i).second;
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
	CCMap * qHash = new CCMap();
	copasi_model m = { (void*)(pModel) , (void*)(pDataModel), (void*)(qHash), (char*)(NULL) };
	
	hashTablesToCleanup.push_back(qHash);
	copasiModelsToCleanup.push_back(m);

	pModel->setSBMLId( string(name) );
	pModel->setObjectName( string(name) );
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
	CCMap * hash = (CCMap*)(compartment.qHash);
	CMetab* pSpecies;
	
	if (!pModel || !hash || !pCompartment) return;	
	if (contains(hash, string(name)))
	{
		pSpecies = getHashValue(hash,string(name)).species;
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
			pSpecies->getCN(),
			pSpecies->getKey(),
			pSpecies,
			0,
			0,
			0};

	hashInsert(hash, 
				(pCompartment->getObjectName() + string("_") + string(name)),
				copasiPtr
				);

	hashInsert(hash, string(name), copasiPtr); //for speedy lookup
}

copasi_compartment cCreateCompartment(copasi_model model, const char* name, double volume)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCompartment* pCompartment = pModel->createCompartment(name, volume);
	CCMap * hash = (CCMap*)(model.qHash);
	copasi_compartment c = { 0, 0 , 0};
	
	if (!pModel || !hash) return c;
	c.CopasiModelPtr = (void*)(pModel);
	c.qHash = model.qHash;
	
	if (contains(hash,string(name)))
	{
		if (getHashValue(hash, string(name)).compartment)
			c.CopasiCompartmentPtr = (void*)(getHashValue(hash, string(name)).compartment);
		
		return c;
	}
	else
	{
		c.CopasiCompartmentPtr = (void*)(pCompartment);
	}	
	
	CopasiPtr copasiPtr = { 
			pCompartment->getCN(),
			pCompartment->getKey(),
			0,
			pCompartment,
			0,
			0};

	hashInsert(hash, string(name),copasiPtr); //for speedy lookup
	
	return c;
}

int cSetValue(copasi_model model, const char * name, double value)
{
	CCMap * hash = (CCMap*)(model.qHash);
	string s(name);
	
	if (!hash) return 0;
	
	if (!contains(hash,s))
	{
		cSetGlobalParameter(model,name,value);
		return 0;
	}

	CopasiPtr p = getHashValue(hash,s);
	
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
	CCMap * hash = (CCMap*)(model.qHash);
	string s(name);
	CCompartment* pVol = NULL;
	
	if (!hash) return;
	
	if (contains(hash,s) && 
		(pVol = getHashValue(hash,s).compartment))
	{
		pVol->setInitialValue(vol);
		pVol->setValue(vol);
	}
}

void cSetConcentration(copasi_model model, const char * name, double conc)
{
	CCMap * hash = (CCMap*)(model.qHash);
	string s(name);
	CMetab* pSpecies = NULL;
	
	if (!hash) return;
	
	if (contains(hash,s) && 
		(pSpecies = getHashValue(hash,s).species))
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
	CCMap * hash = (CCMap*)(model.qHash);
	string s(name);
	CModelValue * pValue = NULL;
	
	if (!hash || !pModel) return 0;
	
	if (contains(hash,s) && 
		(pValue = getHashValue(hash,s).param))
	{
		pValue->setInitialValue(value);
		pValue->setValue(value);
		return 1;
	}

	//parameter not found, so create it
	if (!pValue)
	{
		pValue = pModel->createModelValue(string(name),value);
		pValue->setInitialValue(value);
	
		CopasiPtr copasiPtr = {
				pValue->getCN(),
				pValue->getKey(),
				0,
				0,
				0,
				pValue};

		hashInsert(hash, s, copasiPtr); //for speedy lookup
	}
	
	return 0;
}

void cSetSpeciesType(copasi_model model, const char * name, int isBoundary)
{
	CCMap * hash = (CCMap*)(model.qHash);
	string s(name);
	CMetab* pSpecies = NULL;
	
	if (!hash) return;
	
	if (contains(hash,s) && 
		(pSpecies = getHashValue(hash,s).species))
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
	CCMap * hash = (CCMap*)(model.qHash);
	string s(name);
	int i;
	bool retval=true;
	
	if (!pModel || !hash) return 0;
	
	if (!contains(hash,s))
	{
		CCopasiVectorNS < CCompartment > & compartments = pModel->getCompartments();
		if (compartments.size() > 0 && compartments[0] != NULL)
		{
			CCompartment* pCompartment = compartments[0];
			if (pCompartment)
			{
				string s(pCompartment->getObjectName());
				if	(contains(hash,s) &&
					getHashValue(hash,s).compartment)
					{
						copasi_compartment c = { (void*)getHashValue(hash,s).compartment, model.CopasiModelPtr, model.qHash };
						cCreateSpecies(c,name,0.0);
					}
			}
		}
	}

	if (contains(hash,s) && getHashValue(hash,s).species)
	{
		CopasiPtr & p = (*hash)[s];
		p.assignmentRule = string(formula);
		boost::regex_replace(p.assignmentRule, stupidPowFunction, string("((\\1)^(\\2))"));
		//cout << p.assignmentRule.c_str() << "\n";
		return 1;
	}
	return 0;
}

int cSetAssignmentRuleHelper(copasi_model model, CMetab* pSpecies, const char * formula)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCMap * hash = (CCMap*)(model.qHash);
	int i;
	bool retval=true;
	
	if (!pModel || !hash || !pSpecies) return 0;
	
	if (formula)
	{
		pSpecies->setStatus(CModelEntity::ASSIGNMENT);
		CFunction pFunction;
		string qFormula(formula);
		if (pFunction.setInfix(string(formula)))
		{
			CFunctionParameters& variables = pFunction.getVariables();
			CFunctionParameter* pParam;

			for (i=0; i < variables.size(); ++i)
			{
				pParam = variables[i];

				string s0(pParam->getObjectName());
				
				if (s0 == string("time") || 
					  s0 == string("Time") ||
			     	  s0 == string("TIME"))
				{
					string s1("<");
						s1 += pModel->getValueReference()->getCN();
						s1 += string(">");
					rename(qFormula,s0,s1);
				}
				else
				{
					if (!contains(hash,s0))
						cSetGlobalParameter(model,pParam->getObjectName().c_str(),1.0);
					if (contains(hash,s0))
					{
					 	string s1("<");
							s1 += getHashValue(hash,s0).name;
							s1 += string(">");
						//std::cout << "sub " << s1 << "  for " << s0 << "\n";
						rename(qFormula,s0,s1);
					}
				}
			}
		}

		//std::cout << qFormula << "\n";
		retval = retval & pSpecies->setExpression(qFormula);
	}
	else
		pSpecies->setStatus(CModelEntity::REACTIONS);
	
	return (int)retval;
}

int cCreateVariable(copasi_model model, const char * name, const char * formula)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCMap * hash = (CCMap*)(model.qHash);
	
	if (!hash || !pModel) return 0;

	CModelValue* pModelValue;
	string qname(name);

	if (contains(hash,qname))
	{
			CopasiPtr ptr = getHashValue(hash,qname);
			if (ptr.species)
				return cSetAssignmentRule(model, name, formula);
			if (ptr.param)
				pModelValue = ptr.param;
			else
				return 0;	
	}
	else
	{
		pModelValue = pModel->createModelValue(string(name), 0.0);
	}
	pModelValue->setStatus(CModelValue::ASSIGNMENT);
	int i;
	bool retval = true;

	CFunction pFunction;
	string qFormula(formula);

	if (pFunction.setInfix(string(formula)))
	{
		CFunctionParameters& variables = pFunction.getVariables();
		CFunctionParameter* pParam;

		for (i=0; i < variables.size(); ++i)
		{
			pParam = variables[i];

			string s0(pParam->getObjectName());
			if (s0 == string("time") || 
				  s0 == string("Time") ||
		     	  s0 == string("TIME"))
			{
				string s1("<");
					s1 += pModel->getValueReference()->getCN();
					s1 += string(">");
				rename(qFormula,s0,s1);
			}
			else
			{
				if (!contains(hash,s0))
					cSetGlobalParameter(model,pParam->getObjectName().c_str(),1.0);				
				
				if (contains(hash,s0))
				{
				 	string s1("<");
						s1 += getHashValue(hash,s0).name;
						s1 += string(">");
					rename(qFormula,s0,s1);
				}
			}
		}
	}

	string sFormula( qFormula );
	
	retval = retval & pModelValue->setInitialExpression(sFormula);
	retval = retval & pModelValue->setExpression(sFormula);
	
	CopasiPtr copasiPtr = {
			pModelValue->getCN(),
			pModelValue->getKey(),
			0,
			0,
			0,
			pModelValue};

	hashInsert(hash, qname, copasiPtr); //for speedy lookup
	
	return (int)retval;
}

int cCreateEvent(copasi_model model, const char * name, const char * trigger, const char * variable, const char * formula)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCMap * hash = (CCMap*)(model.qHash);
	
	if (!hash || !pModel)
	{
		return 0;
	}
	
	int i;
	bool retval = true;

	if (!contains(hash,string(variable)))
	{
		cSetGlobalParameter(model,variable,1.0);
	}
	
	if (!contains(hash,string(variable))) return 0;

	CopasiPtr ptr = getHashValue(hash,string(variable));
	
	if (!ptr.species && !ptr.param) return 0;

	CEvent * pEvent = pModel->createEvent(string(name));

	CFunction pFunction;
	string qFormula(trigger);
	replaceSubstring(qFormula,">","gt");
	replaceSubstring(qFormula,"<","lt");
	replaceSubstring(qFormula,">=","ge");
	replaceSubstring(qFormula,"<=","le");
	replaceSubstring(qFormula,"=","eq");

	if (pFunction.setInfix(qFormula))  //parse trigger
	{
		CFunctionParameters& variables = pFunction.getVariables();
		CFunctionParameter* pParam;

		for (i=0; i < variables.size(); ++i)
		{
			pParam = variables[i];

			string s0(pParam->getObjectName());
			if (s0 == string("time") || 
				  s0 == string("Time") ||
		     	  s0 == string("TIME"))
			{
				string s1("<");
					s1 += pModel->getValueReference()->getCN();
					s1 += string(">");
				rename(qFormula,s0,s1);
			}
			else
			{
				if (!contains(hash,s0))
					cSetGlobalParameter(model,s0.c_str(),1.0);
				if (contains(hash,s0))
				{
				 	string s1("<");
						s1 += getHashValue(hash,s0).name;
						s1 += string(">");
					rename(qFormula,s0,s1);
				}
			}
		}
	}
	else
	{
		retval = false;
	}
	
	CExpression * expression = new CExpression(name,pModel);
	retval = retval & expression->setInfix(qFormula);
	pEvent->setTriggerExpressionPtr(expression);   //set trigger
	
	qFormula = string(formula);
	
	if (pFunction.setInfix(string(formula)))   //parse response expression
	{
		CFunctionParameters& variables = pFunction.getVariables();
		CFunctionParameter* pParam;

		for (i=0; i < variables.size(); ++i)
		{
			pParam = variables[i];

			string s0(pParam->getObjectName().c_str());
			if (s0 == string("time") || 
				  s0 == string("Time") ||
			 	  s0 == string("TIME"))
			{
				string s1("<");
					s1 += pModel->getValueReference()->getCN();
					s1 += string(">");
				rename(qFormula,s0,s1);
			}
			else
			{
				if (!contains(hash,s0))
					cSetGlobalParameter(model,s0.c_str(),1.0);

				if (contains(hash,s0))
				{
				 	string s1("<");
						s1 += getHashValue(hash,s0).name;
						s1 += string(">");
					rename(qFormula,s0,s1);
				}
			}
		}
	}
	else
	{
		return 0;
	}
	
	CCopasiVectorN< CEventAssignment > & assignments = pEvent->getAssignments();
	CEventAssignment * assgn = new CEventAssignment(string(name) + string("_assgn"),pModel);
	if (ptr.species)
		retval = retval & assgn->setTargetKey(ptr.species->getKey());   //set target
	else
		retval = retval & assgn->setTargetKey(ptr.param->getKey());

	expression = new CExpression(name,pModel);
	retval = retval & expression->setInfix(qFormula);
	assgn->setExpressionPtr(  expression );   //set expression
	assignments.add(assgn); 
	
	return (int)retval;
}

copasi_reaction cCreateReaction(copasi_model model, const char* name)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCMap * hash = (CCMap*)(model.qHash);
	
	if (!pModel || !hash)
	{
		copasi_reaction r = { 0, 0, 0 };
		return r;
	}
	
	CReaction* pReaction = pModel->createReaction(name);
	
	copasi_reaction r = { (void*)(pReaction), (void*)(pModel), (void*)hash };
	
	CopasiPtr copasiPtr = { 
			pReaction->getCN(),
			pReaction->getKey(),
			0,
			0,
			pReaction,
			0};

	string qname(name);
	hashInsert(hash, qname, copasiPtr); //for speedy lookup

	return r;
}

void cAddReactant(copasi_reaction reaction, const char * species, double stoichiometry)
{
	CReaction* pReaction = (CReaction*)(reaction.CopasiReactionPtr);
	CCMap * hash = (CCMap*)(reaction.qHash);
	
	if (!pReaction || !hash)
	{
		return;
	}

	CMetab* pSpecies = NULL;
	
	string s(species);
	if (contains(hash,s) && (pSpecies = getHashValue(hash,s).species))
	{
		CChemEq* pChemEq = &pReaction->getChemEq();
		pChemEq->addMetabolite(pSpecies->getKey(), stoichiometry, CChemEq::SUBSTRATE);
	}
}

void cAddProduct(copasi_reaction reaction, const char * species, double stoichiometry)
{
	CReaction* pReaction = (CReaction*)(reaction.CopasiReactionPtr);
	CCMap * hash = (CCMap*)(reaction.qHash);
	CMetab* pSpecies = NULL;
	
	if (!pReaction || !hash) return;
	
	string s(species);
	if (contains(hash,s) && (pSpecies = getHashValue(hash,s).species))
	{
		CChemEq* pChemEq = &pReaction->getChemEq();
		pChemEq->addMetabolite(pSpecies->getKey(), stoichiometry, CChemEq::PRODUCT);
	}
}

int cSetReactionRate(copasi_reaction reaction, const char * formula)
{
	int i,j,k;
	CReaction* pReaction = (CReaction*)(reaction.CopasiReactionPtr);
	CCMap * hash = (CCMap*)(reaction.qHash);
	CModel* pModel = (CModel*)(reaction.CopasiModelPtr);
	CFunctionDB* pFunDB = CCopasiRootContainer::getFunctionList();
	
	if (!pReaction || !pModel || !hash) return 0;
	
	if (pFunDB)
	{
		string rateLawName(pReaction->getObjectName() + string("_rate_law")); //existing rate law
		
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

		string formula2(formula);
		//formula2.replace(stupidPowFunction, QString("((\\1)^(\\2))"));
		boost::regex_replace(formula2, stupidPowFunction, string("((\\1)^(\\2))"));
		
		if (pFunction->setInfix(string(formula2.c_str())))
		{
			retval = (int)(pReaction->setFunction(pFunction));
			CFunctionParameters& variables = pFunction->getVariables();
			CFunctionParameter* pParam;

			for (i=0; i < variables.size(); ++i)
			{
				pParam = variables[i];
				
				string s(pParam->getObjectName().c_str());
				
				if (s == string("Time") || s == string("TIME") )
					s = string("time");
				
				if (!contains(hash,s))
				{
					copasi_model model = { (void*)(pModel) , (void*)(0), (void*)(hash) };
					cSetGlobalParameter(model,pParam->getObjectName().c_str(),1.0);				
				}
				
				if (contains(hash,s))
				{
					CopasiPtr p = getHashValue(hash,s);
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
	set<const CCopasiObject*> changedObjects;
	
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
	vector<Refresh*> refreshes = pModel->buildInitialRefreshSequence(changedObjects);
	
	vector<Refresh*>::iterator it2 = refreshes.begin(), endit2 = refreshes.end();
	
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
			cerr << "Error. Running the simulation failed." << endl;
			// check if there are additional error messages
			if (CCopasiMessage::size() > 0)
			{
				// print the messages in chronological order
				cerr << CCopasiMessage::getAllMessageText(true);
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
		list<string> colnames;

		for (j=1; j < cols; ++j)
			colnames.push_back( timeSeries.getTitle(j) );

		colnames.sort();
		colnames.push_front(timeSeries.getTitle(0).c_str());

		j = 0;
		for (list<string>::iterator it=colnames.begin(); j < cols && it != colnames.end(); ++j, it++)
			tc_setColumnName( output, j, (*it).c_str() );
	
		for (j=0; j < cols; ++j)
		{
			k = indexOf(colnames,timeSeries.getTitle(j));
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

void cWriteAntimonyFile(copasi_model model, const char * filename)
{
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	if (pDataModel)
	{
		pDataModel->exportSBML(filename, true, 2, 3);
		loadSBMLFile(filename);
		writeAntimonyFile(filename,NULL);
	}
}

copasi_model cReadAntimonyString(const char * model)
{
	loadString(model); //load Antimony
	const char * s = getSBMLString("__main");  //Antimony -> SBML (at worst, an empty model)
	copasi_model m = cReadSBMLString(s);
	freeAll(); //free Antimony
	return m;
}

copasi_model cReadAntimonyFile(const char * filename)
{
	loadFile(filename); //load Antimony
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
	CCMap * qHash = 0;	
	char * error = NULL;
	string s;
	try 
	{
		pDataModel->importSBML(filename); //SBML -> COPASI
		s = CCopasiMessage::getAllMessageText();
		pModel = pDataModel->getModel();
		qHash = new CCMap();	
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
		hashTablesToCleanup.push_back( qHash );
		copasiModelsToCleanup.push_back(m);
	}
	return m;
}

copasi_model cReadSBMLString(const char * sbml)
{
	copasi_init();
	
	CCopasiDataModel* pDataModel = CCopasiRootContainer::addDatamodel();
	CModel* pModel = 0;
	CCMap * qHash = 0;	
	char * error = NULL;
	string s;
	try 
	{
		pDataModel->importSBMLFromString(sbml); //SBML -> COPASI	
		s = CCopasiMessage::getAllMessageText();
		pModel = pDataModel->getModel();
		qHash = new CCMap();	
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
		hashTablesToCleanup.push_back(qHash);
		copasiModelsToCleanup.push_back(m);
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
		cerr << "Error when computing steady state." << endl;
		return tc_createMatrix(0,0);
	}
	
	const CArrayAnnotation* pAJ = pTask->getJacobianAnnotated();
	//const CEigen & cGetEigenvalues() const;
	
	if (pAJ && pAJ->dimensionality() == 2)
	{
		vector<unsigned int> index(2);
		const vector<string>& annotations = pAJ->getAnnotationsString(1);
		
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

tc_matrix cGetSteadyStateUsingSimulation(copasi_model model, int maxiter)
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
			    cerr << CCopasiMessage::getAllMessageText(true);
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
				list<string> colnames;
				for (int i=0; i < cols; ++i)
                	colnames.push_back( timeSeries.getTitle(i+1) );
				colnames.sort();
                tc_matrix output = tc_createMatrix(cols, 1);
				int k;
				list<string>::iterator it=colnames.begin();
                for (int i=0; i < cols && it != colnames.end(); ++i, it++)
                {
					k = indexOf(colnames, timeSeries.getTitle(i+1));
            		tc_setRowName( output, i, (*it).c_str() );
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
	CTrajectoryTask* pTrajTask = dynamic_cast<CTrajectoryTask*>(TaskList["Time-Course"]);
	// if there isn’t one
	if (pTrajTask == NULL)
	{
		// create a new one
		pTrajTask = new CTrajectoryTask();
		// remove any existing trajectory task just to be sure since in
		// theory only the cast might have failed above
		TaskList.remove("Time-Course");
		// add the new time course task to the task list
		TaskList.add(pTrajTask, true);
	}
	
	CCopasiMessage::clearDeque();
	
	if (pTrajTask && pTrajTask->setMethodType(CCopasiMethod::deterministic))
	{
		//set the start and end time, number of steps, and save output in memory
		CTrajectoryProblem* pProblem=(CTrajectoryProblem*)pTrajTask->getProblem();
		pProblem->setModel(pModel);
		pTrajTask->setScheduled(true);
		pProblem->setStepNumber(10);
		pProblem->setDuration(10.0);
		pDataModel->getModel()->setInitialTime(0.0);
		pProblem->setTimeSeriesRequested(true);
		try
		{
			pTrajTask->initialize(CCopasiTask::ONLY_TIME_SERIES, pDataModel, NULL);
			pTrajTask->process(true);
		}
		catch(...)
		{
			cerr << CCopasiMessage::getAllMessageText(true);
			pTrajTask = NULL;
		}
	}

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
		cerr << "Error when computing steady state." << endl;
		return tc_createMatrix(0,0);
	}

	pTrajTask = dynamic_cast<CTrajectoryTask*>(TaskList["Time-Course"]);
	// if there isn’t one
	if (pTrajTask == NULL)
	{
		pTrajTask = new CTrajectoryTask();
		TaskList.remove("Time-Course");
		TaskList.add(pTrajTask, true);
	}
	
	CCopasiMessage::clearDeque();
	
	if (pTrajTask && pTrajTask->setMethodType(CCopasiMethod::deterministic))
	{
		//set the start and end time, number of steps, and save output in memory
		CTrajectoryProblem* pProblem=(CTrajectoryProblem*)pTrajTask->getProblem();
		pProblem->setModel(pModel);
		pTrajTask->setScheduled(true);
		pProblem->setStepNumber(10);
		pProblem->setDuration(10.0);
		pDataModel->getModel()->setInitialTime(0.0);
		pProblem->setTimeSeriesRequested(true);
		try
		{
			pTrajTask->initialize(CCopasiTask::ONLY_TIME_SERIES, pDataModel, NULL);
			pTrajTask->process(true);
			//pTrajTask->restore();
		}
		catch(...)
		{
			cerr << CCopasiMessage::getAllMessageText(true);
			pTrajTask = NULL;
		}
	}
	
	if (pTrajTask)
	{
		const CTimeSeries & timeSeries = pTrajTask->getTimeSeries();
		int rows = (pModel->getNumMetabs());
		int i,j,k;

		tc_matrix output = tc_createMatrix(rows, 1);

		list<string> rownames;
		for (i=0; i < rows; ++i)
        	rownames.push_back( timeSeries.getTitle(i+1) );
		rownames.sort();
		j = timeSeries.getRecordedSteps() - 1;	

		list<string>::iterator it=rownames.begin();
        for (i=0; i < rows && it != rownames.end(); ++i, it++)
        {
			k = indexOf(rownames ,  timeSeries.getTitle(i+1) );
    		tc_setRowName( output, i, (*it).c_str() );
            tc_setMatrixValue( output, k, 0, timeSeries.getConcentrationData(j,i+1) );
        }

		return output;
	}

	return cGetSteadyStateUsingSimulation(model,10);
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
		cerr << "Error when computing steady state." << endl;
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
		cerr << "Error when performing MCA" << endl;
		return tc_createMatrix(0,0);
	}
	
	CMCAMethod * mcaMethod = dynamic_cast<CMCAMethod*>(pTask->getMethod());
	
	if (!mcaMethod) return tc_createMatrix(0,0);
	
	const CMatrix<C_FLOAT64> & cmatrix = mcaMethod->getUnscaledElasticities();
	const CArrayAnnotation * annot = mcaMethod->getUnscaledElasticitiesAnn();
	const vector<string>& rownames = annot->getAnnotationsString(1),
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
		cerr << "Error when performing MCA" << endl;
		return tc_createMatrix(0,0);
	}
	
	CMCAMethod * mcaMethod = dynamic_cast<CMCAMethod*>(pTask->getMethod());
	
	if (!mcaMethod) return tc_createMatrix(0,0);
	
	const CMatrix<C_FLOAT64> & cmatrix = mcaMethod->getUnscaledConcentrationCC();
	const CArrayAnnotation * annot = mcaMethod->getUnscaledConcentrationCCAnn();
	const vector<string>& rownames = annot->getAnnotationsString(1),
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
		cerr << "Error when performing MCA" << endl;
		return tc_createMatrix(0,0);
	}
	
	CMCAMethod * mcaMethod = dynamic_cast<CMCAMethod*>(pTask->getMethod());
	
	if (!mcaMethod) return tc_createMatrix(0,0);
	
	const CMatrix<C_FLOAT64> & cmatrix = mcaMethod->getUnscaledFluxCC();
	const CArrayAnnotation * annot = mcaMethod->getUnscaledFluxCCAnn();
	const vector<string>& rownames = annot->getAnnotationsString(1),
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
		cerr << "Error when performing MCA" << endl;
		return tc_createMatrix(0,0);
	}
	
	CMCAMethod * mcaMethod = dynamic_cast<CMCAMethod*>(pTask->getMethod());
	
	if (!mcaMethod) return tc_createMatrix(0,0);
	
	const CMatrix<C_FLOAT64> & cmatrix = mcaMethod->getScaledElasticities();
	const CArrayAnnotation * annot = mcaMethod->getScaledElasticitiesAnn();
	const vector<string>& rownames = annot->getAnnotationsString(1),
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
		cerr << "Error when performing MCA" << endl;
		return tc_createMatrix(0,0);
	}
	
	CMCAMethod * mcaMethod = dynamic_cast<CMCAMethod*>(pTask->getMethod());
	
	if (!mcaMethod) return tc_createMatrix(0,0);
	
	const CMatrix<C_FLOAT64> & cmatrix = mcaMethod->getScaledConcentrationCC();
	const CArrayAnnotation * annot = mcaMethod->getScaledConcentrationCCAnn();
	const vector<string>& rownames = annot->getAnnotationsString(1),
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
		cerr << "Error when performing MCA" << endl;
		return tc_createMatrix(0,0);
	}
	
	CMCAMethod * mcaMethod = dynamic_cast<CMCAMethod*>(pTask->getMethod());
	
	if (!mcaMethod) return tc_createMatrix(0,0);
	
	const CMatrix<C_FLOAT64> & cmatrix = mcaMethod->getScaledFluxCC();
	const CArrayAnnotation * annot = mcaMethod->getScaledFluxCCAnn();
	const vector<string>& rownames = annot->getAnnotationsString(1),
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

static int rename(string& target, const string& oldname,const string& newname0)
{
	if (oldname == newname0 || target == newname0) return 0;
	string newname(newname0);

	boost::regex_replace(newname, boost::regex("[^A-Za-z0-9_]",boost::regex::perl), string("_@@@_"));
	//newname.replace(QRegExp("[^A-Za-z0-9_]"),QString("_@@@_"));

	boost::regex regexp1(string("^") + oldname + string("$"),boost::regex::perl),  //just old name
		regexp2(string("^") + oldname + string("([^A-Za-z0-9_\\]]).*"),boost::regex::perl),  //oldname+(!letter/num)
		regexp3(string(".*([^A-Za-z0-9_\\.=\\[])") + oldname + string("$"),boost::regex::perl), //(!letter/num)+oldname
		regexp4(string(".*([^A-Za-z0-9_\\.=\\[])") + oldname + string("([^A-Za-z0-9_\\]]).*"),boost::regex::perl); //(!letter/num)+oldname+(!letter/num)

	boost::regex regexp1r(string("^") + oldname + string("$"),boost::regex::perl),  //just old name
		regexp2r(string("^") + oldname + string("([^A-Za-z0-9_\\]])"),boost::regex::perl),  //oldname+(!letter/num)
		regexp3r(string("([^A-Za-z0-9_\\.=\\[])") + oldname + string("$"),boost::regex::perl), //(!letter/num)+oldname
		regexp4r(string("([^A-Za-z0-9_\\.=\\[])") + oldname + string("([^A-Za-z0-9_\\]])"),boost::regex::perl); //(!letter/num)+oldname+(!letter/num)

	int retval = 0;

	if (boost::regex_match(target, regexp1))
	{
		retval = 1;
		target = newname;
	}

	while (boost::regex_match(target, regexp2))
	{
		retval = 1;
		target = boost::regex_replace(target, regexp2r,newname+string("\\1"));
	}

	while (boost::regex_match(target, regexp3))
	{
		retval = 1;
		target = boost::regex_replace(target, regexp3r, string("\\1")+newname);
	}

	while (boost::regex_match(target, regexp4))
	{
		retval = 1;
		target = boost::regex_replace(target, regexp4r, string("\\1")+newname+string("\\2"));
	}

	target = boost::regex_replace(target, boost::regex(newname),newname0);
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
		cerr << "Error when computing EFM" << endl;
		return tc_createMatrix(0,0);
	}
	
	const vector< CFluxMode > & fluxModes = pTask->getFluxModes();
	CEFMProblem* pProblem = dynamic_cast<CEFMProblem*>(pTask->getProblem());
	
	if (!pProblem)
		return tc_createMatrix(0,0);

	vector< const CReaction * > & reactions = pProblem->getReorderedReactions();
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
	CCMap * hash = (CCMap*)(model.qHash);

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
		if (contains(hash,words[i]))
		{
			copasiPtr = getHashValue(hash,words[i]);
			if (copasiPtr.species)
			{
				targetSpecies << QPair<int,CMetab*>(i, copasiPtr.species);
				cout << i << "  =  " << words[i].c_str() << endl;
			}
		}
	}
	
	//get the target parameters
	QList< CModelValue* > targetParams;
	for (int i=0; i < params.rows; ++i)
	{
		QString rowname(tc_getRowName(params, i));
		if (contains(hash,rowname))
		{
			copasiPtr = getHashValue(hash,rowname);
			if (copasiPtr.param && copasiPtr.param->getStatus() != CModelValue::ASSIGNMENT)
			{
				targetParams << copasiPtr.param;
				cout << "good  " << i << "  =  " << rowname.c_str() << endl;
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
	cout <<" num = " << targetSpecies.size() << endl;
	for (int i=0; i < targetSpecies.size(); ++i)
	{
		k = targetSpecies[i].first;
		pMetab = targetSpecies[i].second;
		pObjectMap->setRole( k , CExperiment::dependent );
		const CCopasiObject* pParticleReference = pMetab->getObject(CCopasiObjectName("Reference=Concentration"));
		pObjectMap->setObjectCN(k, pParticleReference->getCN());
		cout <<" k = " << k << "  => " << pParticleReference->getCN()  << endl;
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
		pFitItem->setLowerBound(CCopasiObjectName(string(QString::number(tc_getMatrixValue(params,i,1)).c_str())));
		pFitItem->setUpperBound(CCopasiObjectName(string(QString::number(tc_getMatrixValue(params,i,2)).c_str())));
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
			cout << "result = " << result << "\n";
		}
	}
	catch (...)
	{
		// failed
		cout << "failed\n";
		return;
	}
	pFitTask->restore();
	
	//assign optimized values back into the model
	for (int i=0; i < params.rows && i < pFitProblem->getSolutionVariables().size(); ++i)
	{
		double x = pFitProblem->getSolutionVariables()[i];
		cSetValue(model, tc_getRowName(params,i), x);
		tc_setMatrixValue(params, i, 0, x);
		cout << tc_getRowName(params,i) << " = " << x << endl;
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
		return numeric_limits<float>::max();
	
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
		 	retval = numeric_limits<float>::max();
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
				if (string(tc_getRowName(ss, j)) == string(tc_getColumnName(*data,k)))
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
		 	retval = numeric_limits<float>::max();
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
		retval = numeric_limits<float>::max();
	}
	
	for (int i=0; i < params->rows; ++i)
	{
		if (g.gene(i) < tc_getMatrixValue(*params, i, 1) || 
			 g.gene(i) > tc_getMatrixValue(*params, i, 2))
		 {
		 	retval = numeric_limits<float>::max();
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
	ifstream file(objective);
	
	mu::Parser parser;
	GAData pData;
	tc_matrix data;
	
	pData.parser = 0;
	pData.model = &model;
	pData.data = 0;
	pData.params = &params;
	
	list<string> words;
	if (file.is_open())
	{
		int numlines=0;
		string delim("\t");
	
		string line;
		getline(file, line);
		if (line[0] == '#')
			line.erase(0,1);

		if (contains(line, ","))
			delim = string(",");

		words = splitString(line , delim);
		
		if (!words.empty())
		{
			while (!file.good())
			{
				getline(file,line);
				++numlines;
			}
		}
		
		file.close();
		
		if (words.empty() || numlines < 1)
		{
			return tc_createMatrix(0,0);
		}
		else
		{
			ifstream file(objective);
			if (file.is_open())
			{
				data = tc_createMatrix(numlines, words.size());
				pData.data = &data;
				getline(file, line);
				int i=0;
				double d;

				i = 0;
				for (list<string>::iterator it=words.begin(); it != words.end(); it++, ++i)
				{
					tc_setColumnName(data, i, (*it).c_str());
				}
				
				i = 0;
				while (file.good() && i < numlines)
				{
					getline(file, line);
					words = splitString( line, delim);
					list<string>::iterator it = words.begin();
					for (int j=0; it != words.end() && j < data.cols; ++j, it++)
					{
						d = string_to_double((*it));
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
			//cout << "gen " << ++k << "\n";
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
		if (string(tc_getColumnName(data,0)) == string("time") ||
  			 string(tc_getColumnName(data,0)) == string("Time") ||
			 string(tc_getColumnName(data,0)) == string("TIME"))
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
				//cout << "\n\ngen " << ++k << "\n";
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
				//cout << "gen " << ++k << "\n";
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

tc_matrix convertFromDoubleMatrix(DoubleMatrix& matrix, vector< string > &rowNames, vector< string > &colNames)
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

void convertToDoubleMatrix(tc_matrix m, DoubleMatrix & matrix, vector< string > &rowNames, vector< string > &colNames)
{
	matrix.resize(m.rows, m.cols);
	
	rowNames.resize(m.rows);
	colNames.resize(m.cols);
	
	for (int i=0; i < m.rows && i < rowNames.size(); ++i)
		rowNames[i] = string(tc_getRowName(m, i));

	for (int i=0; i < m.cols && i < colNames.size(); ++i)
		colNames[i] = string(tc_getColumnName(m, i));
	
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
	vector< string > rowNames, colNames;	
	DoubleMatrix N;
	
	convertToDoubleMatrix( tc_N , N, rowNames, colNames );
	
	//use libstructural
	LibStructural * instance = LibStructural::getInstance();
	instance->loadStoichiometryMatrix (N);

	CCopasiVector< CMetab > & metabolites = pModel->getMetabolites();
	vector<double> iv(metabolites.size(), 0);  //species concentrations
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
	vector< string > rowNames, colNames;	
	DoubleMatrix N;
	
	convertToDoubleMatrix( tc_N , N, rowNames, colNames );
	
	//use libstructural
	LibStructural * instance = LibStructural::getInstance();
	instance->loadStoichiometryMatrix (N);

	CCopasiVector< CMetab > & metabolites = pModel->getMetabolites();
	vector<double> iv(metabolites.size(), 0);  //species concentrations
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
	vector< string > rowNames, colNames;	
	DoubleMatrix N;
	
	convertToDoubleMatrix( tc_N , N, rowNames, colNames );
	
	//use libstructural
	LibStructural * instance = LibStructural::getInstance();
	instance->loadStoichiometryMatrix (N);

	CCopasiVector< CMetab > & metabolites = pModel->getMetabolites();
	vector<double> iv(metabolites.size(), 0);  //species concentrations
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
	vector< string > rowNames, colNames;	
	DoubleMatrix N;
	
	convertToDoubleMatrix( tc_N , N, rowNames, colNames );
	
	//use libstructural
	LibStructural * instance = LibStructural::getInstance();
	instance->loadStoichiometryMatrix (N);

	CCopasiVector< CMetab > & metabolites = pModel->getMetabolites();
	vector<double> iv(metabolites.size(), 0);  //species concentrations
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
	vector< string > rowNames, colNames;	
	DoubleMatrix N;
	
	convertToDoubleMatrix( tc_N , N, rowNames, colNames );
	
	//use libstructural
	LibStructural * instance = LibStructural::getInstance();
	instance->loadStoichiometryMatrix (N);

	CCopasiVector< CMetab > & metabolites = pModel->getMetabolites();
	vector<double> iv(metabolites.size(), 0);  //species concentrations
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

list<string> splitString(const string& seq, const string& _1cdelim)
{
	bool keeptoken = false, _removews = true;
	list<string> L;
	
	string delims = _1cdelim;
	string STR;
	if(delims.empty()) delims = "\n\r";
	if(_removews) delims += " ";
	 
	string::size_type pos=0, LEN = seq.size();
	while(pos < LEN ){
	STR=""; // Init/clear the STR token buffer
	// remove any delimiters including optional (white)spaces
	while( (delims.find(seq[pos]) != string::npos) && (pos < LEN) ) ++pos;
	// leave if @eos
	if(pos==LEN) return L;
	// Save token data
	while( (delims.find(seq[pos]) == string::npos) && (pos < LEN) ) STR += seq[pos++];
	// put valid STR buffer into the supplied list
	//cout << "[" << STR << "]";
	if( ! STR.empty() ) L.push_back(STR);
	}
	return L;
}



int replaceSubstring(std::string& s,const std::string& from, const std::string& to)
{
	int cnt = -1;

	if(from != to && !from.empty())
	{
		string::size_type pos1(0);
		string::size_type pos2(0);
		const string::size_type from_len(from.size());
		const string::size_type to_len(to.size());
		cnt = 0;

		while((pos1 = s.find(from, pos2)) != std::string::npos)
		{
			s.replace(pos1, from_len, to);
			pos2 = pos1 + to_len;
			++cnt;
		}
	}

	return cnt;
}

