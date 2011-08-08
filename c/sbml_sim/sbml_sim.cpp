#include "sbml_sim.h"
#include <iostream>
extern "C"
{
	#include "cvodesim.h"
	#include "ssa.h"
}

using namespace std;

void sbml_rates_function(double t, double * y, double * rates, void * data)
{
	int i;
	SBML_sim * u = (SBML_sim*)data;

	u->time = t;
	
	for (i=0; i < u->variableValues.size(); ++i)
	{
		u->variableValues[i] = y[i];
	}

	for (i=0; i < u->assignmentEqns.size(); ++i)
	{
		u->assignmentValues[i] = u->assignmentEqns[i].Eval();
	}

	for (i=0; i < u->rateEqns.size(); ++i)
	{
		rates[i] = u->rateEqns[i].Eval();
	}
}

int sbml_event_function(int i, double t, double * y, void * data)
{
	SBML_sim * u = (SBML_sim*)data;
	return u->triggerEqns[i].Eval();
}

void sbml_response_function(int i, double * y, void * data)
{
	SBML_sim * u = (SBML_sim*)data;
	for (int j=0; j < u->responseEqns[i].size(); ++j)
		 u->responseEqns[i][j].Eval();
}

double* muparser_add_variable(const char * s, void* d)
{
	SBML_sim * sim = (SBML_sim*)d;
	sim->parameterNames.push_back(std::string(s));
	sim->parameterValues.push_back(1.0);
	return &(sim->parameterValues[ sim->parameterValues.size()-1 ]);
}

static double power(double x, double e) {	return pow(x,e); }
static double ge(double x, double y) { 	return (double)(x >= y); }
static double gt(double x, double y) { 	return (double)(x > y); }
static double le(double x, double y) { 	return (double)(x <= y); }
static double lt(double x, double y) { 	return (double)(x < y); }


static void addSBMLFunctions(mu::Parser & p)
{
	p.DefineFun("pow", &power , false);
	p.DefineFun("ge", &ge , false);
	p.DefineFun("gt", &gt , false);
	p.DefineFun("le", &le , false);
	p.DefineFun("lt", &lt , false);
}


SBML_sim::SBML_sim(std::string sbml_file, bool isFile)
{
	loadSBML(sbml_file, isFile);
}

SBML_sim::SBML_sim(SBMLDocument * doc)
{
	loadSBML(doc);
}

void SBML_sim::loadSBML(std::string sbml_text, bool isFile)
{
	SBMLReader * sbmlreader = new SBMLReader;
	SBMLDocument * doc;
	
	if (isFile)
		doc = sbmlreader->readSBML(sbml_text);
	else
		doc = sbmlreader->readSBMLFromString(sbml_text); 
		
	loadSBML(doc);
	delete doc;
	delete sbmlreader;
}

void SBML_sim::loadSBML(SBMLDocument * doc)
{
	if (!doc || doc->getNumErrors() > 0)
	{
	}
	else
	{
		Model * model = doc->getModel();
		ListOfParameters * params = model->getListOfParameters();
		ListOfReactions * reacs = model->getListOfReactions();
		ListOfSpecies * species = model->getListOfSpecies();
		ListOfSpeciesTypes * types = model->getListOfSpeciesTypes();
		ListOfEvents * events = model->getListOfEvents();
		ListOfRules * rules = model->getListOfRules();

		vector<string> assignmentEquations, rateEquations, eventTriggers;
		vector< vector<string> > eventResponses;

		if (events)
			for (int i=0; i < events->size(); ++i)
			{
				Event * e = events->get(i);
				eventTriggers.push_back( SBML_formulaToString( e->getTrigger()->getMath() ) );
				ListOfEventAssignments * eventAssn = e->getListOfEventAssignments();
				vector<string> responses;
				string s;
				for (int j=0; j < eventAssn->size(); ++j)
				{
					s = eventAssn->get(j)->getVariable();
					s.append("=");
					s.append( SBML_formulaToString( eventAssn->get(j)->getMath() ) );
					responses.push_back(s);
				}

				eventResponses.push_back( responses );
			}

		if (rules)
			for (int i=0; i < rules->size(); ++i)
			{
				Rule * r = rules->get(i);
			
				if (r->isAssignment())
				{
					AssignmentRule * ar  = (AssignmentRule*)r;
					assignmentVariables.push_back(ar->getVariable());
					assignmentValues.push_back(1.0);
					assignmentEquations.push_back(ar->getFormula());
				}
			}

		if (species)
			for (int i=0; i < species->size(); ++i)
				if (!species->get(i)->getConstant() && !species->get(i)->getBoundaryCondition())
				{
					variableNames.push_back(species->get(i)->getId());
					if (species->get(i)->isSetInitialAmount())
						variableValues.push_back(species->get(i)->getInitialAmount());
					else
					if (species->get(i)->isSetInitialConcentration())
						variableValues.push_back(species->get(i)->getInitialConcentration());
					else
						variableValues.push_back(0.0);
				}
				else
				{
					parameterNames.push_back(species->get(i)->getId());
					if (species->get(i)->isSetInitialAmount())
						parameterValues.push_back(species->get(i)->getInitialAmount());
					else
					if (species->get(i)->isSetInitialConcentration())
						parameterValues.push_back(species->get(i)->getInitialConcentration());
					else
						parameterValues.push_back(0.0);
				}

		if (params)
			for (int i=0; i < params->size(); ++i)
			{
				parameterNames.push_back(params->get(i)->getId());
				parameterValues.push_back(params->get(i)->getValue());
			}

		int numReacs = 0;
		
		if (reacs)
			numReacs = reacs->size();

		stoichiometryMatrix = new double[ numReacs * variableNames.size() ];

		for (int i=0; i < numReacs; ++i)
		{
			Reaction * r = reacs->get(i);
			reactionNames.push_back(r->getId());
			rateEquations.push_back(r->getKineticLaw()->getFormula());
			ListOfSpeciesReferences * reactants = r->getListOfReactants(),
									* products  = r->getListOfProducts();

			for (int j=0; j < variableNames.size(); ++j)
			{
				stoichiometryMatrix[ j*numReacs + i ] = 0.0;

				for (int k=0; k < reactants->size(); ++k)
					if (reactants->get(k) && reactants->get(k)->getSpecies() == variableNames[j])
						stoichiometryMatrix[ j*numReacs + i ] -= 1.0;
						//stoichiometryMatrix[ j*numReacs + i ] -= SpeciesReference_getStoichiometry(reactants->get(k));
					
				for (int k=0; k < products->size(); ++k)
					if (products->get(k) && products->get(k)->getSpecies() == variableNames[j])
						stoichiometryMatrix[ j*numReacs + i ] += 1.0;
						//stoichiometryMatrix[ j*numReacs + i ] += SpeciesReference_getStoichiometry(reactants->get(k));
			}
		}
		
		for (int i=0; i < rateEquations.size(); ++i)
		{
			mu::Parser p;
			addSBMLFunctions(p);
			p.SetExpr(rateEquations[i]);

			for (int j=0; j < variableNames.size(); ++j)
				p.DefineVar("time",&(this->time));

			for (int j=0; j < variableNames.size(); ++j)
				p.DefineVar("Time",&(this->time));
			
			for (int j=0; j < variableNames.size(); ++j)
				p.DefineVar(variableNames[j],&variableValues[j]);

			for (int j=0; j < parameterNames.size(); ++j)
				p.DefineVar(parameterNames[j],&parameterValues[j]);
			
			for (int j=0; j < assignmentVariables.size(); ++j)
				p.DefineVar(assignmentVariables[j],&assignmentValues[j]);

			p.SetVarFactory(muparser_add_variable, (void*)this);

			try
			{
				p.Eval();
				rateEqns.push_back(p);
			}
			catch(...)
			{
				//reactionNames.clear();
				//rateEqns.clear();
				break;
			}
		}
		
		for (int i=0; i < assignmentEquations.size(); ++i)
		{
			mu::Parser p;
			addSBMLFunctions(p);
			p.SetExpr(assignmentEquations[i]);
			
			for (int j=0; j < variableNames.size(); ++j)
				p.DefineVar(variableNames[j],&variableValues[j]);

			for (int j=0; j < parameterNames.size(); ++j)
				p.DefineVar(parameterNames[j],&parameterValues[j]);
			
			for (int j=0; j < assignmentVariables.size(); ++j)
				p.DefineVar(assignmentVariables[j],&assignmentValues[j]);

			//p.SetVarFactory(muparser_add_variable, (void*)this);

			try
			{
				p.Eval();
				assignmentEqns.push_back(p);
			}
			catch(...)
			{
				std::cout << assignmentEquations[i] << std::endl;
				//assignmentVariables.clear();
				//assignmentEqns.clear();
				break;
			}
		}

		for (int i=0; i < eventTriggers.size(); ++i)
		{
			mu::Parser p;
			addSBMLFunctions(p);
			p.SetExpr(eventTriggers[i]);

			for (int j=0; j < variableNames.size(); ++j)
				p.DefineVar("time",&(this->time));

			for (int j=0; j < variableNames.size(); ++j)
				p.DefineVar("Time",&(this->time));
			
			for (int j=0; j < variableNames.size(); ++j)
				p.DefineVar(variableNames[j],&variableValues[j]);

			for (int j=0; j < parameterNames.size(); ++j)
				p.DefineVar(parameterNames[j],&parameterValues[j]);
			
			for (int j=0; j < assignmentVariables.size(); ++j)
				p.DefineVar(assignmentVariables[j],&assignmentValues[j]);

			try
			{
				p.Eval();
				
				//resposes for the trigger
				vector<mu::Parser> responses;
				for (int j=0; j < eventResponses[i].size(); ++j)
				{
					mu::Parser p;
					addSBMLFunctions(p);
					p.SetExpr(eventResponses[i][j]);

					try
					{
						p.Eval();
						responses.push_back(p);
					}
					catch(...) {}
				}

				if (responses.size() > 0)
				{
					triggerEqns.push_back(p);
					responseEqns.push_back(responses);
				}
			}
			catch(...)
			{
				//assignmentVariables.clear();
				//assignmentEqns.clear();
				break;
			}
		}

		//delete params;
		//delete reacs;
	}
}

vector< vector<double> > SBML_sim::simulate(double time, double stepSize) const
{
	int n = variableValues.size();
	double * y0 = new double[n];
	for (int i=0; i < variableValues.size(); ++i)
		y0[i] = variableValues[i];
	
	double * y = ODEsim2(n, reactionNames.size(), stoichiometryMatrix , &sbml_rates_function, y0, 0.0, time, stepSize, (void*)this, triggerEqns.size() , sbml_event_function, sbml_response_function);
	
	vector< vector<double> > res;
	int sz = (int)(time/stepSize);
	
	if (y)
	{
		for (int j=0; j <= n; ++j)
		{
			vector<double> col(sz,0.0);
			for (int i=0; i < sz; ++i)
				col[i] = y[ i * (n+1) + j ];
			res.push_back(col);
		}
		free(y);
	}
	
	free(y0);
	return res;
}

vector<double> SBML_sim::steadyState() const
{
	int n = variableValues.size();
	double * y0 = new double[n];
	for (int i=0; i < variableValues.size(); ++i)
		y0[i] = variableValues[i];
	
	double * y = steadyState2(n, reactionNames.size(), stoichiometryMatrix , &sbml_rates_function, y0, (void*)this, 1.0E-5, 10000.0, 1.0, triggerEqns.size() , sbml_event_function, sbml_response_function);
	vector< double > res(n,0.0);
	
	if (y)
	{
		for (int j=0; j < n; ++j)
		{
			res[j] = y[j];
		}
		free(y);
	}
	
	free(y0);
	return res;	
}

vector< vector<double> > SBML_sim::ssa(double time) const
{
	int n = variableValues.size();
	double * y0 = new double[n];
	for (int i=0; i < variableValues.size(); ++i)
		y0[i] = variableValues[i];
		
	int sz;
	double * y = SSA(n, reactionNames.size(), stoichiometryMatrix , &sbml_rates_function, y0, 0.0, time, 100000, &sz, (void*)this, triggerEqns.size(), sbml_event_function, sbml_response_function);

	vector< vector<double> > res;

	if (y)
	{
		for (int j=0; j <= n; ++j)
		{
			vector<double> col(sz,0.0);
			for (int i=0; i < sz; ++i)
				col[i] = y[ i * (n+1) + j ];
			res.push_back(col);
		}
		free(y);
	}

	free(y0);
	return res;
}

void SBML_sim::setVariableValues( const vector<double> & v )
{
	for (int i=0; i < v.size() && i < variableValues.size(); ++i)
		variableValues[i] = v[i];
}

void SBML_sim::setParameters( const vector<double> & v )
{
	for (int i=0; i < v.size() && i < parameterValues.size(); ++i)
		parameterValues[i] = v[i];
}

vector< string > SBML_sim::getVariableNames() const
{
	return variableNames;
}

vector< string > SBML_sim::getParameterNames() const
{
	return parameterNames;
}

vector< double > SBML_sim::getVariableValues() const
{
	return variableValues;
}

vector< double > SBML_sim::getRateValues() const
{
	return rateValues;
}

vector< double > SBML_sim::getParameterValues() const
{
	return parameterValues;
}

