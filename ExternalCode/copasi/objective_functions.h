#ifndef COPASI_API_OBJECTIVE_FUNCTIONS
#define COPASI_API_OBJECTIVE_FUNCTIONS
#include "copasi_api.h"

/**************************************************
          List of available optimization functions
***************************************************/
double fit_time_course(copasi_model , tc_matrix );
double fit_input_output(copasi_model , tc_matrix );
double bimodality(copasi_model , tc_matrix );
double oscillation(copasi_model , tc_matrix );
double nonmonotonicity(copasi_model , tc_matrix );
double adaptive_response(copasi_model , tc_matrix );
double coefficient_of_variation(copasi_model , tc_matrix );

/***************************************************
 Data structure for storing function name and pointer
***************************************************/
typedef struct 
{ 
	const char * name;  //name of objective function
	int minmax;  //-1 for minimize, +1 for maximize
	double (*f)(copasi_model, tc_matrix); //pointer to objective function
} 
CopasiAPIOptimizationFunctionPtr;

/**********************************************
     Add new optimization functions to this array
**********************************************/
CopasiAPIOptimizationFunction 
CopasiAPIOptimizationFunctions[] = 
{
	{"fit time-course data", -1, &fit_time_course},
	{"fit input-output data", -1, &fit_input_output},
	{"bimodal", 1, &bimodality},
	{"oscillation", 1, &oscillation},
	{"non-monotonicity", -1, &nonmonotonicity},
	{"adaptive response", -1, &adaptive_response},
	{"low coefficient of variation", -1, &coefficient_of_variation},
	0
};

/**********************************************
                      Function definitions
**********************************************/

double bimodality(copasi_model model, tc_matrix input)
{
	int bins = 100, i, j, k, i1, i2;
	double minval, maxval, dx, score=0.0;
	int hi1, hi2, lo1;
	tc_matrix result = simulateStochastic(model, tc_getMatrixValue(input, 0, 0), tc_getMatrixValue(input, 0, 1), tc_getMatrixValue(input, 0, 2));
	tc_matrix hist;
	
	hist = tc_createMatrix(100, result.cols()-1);
	dx = 1.0/ (double)results.rows;
	
	for (j=0; j < result.cols; ++j)
	{
		minval = maxval = tc_getMatrixValue(result, j, 0);
		for (i=0; i < result.rows(); ++i)
		{
			if (tc_getMatrixValue(result, j, i) < minval)
				minval = tc_getMatrixValue(result, j, i);
			else
			if (tc_getMatrixValue(result, j, i) > maxval)
				maxval = tc_getMatrixValue(result, j, i);
		}

		for (i=0; i < result.rows; ++i)
		{
			k = (int)(100 * (tc_getMatrixValue(result, i, j) - minval)/(maxval - minval));
			tc_setMatrixValue(hist, k, j, tc_getMatrixValue(hist, k, j) + dx);
		}
		
		//find modes
		//h1 and h2 are the two peaks, and lo1 is the valley between
		
		hi1 = hi2 = lo1 = 0.0;
		for (i=0; i < result.rows; ++i)
		{
			if (tc_getMatrixValue(hist, i, j) > tc_getMatrixValue(hist, hi1, j))
				hi1 = i;
			else
			if (tc_getMatrixValue(hist, i, j) > tc_getMatrixValue(hist, hi2, j))
				hi2 = i;
		}
		
		if (hi1 < hi2)
		{
			i1 = hi1;
			i2 = hi2;
		}
		else
		{
			i1 = hi2;
			i2 = hi1;
		}
		
		lo1 = i1;

		for (i=i1; i < i2; ++i)
		{
			if (tc_getMatrixValue(hist, i, j) < tc_getMatrixValue(hist, lo1, j))
				lo1 = i;
		}
		
		if ((tc_getMatrixValue(hist, hi1, j) - tc_getMatrixValue(hist, lo1, j)) > score)
			score = tc_getMatrixValue(hist, hi1, j) - tc_getMatrixValue(hist, lo1, j);
	}
	
	tc_deleteMatrix(result);
	tc_deleteMatrix(hist);
	
	return 0.0;
}

double oscillation(copasi_model model, tc_matrix target)
{
	return 0.0;
}

double nonmonotonicity(copasi_model model, tc_matrix target)
{
	return 0.0;
}

double adaptive_response(copasi_model model, tc_matrix target)
{
	return 0.0;
}

double coefficient_of_variation(copasi_model model, tc_matrix target)
{
	return 0.0;
}

#endif

