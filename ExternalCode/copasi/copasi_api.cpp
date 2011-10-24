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

//using macro instead of this variable (as in original copasi code) causes some issues in visual studio
unsigned C_INT32 C_INVALID_INDEX = std::numeric_limits< unsigned C_INT32 >::max();
//#ifdef _WIN32
    static double NaN = std::numeric_limits<double>::quiet_NaN(); 
//#else
	//static double NaN = 0.0/0.0;
//#endif

//this "wrapper" struct is used to store pointer to 
//either a compartment, species, reaction, or parameter
//it also stores the copasi's unique name and the normal human readable name
struct CopasiPtr 
{ 
	string name;
	string key;
	CMetab * species; 
	CCompartment * compartment;
	CReaction * reaction;
	CModelValue * param;
	string assignmentRule;
	bool unused;
};


/***************************
  All of these global variables
  are related hashing names
  and return types
***************************/
typedef map< string, CopasiPtr > CCMap;
static list< CCMap* > hashTablesToCleanup;
static list< copasi_model > copasi_modelsToCleanup;
static map< void*,  map<string,bool> > returnTypeFilters;

/*****************************
  These functions are used for
   string replacements
******************************/
static int rename(string& target, const string& oldname,const string& newname0);
static int replaceSubstring(string& target, const string& oldname,const string& newname0);
static boost::regex sbmlPowFunction("pow\\s*\\(\\s*([^,]+)\\s*,\\s*([^,]+)\\s*\\)", boost::regex::perl);

/*****************************
  These functions are used for
   hashing
******************************/
template <typename T1, typename T2>
bool contains(map<T1,T2> * hash, const T1 & s)
{
	return hash && (hash->find(s) != hash->end());
}

bool contains(const string& str, const string & s)
{
	return str.find(s) != string::npos;
}

template <typename T1, typename T2>
T2 & getHashValue(map<T1,T2> * hash, const T1 & s)
{
	return (*hash)[s];
}

template <typename T1, typename T2>
void hashInsert(map<T1,T2> * hash, const T1 & s, T2 v)
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

/*****************************
       Copasi helper function
******************************/

void copasi_init()
{
	CCopasiRootContainer::init(0, NULL);
}

void copasi_end()
{
	for (list<CCMap*>::iterator i = hashTablesToCleanup.begin(); i != hashTablesToCleanup.end(); i++)
		delete (*i);

	list< copasi_model > models = copasi_modelsToCleanup;
	copasi_modelsToCleanup.clear();

	for (list<copasi_model>::iterator i = models.begin(); i != models.end(); i++)
		cRemoveModel(*i);

	CCopasiRootContainer::destroy();
}

//enable assignment rule substitutions, e.g. if A = sin(time), then B = 2*A  becomes B = 2*sin(time)
int cSetAssignmentRuleHelper(copasi_model , CMetab * , const char * );

static int DO_CLEANUP_ASSIGNMENT_RULES = 1;

void cEnableAssignmentRuleReordering()
{
	DO_CLEANUP_ASSIGNMENT_RULES = 1;
}

void cDisableAssignmentRuleReordering()
{
	DO_CLEANUP_ASSIGNMENT_RULES = 0;
}

//populate the hash table 
void populate_hash(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	CCMap * hash = (CCMap*)(model.qHash);

	if (!pModel || !hash) return;

	CCopasiVectorNS < CCompartment > & compartments = pModel->getCompartments();
	CCopasiVector< CMetab > & species = pModel->getMetabolites();
	CCopasiVectorNS < CReaction > & reacs = pModel->getReactions();
	CCopasiVectorN< CModelValue > & params = pModel->getModelValues();
	
	for (int i=0; i < compartments.size(); ++i)
		if (compartments[i])
		{
			CopasiPtr copasiPtr = { 
				compartments[i]->getCN(),
				compartments[i]->getKey(),
				0,				
				compartments[i],
				0,
				0,
				"",
				true};

			hashInsert(hash,   compartments[i]->getObjectName(),		copasiPtr );
		}

	for (int i=0; i < species.size(); ++i)
		if (species[i])
		{
			CopasiPtr copasiPtr = { 
				species[i]->getCN(),
				species[i]->getKey(),
				species[i],
				0,
				0,
				0,
				"",
				true};

			hashInsert(hash,  species[i]->getObjectName(),	  copasiPtr );

			if (species[i]->getCompartment())
				hashInsert(hash,  species[i]->getCompartment()->getObjectName() + string("_") + species[i]->getObjectName(), copasiPtr);

		}

	for (int i=0; i < reacs.size(); ++i)
		if (reacs[i])
		{
			CopasiPtr copasiPtr = { 
				reacs[i]->getCN(),
				reacs[i]->getKey(),
				0,		
				0,		
				reacs[i],
				0,
				"",
				true};

			hashInsert(hash,   reacs[i]->getObjectName(),		copasiPtr );
		}

	for (int i=0; i < params.size(); ++i)
		if (params[i])
		{
			CopasiPtr copasiPtr = { 
				params[i]->getCN(),
				params[i]->getKey(),
				0,		
				0,		
				0,
				params[i],
				"",
				true};

			hashInsert(hash,   params[i]->getObjectName(),	 copasiPtr );
		}
}

int copasi_cleanup_assignments(copasi_model model)
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
	bool replace_needed = (bool)(DO_CLEANUP_ASSIGNMENT_RULES);
	
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
		if ((*i).second.species)
			if ((*i).second.assignmentRule.empty())
			{
				if ((*i).second.unused)
				{
					(*i).second.species->setStatus(CModelEntity::FIXED); //unused species
				}
			}
			else
			{
				retval = retval * cSetAssignmentRuleHelper(model, (*i).second.species, (*i).second.assignmentRule.c_str());
			}
	return retval;
}

/**********************************************************************************
 The following function is a "hack" for avoiding repeated memory allocations when calling
  tc_createMatrix. For example, suppose a function is calling getSpeciesConcentrations
  inside a look, then it can use setStorageMatrix to prevent re-allocation of new memory
  during every iteration (for speedup). The calling function must be extremely careful. 
  The unsetStorageMatrix must be called at the end of the loop, otherwise there is a good    
***********************************************************************************/
static tc_matrix * _StorageMatrix = NULL;
void setStorageMatrix(tc_matrix * m)
{
	_StorageMatrix = m;
}

void unsetStorageMatrix()
{
	_StorageMatrix = NULL;
}

tc_matrix efficiently_createMatrix(int r, int c)
{
	if (_StorageMatrix)
		return (*_StorageMatrix);
	return tc_createMatrix(r,c);
}

/* The Main API functions */

void cRemoveModel(copasi_model model)
{
	//remove from list
	for (list<copasi_model>::iterator i=copasi_modelsToCleanup.begin(); i != copasi_modelsToCleanup.end(); i++)
		if ((*i).CopasiDataModelPtr == model.CopasiDataModelPtr)
		{
			copasi_model m = { (void*)NULL, (void*)NULL, (void*)NULL, (char*)NULL, (char*)NULL};
			(*i) = m;
		}

	//delete model
	if (model.errorMessage)
		free(model.errorMessage);
	if (model.warningMessage)
		free(model.warningMessage);
	if (model.CopasiDataModelPtr)
		CCopasiRootContainer::removeDatamodel((CCopasiDataModel*)model.CopasiDataModelPtr);
}

void clearcopasi_model(copasi_model model)
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

// ------------------------------------------------------------------
// Create model group
// ------------------------------------------------------------------


copasi_model cCreateModel(const char * name)
{
	copasi_init();

	CCopasiDataModel* pDataModel = CCopasiRootContainer::addDatamodel();
	CModel* pModel = pDataModel->getModel();
	CCMap * qHash = new CCMap();
	copasi_model m = { (void*)(pModel) , (void*)(pDataModel), (void*)(qHash), (char*)(NULL), (char*)(NULL)};
	
	hashTablesToCleanup.push_back(qHash);
	copasi_modelsToCleanup.push_back(m);

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
			//pSpecies->setValue(iv);
			//pSpecies->setInitialValue(iv);
			pSpecies->setInitialConcentration(iv);
		}
		return;
	}
	
	pSpecies = pModel->createMetabolite(name, pCompartment->getObjectName(), iv, CMetab::REACTIONS);
	pSpecies->setConcentration(iv);
	//pSpecies->setValue(iv);
	//pSpecies->setInitialValue(iv);
	pSpecies->setInitialConcentration(iv);

	CopasiPtr copasiPtr = { 
			pSpecies->getCN(),
			pSpecies->getKey(),
			pSpecies,
			0,
			0,
			0,
			"",
			true};

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
			0,
			"",
			false};

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

	CopasiPtr & p = getHashValue(hash,s);
	
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
	}
}

void cSetInitialConcentration(copasi_model model, const char * name, double conc)
{
	CCMap * hash = (CCMap*)(model.qHash);
	string s(name);
	CMetab* pSpecies = NULL;
	
	if (!hash) return;
	
	if (contains(hash,s) && 
		(pSpecies = getHashValue(hash,s).species))
	{
		pSpecies->setInitialConcentration(conc);
	}
}

void cSetAmount(copasi_model model, const char * name, double amnt)
{
	CCMap * hash = (CCMap*)(model.qHash);
	string s(name);
	CMetab* pSpecies = NULL;
	
	if (!hash) return;
	
	if (contains(hash,s) && 
		(pSpecies = getHashValue(hash,s).species))
	{
		pSpecies->setValue(amnt);
		pSpecies->setInitialValue(amnt);
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
				pValue,
				"",
				true};

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
		if (isBoundary)
			pSpecies->setStatus(CModelEntity::FIXED);
		else
			pSpecies->setStatus(CModelEntity::REACTIONS);
	}
}

void cCreateSpecies(CModel * pModel, CCMap * hash, string s)
{
	if (!pModel || !hash) return;

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
						copasi_compartment c = { (void*)getHashValue(hash,s).compartment, pModel, hash };
						cCreateSpecies(c,s.c_str(),0.0);
					}
			}
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
		cCreateSpecies(pModel, hash, s);
	}

	if (contains(hash,s) && getHashValue(hash,s).species)
	{
		CopasiPtr & p = (*hash)[s];
		p.assignmentRule = string(formula);
		boost::regex_replace(p.assignmentRule, sbmlPowFunction, string("((\\1)^(\\2))"));
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
						rename(qFormula,s0,s1);
					}
				}
			}
		}

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
			CopasiPtr & ptr = getHashValue(hash,qname);
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
			pModelValue,
			"",
			true};

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

	CopasiPtr & ptr = getHashValue(hash,string(variable));
	
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
			0,
			"",
			false};

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
	if (!contains(hash, s))
	{
		cCreateSpecies((CModel*)reaction.CopasiModelPtr, (CCMap*)reaction.qHash, s);
	}
	
	if (contains(hash,s))
	{
		CopasiPtr & p = getHashValue(hash,s);
		if (pSpecies = p.species)
		{
			CChemEq* pChemEq = &pReaction->getChemEq();
			pChemEq->addMetabolite(pSpecies->getKey(), stoichiometry, CChemEq::SUBSTRATE);
			p.unused = false;
		}
	}

	if (pSpecies && pSpecies->getCompartment())
	{
		s = pSpecies->getCompartment()->getObjectName() + string("_") + s;
		if (contains(hash,s))
		{
			CopasiPtr & p = getHashValue(hash,s);
			if (pSpecies = p.species)
				p.unused = false;
		}
	}
}

void cAddProduct(copasi_reaction reaction, const char * species, double stoichiometry)
{
	CReaction* pReaction = (CReaction*)(reaction.CopasiReactionPtr);
	CCMap * hash = (CCMap*)(reaction.qHash);
	CMetab* pSpecies = NULL;
	
	if (!pReaction || !hash || !reaction.CopasiModelPtr) return;
	
	string s(species);
	if (!contains(hash, s))
	{
		cCreateSpecies((CModel*)reaction.CopasiModelPtr, (CCMap*)reaction.qHash, s);
	}

	if (contains(hash,s))
	{
		CopasiPtr & p = getHashValue(hash,s);
		if (pSpecies = p.species)
		{
			CChemEq* pChemEq = &pReaction->getChemEq();
			pChemEq->addMetabolite(pSpecies->getKey(), stoichiometry, CChemEq::PRODUCT);
			p.unused = false;
		}
	}

	if (pSpecies && pSpecies->getCompartment())
	{
		s = pSpecies->getCompartment()->getObjectName() + string("_") + s;
		if (contains(hash,s))
		{
			CopasiPtr & p = getHashValue(hash,s);
			if (pSpecies = p.species)
				p.unused = false;
		}
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
		//formula2.replace(sbmlPowFunction, QString("((\\1)^(\\2))"));
		boost::regex_replace(formula2, sbmlPowFunction, string("((\\1)^(\\2))"));
		
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
					copasi_model model = { (void*)(pModel) , (void*)(0), (void*)(hash), (char*)(NULL), (char*)(NULL)};
					cSetGlobalParameter(model,pParam->getObjectName().c_str(),1.0);				
				}
				
				if (contains(hash,s))
				{
					CopasiPtr & p = getHashValue(hash,s);
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

void cCompileModel(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	
	if (!pModel) return;

	copasi_cleanup_assignments(model);
	
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
	//cCompileModel(model);
	
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
	
		tc_matrix output = efficiently_createMatrix(rows, cols);
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

// STUB: NEEDS TO BE IMPLEMENTED
double cOneStep(copasi_model model, double timeStep)
{
	return 0.0;
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

static int _SBML_LEVEL = 2;
static int _SBML_VERSION = 2;
void cSetSBMLLevelAndVersion(int level, int version)
{
	_SBML_LEVEL = level;
	_SBML_VERSION = version;
}

void cWriteSBMLFile(copasi_model model, const char * filename)
{
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	if (pDataModel)
		pDataModel->exportSBML(filename, true, _SBML_LEVEL, _SBML_VERSION);
}

void cWriteAntimonyFile(copasi_model model, const char * filename)
{
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	if (pDataModel)
	{
		pDataModel->exportSBML(filename, true, _SBML_LEVEL, _SBML_VERSION);
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

//this helper class can load either sbml file or string, since both are identical except for one line
static copasi_model cReadSBML_helper(const char * sbml, bool isFile)
{
	copasi_init();
	
	CCopasiDataModel* pDataModel = CCopasiRootContainer::addDatamodel();
	CModel* pModel = 0;
	CCMap * qHash = 0;	
	char * error = NULL;
	char * warning = NULL;
	string s;
	CCopasiMessage::Type type;

	try 
	{
		if (isFile)  //here, the code is different depending on file or string input
			pDataModel->importSBML(sbml); //SBML file -> COPASI
		else
			pDataModel->importSBMLFromString(sbml); //SBML string -> COPASI	

		s = CCopasiMessage::getAllMessageText();
		type = CCopasiMessage::getHighestSeverity();
		pModel = pDataModel->getModel();
		qHash = new CCMap();	
	}
	catch(...)
	{
		s = CCopasiMessage::getAllMessageText();
		type = CCopasiMessage::ERROR;
	}

	int len = s.length();
	if (len > 1)
	{
		char * msg = (char*)malloc((1+len) * sizeof(char));
		if (msg)
		{
			for (int i=0; i < len; ++i) msg[i] = s[i];
			msg[len-1] = 0;
		}

		//error or warning?
		if (type == CCopasiMessage::EXCEPTION || type == CCopasiMessage::ERROR)
			error = msg;
		else
			warning = msg;
	}

	copasi_model m = { (void*)(pModel) , (void*)(pDataModel), (void*)(qHash), (char*)(error), (char*)warning};
	if (pModel && qHash)
	{
		hashTablesToCleanup.push_back( qHash );
		copasi_modelsToCleanup.push_back(m);
		populate_hash(m);
	}
	return m;
}

copasi_model cReadSBMLFile(const char * filename)
{
	return cReadSBML_helper(filename, true);
}

copasi_model cReadSBMLString(const char * sbml)
{
	return cReadSBML_helper(sbml, false);
}

tc_matrix cGetJacobian(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	//cCompileModel(model);
	
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
		tc_matrix J = efficiently_createMatrix(n,n);
		
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
	
	//cCompileModel(model);

    int iter = 0;
    double err = 2.0, eps = 0.01, time = 10.0;

   	CCopasiVectorN< CCopasiTask > & TaskList = * pDataModel->getTaskList();
	CTrajectoryTask* pTask = dynamic_cast<CTrajectoryTask*>(TaskList["Time-Course"]);
	// if there isn’t one
	if (pTask == NULL)
	{
		pTask = new CTrajectoryTask();
		TaskList.remove("Time-Course");
		TaskList.add(pTask, true);
	}

	if (pTask)
		pTask->setUpdateModel(true);

    while (iter < maxiter && err > eps)
    {
        ++iter;
        time *= 2.0;

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
                pTask->restore();
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
                tc_matrix output = efficiently_createMatrix(cols, 1);
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
	
	//cCompileModel(model);

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
		pProblem->setDuration(100.0);
		pDataModel->getModel()->setInitialTime(0.0);
		pProblem->setTimeSeriesRequested(true);
		try
		{
			pTrajTask->initialize(CCopasiTask::ONLY_TIME_SERIES, pDataModel, NULL);
			pTrajTask->process(true);
			pTrajTask->restore();
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
		pTask->setUpdateModel(true);
		pTask->initialize(CCopasiTask::OUTPUT, pDataModel, NULL);
		pTask->process(true);
		pTask->restore();
	}
	catch (...)
	{
		cerr << "Error when computing steady state." << endl;
		return tc_createMatrix(0,0);
	}

	CCopasiMessage::clearDeque();
	
	return cGetFloatingSpeciesConcentrations(model);
}

tc_matrix cGetEigenvalues(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCopasiDataModel* pDataModel = (CCopasiDataModel*)(model.CopasiDataModelPtr);
	
	if (!pModel || !pDataModel) return tc_createMatrix(0,0);
	//cCompileModel(model);
	
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

	tc_matrix E = efficiently_createMatrix(im.size(),2);
	
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
	//cCompileModel(model);
	
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

	tc_matrix M = efficiently_createMatrix(rows, cols);
	
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
	//cCompileModel(model);
	
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

	tc_matrix M = efficiently_createMatrix(rows, cols);
	
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
	//cCompileModel(model);
	
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

	tc_matrix M = efficiently_createMatrix(rows, cols);
	
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
	//cCompileModel(model);
	
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

	tc_matrix M = efficiently_createMatrix(rows, cols);
	
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
	//cCompileModel(model);
	
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

	tc_matrix M = efficiently_createMatrix(rows, cols);
	
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
	//cCompileModel(model);
	
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
	
	tc_matrix M = efficiently_createMatrix(rows, cols);

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
	//cCompileModel(model);
	
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
	//cCompileModel(model);
	
	CCopasiVector< CMetab > & species = pModel->getMetabolites();
	CCopasiVectorNS < CReaction > & reacs = pModel->getReactions();
	CMatrix < C_FLOAT64 > stoi = pModel->getStoi();

	tc_matrix N = efficiently_createMatrix( stoi.numRows(), stoi.numCols() );

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
	//cCompileModel(model);
	
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
	tc_matrix M = efficiently_createMatrix( reactions.size() , fluxModes.size() );
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

	tc_matrix output = efficiently_createMatrix(data->rows, data->cols);
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
	
	tc_matrix result = efficiently_createMatrix( pop.size(), params.rows );
	
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
	tc_matrix m = efficiently_createMatrix(matrix.numRows(), matrix.numCols());
	
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

tc_matrix cGetReactionRates(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	
	if (!pModel) return tc_createMatrix(0,0);

	const CCopasiVectorNS < CReaction > & reactions = pModel->getReactions();

	tc_matrix res  = efficiently_createMatrix(1, reactions.size());

	for (int i=0; i < reactions.size(); ++i)
		if (reactions[i])
		{
			tc_setColumnName(res, i, reactions[i]->getObjectName().c_str());
			tc_setMatrixValue(res, 0, i, reactions[i]->calculateFlux());
		}
	
	return res;
}

double cGetReactionRate(copasi_model model, const char * name)
{
	return cGetFlux(model, name);
}

tc_matrix cGetReactionRatesEx(copasi_model model, tc_matrix conc)
{
	int i,j;
	tc_matrix savedMatrix = tc_createMatrix(conc.rows, conc.cols);
	for (i=0; i < conc.rows; ++i)
		for (j=0; j < conc.cols; ++j)
			tc_setMatrixValue( savedMatrix, i, j, tc_getMatrixValue(conc, i, j));
	cSetValues(model,conc);

	double * temp = conc.values;
	conc.values = savedMatrix.values;

	cSetValues(model,conc);

	conc.values = temp;
	tc_deleteMatrix(savedMatrix);

	tc_matrix rates = cGetReactionRates(model);
	

	return rates;
}

tc_matrix cGetFloatingSpeciesConcentrations(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCMap * hash = (CCMap*)(model.qHash);
	
	if (!pModel) return tc_createMatrix(0,0);

	const CCopasiVector< CMetab > & species = pModel->getMetabolites();

	int n = 0;
	for (int i=0; i < species.size(); ++i)
		if (species[i] && 
			(species[i]->getStatus() == CModelEntity::ODE || species[i]->getStatus() == CModelEntity::REACTIONS))
			++n;

	tc_matrix res  = efficiently_createMatrix(n,1);

	for (int i=0, j=0; i < species.size(); ++i)
		if (species[i] && 
			(species[i]->getStatus() == CModelEntity::ODE || species[i]->getStatus() == CModelEntity::REACTIONS))
		{
			tc_setMatrixValue(res, j, 0, species[i]->getConcentration());
			tc_setRowName(res, j, species[i]->getObjectName().c_str());
			++j;
		}

	return res;
}

tc_matrix cGetBoundarySpecies(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCMap * hash = (CCMap*)(model.qHash);
	
	if (!pModel) return tc_createMatrix(0,0);

	const CCopasiVector< CMetab > & species = pModel->getMetabolites();

	int n = 0;
	for (int i=0; i < species.size(); ++i)
		if (species[i] && 
			(species[i]->getStatus() == CModelEntity::FIXED))
			++n;

	tc_matrix res  = efficiently_createMatrix(n,1);

	for (int i=0, j=0; i < species.size(); ++i)
		if (species[i] && 
			(species[i]->getStatus() == CModelEntity::FIXED))
		{
			tc_setMatrixValue(res, j, 0, species[i]->getConcentration());
			tc_setRowName(res, j, species[i]->getObjectName().c_str());
			++j;
		}

	return res;
}

int cGetNumberOfSpecies(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCMap * hash = (CCMap*)(model.qHash);
	
	if (!pModel) return 0;

	const CCopasiVector< CMetab > & species = pModel->getMetabolites();

	int n = species.size();
	return n;
}

int cGetNumberOfFloatingSpecies(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCMap * hash = (CCMap*)(model.qHash);
	
	if (!pModel) return 0;

	const CCopasiVector< CMetab > & species = pModel->getMetabolites();

	int n = 0;
	for (int i=0; i < species.size(); ++i)
		if (species[i] && 
			(species[i]->getStatus() == CModelEntity::ODE || species[i]->getStatus() == CModelEntity::REACTIONS))
			++n;
	return n;
}

int cGetNumberOfBoundarySpecies(copasi_model model)
{
		CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCMap * hash = (CCMap*)(model.qHash);
	
	if (!pModel) return 0;

	const CCopasiVector< CMetab > & species = pModel->getMetabolites();

	int n = 0;
	for (int i=0; i < species.size(); ++i)
		if (species[i] && 
			(species[i]->getStatus() == CModelEntity::FIXED))
			++n;
	return n;
}
tc_matrix cGetFloatingSpeciesIntitialConcentrations (copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCMap * hash = (CCMap*)(model.qHash);
	
	if (!pModel) return tc_createMatrix(0,0);

	const CCopasiVector< CMetab > & species = pModel->getMetabolites();

	int n = 0;
	for (int i=0; i < species.size(); ++i)
		if (species[i] && 
			(species[i]->getStatus() == CModelEntity::ODE || species[i]->getStatus() == CModelEntity::REACTIONS))
			++n;

	tc_matrix res  = efficiently_createMatrix(n,1);

	for (int i=0, j=0; i < species.size(); ++i)
		if (species[i] && 
			(species[i]->getStatus() == CModelEntity::ODE || species[i]->getStatus() == CModelEntity::REACTIONS))
		{
			tc_setMatrixValue(res, j, 0, species[i]->getInitialConcentration());
			tc_setRowName(res, j, species[i]->getObjectName().c_str());
			++j;
		}

	return res;
}

void cSetFloatingSpeciesIntitialConcentrations (copasi_model model, tc_matrix sp)
{
	if (sp.rows > sp.cols)  //row vector or column vector (lets allow both)
	{
		for (int i=0; i < sp.rows; ++i)
			cSetInitialConcentration(model, tc_getRowName(sp,i), tc_getMatrixValue(sp, i, 0));
	}
	else
	{
		for (int i=0; i < sp.cols; ++i)
			cSetInitialConcentration(model, tc_getColumnName(sp,i), tc_getMatrixValue(sp, 0, i));
	} 
}

void cSetBoundarySpeciesConcentrations (copasi_model model, tc_matrix d)
{
	cSetValues(model, d);
}

tc_matrix cGetAllSpecies(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCMap * hash = (CCMap*)(model.qHash);
	
	if (!pModel) return tc_createMatrix(0,0);

	const CCopasiVector< CMetab > & species = pModel->getMetabolites();

	tc_matrix res  = efficiently_createMatrix(1, species.size());

	for (int i=0; i < species.size(); ++i)
		if (species[i])
		{
			tc_setColumnName(res, i, species[i]->getObjectName().c_str());
			tc_setMatrixValue(res, 0, i, species[i]->getConcentration());
		}
	
	return res;
}

tc_matrix cGetCompartments(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCMap * hash = (CCMap*)(model.qHash);
	
	if (!pModel) return tc_createMatrix(0,0);

	const CCopasiVectorNS< CCompartment > & compartments = pModel->getCompartments();

	tc_matrix res  = efficiently_createMatrix(1, compartments.size());

	for (int i=0; i < compartments.size(); ++i)
		if (compartments[i])
		{
			tc_setColumnName(res, i, compartments[i]->getObjectName().c_str());
			tc_setMatrixValue(res, 0, i, compartments[i]->getValue());
		}
	return res;
}

tc_matrix cGetAmounts(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCMap * hash = (CCMap*)(model.qHash);
	
	if (!pModel) return tc_createMatrix(0,0);

	const CCopasiVector< CMetab > & species = pModel->getMetabolites();

	tc_matrix res  = efficiently_createMatrix(1, species.size());

	for (int i=0; i < species.size(); ++i)
		if (species[i] && species[i]->getCompartment())
		{
			tc_setColumnName(res, i, species[i]->getObjectName().c_str());
			tc_setMatrixValue(res, 0, i, 
					CMetab::convertToNumber( species[i]->getConcentration(), *species[i]->getCompartment(), pModel ));
		}

	return res;
}

double cGetConcentration(copasi_model model, const char * name)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCMap * hash = (CCMap*)(model.qHash);
	string s(name);
	
	if (!pModel || !contains(hash, s)) return -1.0;
	CopasiPtr & p = getHashValue(hash, s);

	if (!p.species || !p.species->getCompartment()) return -1.0;	

	return p.species->getConcentration();
}

double cGetAmount(copasi_model model, const char * name)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCMap * hash = (CCMap*)(model.qHash);
	string s(name);
	
	if (!pModel || !contains(hash, s)) return -1.0;
	CopasiPtr & p = getHashValue(hash, s);

	if (!p.species || !p.species->getCompartment()) return -1.0;	

	return CMetab::convertToNumber( p.species->getConcentration(), *p.species->getCompartment(), pModel );
}

tc_matrix cGetRatesOfChangeEx(copasi_model model, tc_matrix m)
{
	cSetValues(model, m);
	return cGetRatesOfChange(model);
}

tc_matrix cGetRatesOfChange(copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	
	if (!pModel)
		return tc_createMatrix(0,0);

	const CCopasiVector< CMetab > & species = pModel->getMetabolites();
	int n = pModel->getState().getNumVariable();

	if (n > species.size()) n = species.size();
	tc_matrix res  = efficiently_createMatrix(1, n);

	for (int i=0; i < species.size(); ++i)
		if (species[i] && species[i]->getCompartment())
		{
			tc_setColumnName(res, i, species[i]->getObjectName().c_str());
		}
	
	pModel->calculateDerivatives(res.values);
	return res;
}

double cGetFlux(copasi_model model, const char * name)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCMap * hash = (CCMap*)(model.qHash);
	string s (name);	

	if (!pModel || !contains(hash, s)) return NaN;

	CopasiPtr & p = getHashValue(hash, s);

	if (!p.reaction) return NaN;

	return p.reaction->calculateFlux();
}

double cGetParticleFlux(copasi_model model, const char * name)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCMap * hash = (CCMap*)(model.qHash);
	string s (name);	

	if (!pModel || !contains(hash, s)) return NaN;

	CopasiPtr & p = getHashValue(hash, s);

	if (!p.reaction) return NaN;

	return p.reaction->calculateParticleFlux();
}


// ------------------------------------------------------------------
// Parameter Group
// ------------------------------------------------------------------

int sSetGlobalParameter(copasi_model model, const char * name, double value)
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
				pValue,
				"",
				true};

		hashInsert(hash, s, copasiPtr); //for speedy lookup
	}
	
	return 0;
}

int cGetNumberOfGlobalParameters (copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	if (!pModel) return 0;

	CCopasiVectorN< CModelValue > & params = pModel->getModelValues();
	return params.size();
}

tc_matrix cGetGlobalParameters (copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCMap * hash = (CCMap*)(model.qHash);

	if (!pModel || !hash) return tc_createMatrix(0,0);

	CCopasiVectorN< CModelValue > & params = pModel->getModelValues();

	tc_matrix m = efficiently_createMatrix(params.size(),1);

	for (int i=0; i < params.size(); ++i)
		if (params[i])
		{
			tc_setRowName(m, i, params[i]->getObjectName().c_str());
			tc_setMatrixValue(m, i, 0, params[i]->getValue());
		}
	return m;
}

void cSetValues (copasi_model model, tc_matrix gp)
{
	if (gp.rows > gp.cols)  //row vector or column vector (lets allow both)
	{
		for (int i=0; i < gp.rows; ++i)
			cSetValue(model, tc_getRowName(gp,i), tc_getMatrixValue(gp, i, 0));
	}
	else
	{
		for (int i=0; i < gp.cols; ++i)
			cSetValue(model, tc_getColumnName(gp,i), tc_getMatrixValue(gp, 0, i));
	}
}

void cSetGlobalParameterValues (copasi_model model, tc_matrix gp)
{
	cSetValues(model, gp);
}

void cSetCompartmentVolumes (copasi_model model, tc_matrix v)
{
	cSetValues(model, v);
}

int cGetNumberOfCompartments (copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	
	if (!pModel) return 0;

	const CCopasiVectorNS< CCompartment > & compartments = pModel->getCompartments();
	return compartments.size();
}

int cGetNumberOfReactions (copasi_model model)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	
	if (!pModel) return 0;

	CCopasiVectorNS < CReaction > & reacs = pModel->getReactions();
	return reacs.size();
}

tc_matrix cGetReactionRatesFromTimeCourse(copasi_model model, tc_matrix results)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCMap * hash = (CCMap*)(model.qHash);

	if (!pModel || !hash) return results;

	vector<CMetab*> metabs;
	metabs.resize(results.cols, (CMetab*)NULL);
	for (int i=0; i < results.cols; ++i)
	{
		string s(tc_getColumnName(results, i));
		if (contains(hash, s))
		{
			CopasiPtr & p = getHashValue(hash,s);
			metabs[i] = p.species;
		}
	}

	list<string> colnames;
	CCopasiVectorNS < CReaction > & reacs = pModel->getReactions();
	int i,j,k;

	//get reaction names and sort them	
	for (i=0; i < reacs.size(); ++i)
		if (reacs[i])
			colnames.push_back(reacs[i]->getObjectName());
	
	tc_matrix output = tc_createMatrix(results.rows, 1 + reacs.size());
	colnames.sort();
	vector<CReaction*> ordered_reacs;
	ordered_reacs.resize(reacs.size(),(CReaction*)0);

	for (i=0; i < reacs.size(); ++i) //map from original to sorted list
		if (reacs[i])
		{
			k = indexOf(colnames, reacs[i]->getObjectName());
			if (k > -1)
				ordered_reacs[k] = reacs[i];
		}

	j = 0;
	tc_setColumnName( output, 0, tc_getColumnName(results,0) );  //first column name remains same, e.g. "time"
	
	for (list<string>::iterator it=colnames.begin(); j < (1+output.cols) && it != colnames.end(); ++j, it++) //set column names
		tc_setColumnName( output, 1+j, (*it).c_str() );
	
	//the main loop -- calculate fluxes for each row
	for (i=0; i < results.rows; ++i)
	{
		tc_setMatrixValue( output, i, 0, 
				tc_getMatrixValue(results,i,0) );  //copy the first column, presumably the independent variable (e.g. time)
		for (j=0; j < (results.cols-1); ++j) //update metab concentrations
			if (metabs[j])
			{
				double v = tc_getMatrixValue(results, i, j+1);
				metabs[j]->setConcentration(v);
				metabs[j]->setValue(v); 
			}			
		for (j=0; j <  (output.cols-1); ++j) //calculate flux
		{
			CReaction * r = ordered_reacs[j];
			tc_setMatrixValue( output, i, 1+j, r->calculateFlux() );
		}
	}

	return output;
}

tc_matrix cGetCCFromTimeCourse(copasi_model model, tc_matrix results)
{
	CModel* pModel = (CModel*)(model.CopasiModelPtr);
	CCMap * hash = (CCMap*)(model.qHash);

	if (!pModel || !hash) return results;

	vector<CMetab*> metabs;
	metabs.resize(results.cols, (CMetab*)NULL);
	for (int i=0; i < results.cols; ++i)
	{
		string s(tc_getColumnName(results, i));
		if (contains(hash, s))
		{
			CopasiPtr & p = getHashValue(hash, s);
			metabs[i] = p.species;
		}
	}
	
	tc_matrix cc = cGetScaledConcentrationConcentrationCoeffs(model);
	setStorageMatrix(&cc); //for speed-up.. might be risky on multithreaded apps
	
	int i,j,k;

	//get cc names
	list<string> colnames;
	for (i=0; i < cc.rows; ++i)
	{
		string rowname( tc_getRowName(cc, i) );
		for (j=0; j < cc.cols; ++j)
		{
			string colname( tc_getColumnName(cc, j) );			
			colnames.push_back( string("cc_") + rowname + string("_") + colname); //naming convention: cc_x_y
		}
	}
	
	tc_matrix output = tc_createMatrix(results.rows, 1 + colnames.size()); //the matrix with cc's for each row in the time series

	j = 0;
	tc_setColumnName( output, 0, tc_getColumnName(results,0) );  //first column name remains same, e.g. "time"
	
	for (list<string>::iterator it=colnames.begin(); j < (1+output.cols) && it != colnames.end(); ++j, it++) //set column names
		tc_setColumnName( output, 1+j, (*it).c_str() );
	
	//the main loop -- calculate cc for each row
	for (i=0; i < results.rows; ++i)
	{
		tc_setMatrixValue( output, i, 0, 
				tc_getMatrixValue(results,i,0) );  //copy the first column, presumably the independent variable (e.g. time)

		for (j=0; j < (results.cols-1); ++j) //update metab concentrations
			if (metabs[j])
			{
				double v = tc_getMatrixValue(results, i, j+1);
				metabs[j]->setConcentration(v);
				metabs[j]->setValue(v); 
			}

		cc = cGetScaledConcentrationConcentrationCoeffs(model);
		int l = 0;
		for (j=0; j <  cc.rows; ++j)
			for (k=0; k <  cc.cols; ++k)
			{
				tc_setMatrixValue( output, i, 1+l, 
					tc_getMatrixValue(cc, i, j) );
				++l;
			}
	}

	unsetStorageMatrix();
	return output;
}


tc_matrix cGetCustomFormulaFromTimeCourse(copasi_model model, tc_matrix results, const char * formula)
{
	return results;
}

tc_matrix cFilterTimeCourseResults(copasi_model model, tc_matrix results, tc_strings names)
{
	return results;
}

