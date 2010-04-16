#include "sbml_sim.h"
#include <iostream>

using namespace std;

void sbml_rates_function(double t, double * y, double * rates, void * data)
{
	int i;
	SBML_sim * u = (SBML_sim*)data;
	
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

double power(double x, double e)
{
	return pow(x,e);
}

SBML_sim::SBML_sim(string sbml_text, bool isFile)
{
	SBMLReader * sbmlreader = new SBMLReader;
	SBMLDocument * doc;
	
	if (isFile)
		doc = sbmlreader->readSBML(sbml_text);
	else
		doc = sbmlreader->readSBMLFromString(sbml_text); 

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
		ListOfRules * rules = model->getListOfRules();
		
		vector<string> assignmentEquations, rateEquations;
		
		for (int i=0; i < rules->size(); ++i)
		{
			Rule * r = rules->get(i);
			
			if (r->isAssignment())
			{
				AssignmentRule * ar  = (AssignmentRule*)r;
				assignmentVariables.push_back(ar->getVariable());
				assignmentValues.push_back(0.0);
				assignmentEquations.push_back(ar->getFormula());
			}
		}
		
		for (int i=0; i < species->size(); ++i)
			if (!species->get(i)->getConstant() && !species->get(i)->getBoundaryCondition())
			{
				variableNames.push_back(species->get(i)->getId());
				variableValues.push_back(species->get(i)->getInitialConcentration());
			}
			else
			{
				parameterNames.push_back(species->get(i)->getId());
				parameterValues.push_back(species->get(i)->getInitialConcentration());
			}

		for (int i=0; i < params->size(); ++i)
		{
			parameterNames.push_back(params->get(i)->getId());
			parameterValues.push_back(params->get(i)->getValue());
		}

		int numReacs = reacs->size();

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
					if (reactants->get(k)->getSpecies() == variableNames[j])
						stoichiometryMatrix[ j*numReacs + i ] -= ((SpeciesReference*)(reactants->get(k)))->getStoichiometry();
					
				for (int k=0; k < products->size(); ++k)
					if (products->get(k)->getSpecies() == variableNames[j])
						stoichiometryMatrix[ j*numReacs + i ] += ((SpeciesReference*)(reactants->get(k)))->getStoichiometry();;
			}
		}
		
		for (int i=0; i < rateEquations.size(); ++i)
		{
			mu::Parser p;
			p.DefineFun("pow", &power , false);
			p.SetExpr(rateEquations[i]);
			
			for (int j=0; j < variableNames.size(); ++j)
				p.DefineVar(variableNames[j],&variableValues[j]);

			for (int j=0; j < parameterNames.size(); ++j)
				p.DefineVar(parameterNames[j],&parameterValues[j]);
			
			for (int j=0; j < assignmentVariables.size(); ++j)
				p.DefineVar(assignmentVariables[j],&assignmentValues[j]);

			try
			{
				rateEqns.push_back(p);
			}
			catch(...)
			{
				reactionNames.clear();
				rateEqns.clear();
				break;
			}
		}
		
		for (int i=0; i < assignmentEquations.size(); ++i)
		{
			mu::Parser p;
			p.DefineFun("pow", &power , false);
			p.SetExpr(assignmentEquations[i]);
			
			for (int j=0; j < variableNames.size(); ++j)
				p.DefineVar(variableNames[j],&variableValues[j]);

			for (int j=0; j < parameterNames.size(); ++j)
				p.DefineVar(parameterNames[j],&parameterValues[j]);
			
			for (int j=0; j < assignmentVariables.size(); ++j)
				p.DefineVar(assignmentVariables[j],&assignmentValues[j]);

			try
			{
				p.Eval();
				assignmentEqns.push_back(p);
			}
			catch(...)
			{
				assignmentVariables.clear();
				assignmentEqns.clear();
				break;
			}
		}

		//delete params;
		//delete reacs;
		delete doc;
	}
}

vector< vector<double> > SBML_sim::simulate(double time, double stepSize) const
{
	int n = variableValues.size();
	double * y0 = new double[n];
	for (int i=0; i < variableValues.size(); ++i)
		y0[i] = variableValues[i];
	
	double * y = ODEsim2(n, reactionNames.size(), stoichiometryMatrix , &sbml_rates_function, y0, 0.0, time, stepSize, (void*)this, 0, 0, 0);
	
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
	
	double * y = steadyState2(n, reactionNames.size(), stoichiometryMatrix , &sbml_rates_function, y0, (void*)this, 1.0E-5, 10000.0, 1.0, 0, 0, 0);
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
	double * y = SSA(n, reactionNames.size(), stoichiometryMatrix , &sbml_rates_function, y0, 0.0, time, 100000, &sz, (void*)this);
	
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

