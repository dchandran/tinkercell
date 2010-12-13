/****************************************************************************
** This plugin makes numerous algorithms from COPASI available inside TinkerCell
** including deterministic, exact stochastic, Tau-leap, and hybrid simulations
** steady state analysis, Jacobian, eigenvalues
** all with sliders 
****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "copasi/copasi_api.h"
#include "TC_api.h"

copasi_model model;
double start, end;
int numpoints;

void setSliderValues(tc_matrix params)
{
	int i;
	model = tc_CopasiModel();
	for (i=0; i < params.rows; ++i)
		setValue(model, tc_getRowName(params,i), tc_getMatrixValue(params,i, 0));  //set values from slider
}

tc_matrix getSliderTable()
{
	int i;
	tc_matrix params, N, allParams, initVals;
	tc_items A, B;
	
	A = tc_allItems();
	params = tc_getParameters(A);
	N = tc_getStoichiometry(A);
	B = tc_findItems(N.rownames);
	tc_deleteMatrix(N);
	
	initVals = tc_getInitialValues(B);
	allParams = tc_createMatrix(initVals.rows+params.rows,2);

	for (i=0; i < params.rows; ++i)
	{
		tc_setRowName(allParams,i, tc_getRowName(params,i));
		tc_setMatrixValue(allParams,i,0,tc_getMatrixValue(params,i,0)/10.0);
		tc_setMatrixValue(allParams,i,1, 2*tc_getMatrixValue(params,i,0) - tc_getMatrixValue(allParams,i,0));
	}
	for (i=0; i < initVals.rows; ++i)
	{
		tc_setRowName(allParams,i+params.rows, tc_getRowName(initVals,i));
		tc_setMatrixValue(allParams,i+params.rows,0,tc_getMatrixValue(initVals,i,0)/10.0);
		tc_setMatrixValue(allParams,i+params.rows,1, 2*tc_getMatrixValue(initVals,i,0) - tc_getMatrixValue(allParams,i+params.rows,0));
	}
	
	tc_deleteMatrix(initVals);
	tc_deleteMatrix(params);
	tc_deleteItemsArray(B);
	return allParams;
}

void ode(tc_matrix params)
{
	tc_matrix output;
	setSliderValues(params);	
	//simulate
	output = simulateDeterministic(model, start, end, numpoints);
	tc_plot(output, "Time course simulation");
	tc_deleteMatrix(output);
}

void call_ode(tc_matrix input)
{
	tc_matrix allParams, output;
	model = tc_CopasiModel();
	start = tc_getMatrixValue(input, 0, 0); //start time
	end = tc_getMatrixValue(input, 1, 0); //end time
	numpoints = 	tc_getMatrixValue(input, 2, 0); //num points
	
	if (tc_getMatrixValue(input, 4, 0) > 0) // sliders?
	{
		allParams = getSliderTable();
		tc_createSliders( allParams, &ode );
		tc_deleteMatrix(allParams);
	}
	else
	{
		output = simulateDeterministic(model, start, end, numpoints);
		tc_plot(output, "Time course simulation");
	}
	tc_deleteMatrix(input);
	tc_deleteMatrix(output);
}

void ssa(tc_matrix params)
{
	tc_matrix output;
	setSliderValues(params);	
	//simulate
	output = simulateStochastic(model, start, end, numpoints);
	tc_plot(output, "Stochastic simulation");
	tc_deleteMatrix(output);
}

void call_ssa(tc_matrix input)
{
	tc_matrix allParams, output;
	model = tc_CopasiModel();
	start = tc_getMatrixValue(input, 0, 0); //start time
	end = tc_getMatrixValue(input, 1, 0); //end time
	numpoints = 	tc_getMatrixValue(input, 2, 0); //num points
	
	if (tc_getMatrixValue(input, 4, 0) > 0) // sliders?
	{
		allParams = getSliderTable();
		tc_createSliders( allParams, &ssa );
		tc_deleteMatrix(allParams);
	}
	else
	{
		output = simulateStochastic(model, start, end, numpoints);
		tc_plot(output, "Stochastic simulation");
	}
	tc_deleteMatrix(input);
	tc_deleteMatrix(output);
}

void tauleap(tc_matrix params)
{
	tc_matrix output;
	setSliderValues(params);	
	//simulate
	output = simulateTauLeap(model, start, end, numpoints);
	tc_plot(output, "Stochastic simulation");
	tc_deleteMatrix(output);
}

void call_tauleap(tc_matrix input)
{
	tc_matrix allParams, output;
	model = tc_CopasiModel();
	start = tc_getMatrixValue(input, 0, 0); //start time
	end = tc_getMatrixValue(input, 1, 0); //end time
	numpoints = 	tc_getMatrixValue(input, 2, 0); //num points
	
	if (tc_getMatrixValue(input, 4, 0) > 0) // sliders?
	{
		allParams = getSliderTable();
		tc_createSliders( allParams, &tauleap );
		tc_deleteMatrix(allParams);
	}
	else
	{
		output = simulateTauLeap(model, start, end, numpoints);
		tc_plot(output, "Stochastic simulation");
	}
	tc_deleteMatrix(input);
	tc_deleteMatrix(output);
}

void hybrid(tc_matrix params)
{
	tc_matrix output;
	setSliderValues(params);
	//simulate
	output = simulateHybrid(model, start, end, numpoints);
	tc_plot(output, "Hybrid simulation");
	tc_deleteMatrix(output);
}

void call_hybrid(tc_matrix input)
{
	tc_matrix allParams, output;
	model = tc_CopasiModel();
	start = tc_getMatrixValue(input, 0, 0); //start time
	end = tc_getMatrixValue(input, 1, 0); //end time
	numpoints = 	tc_getMatrixValue(input, 2, 0); //num points
	
	if (tc_getMatrixValue(input, 4, 0) > 0) // sliders?
	{
		allParams = getSliderTable();
		tc_createSliders( allParams, &hybrid );
		tc_deleteMatrix(allParams);
	}
	else
	{
		output = simulateHybrid(model, start, end, numpoints);
		tc_plot(output, "Hybrid simulation");
	}
	tc_deleteMatrix(input);
	tc_deleteMatrix(output);
}

void state(tc_matrix params)
{
	int i;
	long p;
	tc_matrix ss;
	setSliderValues(params);
	ss = getSteadyState(model);	
	for (i=0; i < ss.rows; ++i)
	{
		p = tc_find(tc_getRowName(ss, i));
		if (p)
			tc_displayNumber(p, tc_getMatrixValue(ss, i, 0));  //display steady state on the screen
	}
	tc_deleteMatrix(params);
	tc_deleteMatrix(ss);
}

void stateScan(tc_matrix params)
{	
}

void stateScan2D(tc_matrix params)
{	
}

void call_stateScan(tc_matrix input)
{
}

void call_stateScan2D(tc_matrix input)
{
}

void jac(tc_matrix params)
{
	tc_matrix J;
	setSliderValues(params);
	J = getJacobian(model);	
	tc_scatterplot(J, "Jacobian at steady state");
	tc_deleteMatrix(J);
	tc_deleteMatrix(params);
}

void eigen(tc_matrix params)
{
	tc_matrix E;
	setSliderValues(params);
	E = getEigenvalues(model);	
	tc_scatterplot(E, "Eigenvalues at steady state");
	tc_deleteMatrix(E);
	tc_deleteMatrix(params);
}

void setup_ode()
{
	tc_matrix m;
	char * cols[] = { "value",0 };
	char * rows[] = { "start time", "end time", "num. points", "update model", "use sliders", 0 };
	double values[] = { 0, 10, 100, 1, 1 };
	char * options[] = { "Yes", "No"};
	tc_strings s = {2,options};

	m.rows = m.rownames.length = 5;
	m.cols = m.colnames.length =  1;
	m.colnames.strings = cols;
	m.rownames.strings = rows;
	m.values = values;

	tc_createInputWindow(m,"Deterministic simulation", &call_ode);
	tc_addInputWindowOptions("Deterministic simulation",3, 0,  s);
	tc_addInputWindowOptions("Deterministic simulation",4, 0,  s);
}

void setup_ssa()
{
	tc_matrix m;
	char * cols[] = { "value",0 };
	char * rows[] = { "start time", "end time", "num. points", "update model", "use sliders", 0 };
	double values[] = { 0, 10, 100, 1, 1 };
	char * options[] = { "Yes", "No"};
	tc_strings s = {2,options};

	m.rows = m.rownames.length = 5;
	m.cols = m.colnames.length =  1;
	m.colnames.strings = cols;
	m.rownames.strings = rows;
	m.values = values;

	tc_createInputWindow(m,"Exact stochastic simulation", &call_ssa);
	tc_addInputWindowOptions("Exact stochastic simulation",3, 0,  s);
	tc_addInputWindowOptions("Exact stochastic simulation",4, 0,  s);
}

void setup_tauleap()
{
	tc_matrix m;
	char * cols[] = { "value",0 };
	char * rows[] = { "start time", "end time", "num. points", "update model", "use sliders", 0 };
	double values[] = { 0, 10, 100, 1, 1 };
	char * options[] = { "Yes", "No"};
	tc_strings s = {2,options};

	m.rows = m.rownames.length = 5;
	m.cols = m.colnames.length =  1;
	m.colnames.strings = cols;
	m.rownames.strings = rows;
	m.values = values;

	tc_createInputWindow(m,"Tau-leap algorithm", &call_tauleap);
	tc_addInputWindowOptions("Tau-leap algorithm",3, 0,  s);
	tc_addInputWindowOptions("Tau-leap algorithm",4, 0,  s);
}

void setup_hybrid()
{
	tc_matrix m;
	char * cols[] = { "value",0 };
	char * rows[] = { "start time", "end time", "num. points", "update model", "use sliders", 0 };
	double values[] = { 0, 10, 100, 1, 1 };
	char * options[] = { "Yes", "No"};
	tc_strings s = {2,options};

	m.rows = m.rownames.length = 5;
	m.cols = m.colnames.length =  1;
	m.colnames.strings = cols;
	m.rownames.strings = rows;
	m.values = values;

	tc_createInputWindow(m,"Hybrid algorithm", &call_hybrid);
	tc_addInputWindowOptions("Hybrid algorithm",3, 0,  s);
	tc_addInputWindowOptions("Hybrid algorithm",4, 0,  s);
}

void callback()
{
	tc_matrix allParams = getSliderTable();
	tc_addInputWindowOptions("Steady state scan",0, 0, allParams.rownames);
	tc_addInputWindowOptions("2D Steady state scan",0, 0, allParams.rownames);
	tc_addInputWindowOptions("2D Steady state scan",4, 0, allParams.rownames);
	tc_deleteMatrix(allParams);
}

void setup_stateScan()
{
	tc_matrix m;
	char * cols[] = { "value",0 };
	char * rows[] = { "parameter", "start", "end", "num. points", "use sliders", 0 };
	double values[] = { 0, 0, 10, 10, 1 };
	char * options[] = { "Yes", "No"};
	tc_strings s = {2,options};
	tc_matrix allParams = getSliderTable();
	
	m.rows = m.rownames.length = 5;
	m.cols = m.colnames.length =  1;
	m.colnames.strings = cols;
	m.rownames.strings = rows;
	m.values = values;

	tc_createInputWindow(m,"Steady state scan", &call_stateScan);
	tc_addInputWindowOptions("Steady state scan",0, 0,  allParams.rownames);
	tc_addInputWindowOptions("Steady state scan",4, 0,  s);
	tc_deleteMatrix(allParams);
	tc_callback(&callback);
}


void setup_stateScan2D()
{
	tc_matrix m;
	char * cols[] = { "value",0 };
	char * rows[] = { "parameter 1", "start", "end", "num. points", "parameter 2", "start", "end", "num. points", "use sliders", 0 };
	double values[] = { 0, 0, 10, 10, 1, 0, 10, 10, 1 };
	char * options[] = { "Yes", "No"};
	tc_strings s = {2,options};
	tc_matrix allParams = getSliderTable();
	
	m.rows = m.rownames.length = 9;
	m.cols = m.colnames.length =  1;
	m.colnames.strings = cols;
	m.rownames.strings = rows;
	m.values = values;

	tc_createInputWindow(m,"2D Steady state scan", &call_stateScan2D);
	tc_addInputWindowOptions("2D Steady state scan",0, 0,  allParams.rownames);
	tc_addInputWindowOptions("2D Steady state scan",8, 0,  s);
	tc_deleteMatrix(allParams);
	tc_callback(&callback);
}

void setup_state()
{
	tc_matrix allParams = getSliderTable();
	tc_createSliders(allParams, &state);
	tc_deleteMatrix(allParams);
}

void setup_jac()
{
	tc_matrix allParams = getSliderTable();
	tc_createSliders(allParams, &jac);
	tc_deleteMatrix(allParams);
}

void setup_eigen()
{
	tc_matrix allParams = getSliderTable();
	tc_createSliders(allParams, &eigen);
	tc_deleteMatrix(allParams);
}

TCAPIEXPORT void tc_main()
{
	//add function to menu. args : function, name, description, category, icon file, target part/connection family, in functions list?, in context menu? default?
	tc_addFunction(&setup_ode, "Deterministic simulation", "uses COPASI", "Simulate", "cvode.png", "", 1, 0, 1);
	tc_addFunction(&setup_ssa, "Stochastic simulation (exact)", "uses COPASI", "Simulate", "stochastic.png", "", 1, 0, 0);
	tc_addFunction(&setup_tauleap, "Stochastic simulation (Tau-leap)", "uses COPASI", "Simulate", "stochastic.png", "", 1, 0, 0);
	tc_addFunction(&setup_hybrid, "Hybrid simulation", "uses COPASI", "Simulate", "cvode.png", "", 1, 0, 0);
	tc_addFunction(&setup_state, "Get steady state", "uses COPASI", "Steady state", "steadystate.png", "", 1, 0, 0);
	tc_addFunction(&setup_jac, "Jacobian", "uses COPASI", "Steady state", "tabasco_like.png", "", 1, 0, 0);
	tc_addFunction(&setup_eigen, "Eigenvalues", "uses COPASI", "Steady state", "tabasco_like.png", "", 1, 0, 0);
	tc_addFunction(&setup_stateScan, "Parameter scan", "uses COPASI", "Steady state", "steadystate.png", "", 1, 0, 0);
	tc_addFunction(&setup_stateScan2D, "2D Parameter scan", "uses COPASI", "Steady state", "steadystate.png", "", 1, 0, 0);
}


