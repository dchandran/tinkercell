#include <stdlib.h>
#include <stdio.h>
#include "copasi_api.h"

copasi_model model1(); //oscillation
copasi_model model2(); //positive feebdack gene regulation
void eigen(copasi_model, const char*); //compute eigenvalues by changing parameters (similar to root-locus)

int main()
{
	tc_matrix efm, output, params;
	copasi_model m1, m2;
	
	m1 = model1();
	output = cSimulateTauLeap(m1, 0, 100, 200);  //model, start, end, num. points
	tc_printMatrixToFile("output.tab", output);	
	tc_deleteMatrix(output);
	
	params = tc_createMatrix(3,3);
	tc_setRowName(params,0,"k1");
	tc_setRowName(params,1,"k2");
	tc_setRowName(params,2,"k3");
	tc_setMatrixValue(params, 0, 0, 0.1);
	tc_setMatrixValue(params, 0, 1, 0.0);
	tc_setMatrixValue(params, 0, 2, 1.0);
	tc_setMatrixValue(params, 1, 0, 0.2);
	tc_setMatrixValue(params, 1, 1, 0.0);
	tc_setMatrixValue(params, 1, 2, 1.0);
	tc_setMatrixValue(params, 2, 0, 0.3);
	tc_setMatrixValue(params, 2, 1, 0.0);
	tc_setMatrixValue(params, 2, 2, 1.0);
	
	cSetValue(m1,"k1",1.0);
	cSetValue(m1,"k2",1.0);
	cSetValue(m1,"k3",1.0);
	
	cFitModelToData(m1, "output.tab", params, "NelderMead");
	
	//tc_printMatrixToFile("params.out", params);

	//cleanup
	tc_deleteMatrix(output);
	cRemoveModel(m1);
	copasi_end();
	return 0;
}

copasi_model model1()
{
	//model named M
	copasi_model model = cCreateModel("M");
	copasi_reaction R1, R2, R3;
	
	//species
	copasi_compartment cell = cCreateCompartment(model, "cell", 1.0);
	copasi_compartment out = cCreateCompartment(model, "out", 1.0);
	cCreateSpecies(cell, "A", 2);
	cCreateSpecies(out, "A", 1);
	cCreateSpecies(cell, "C", 3);
	
	//parameters	
	cSetValue(model, "k1", 0.1);   //k1
	cSetValue(model, "k2", 0.2);   //k2
	cSetValue(model, "k3", 0.3);   //k3
	
	//reactions -- make sure all parameters or species are defined BEFORE this step
	R1 = cCreateReaction(model, "R1");  // A+B -> 2B
	
	cAddReactant(R1, "cell_A", 1.0);
	cAddReactant(R1, "out_A", 1.0);
	cAddProduct(R1, "out_A", 2.0);
	cSetReactionRate(R1, "k1*cell_A*out_A");

	R2 = cCreateReaction(model, "R2");  //B+C -> 2C
	cAddReactant(R2, "out_A", 1.0);
	cAddReactant(R2, "C", 1.0);
	cAddProduct(R2, "C", 2.0);
	cSetReactionRate(R2, "k2*out_A*C");

	R3 = cCreateReaction(model, "R3"); //C+A -> 2A
	cAddReactant(R3, "C", 1.0);
	cAddReactant(R3, "cell_A", 1.0);
	cAddProduct(R3, "cell_A", 2.0);
	cSetReactionRate(R3, "k3*C*cell_A");

	//assignment rule -- make sure all parameters or species are defined BEFORE this step
	//cCreateVariable(model, "prod1","sin(time)");
	//cCreateVariable(model, "prod2","prod1 * prod1");
	//cCreateEvent(model, "event1", "ge(Time,5)", "C", "C/2.0");
	return model;
}

copasi_model model2()
{
	//model named M
	copasi_model model = cCreateModel("M");
	copasi_compartment cell;
	copasi_reaction R1, R2, R3, R4;
	
	//species
	cell = cCreateCompartment(model, "cell", 1.0);
	cCreateSpecies(cell, "mRNA", 0);
	cCreateSpecies(cell, "Protein", 0);
	
	//parameters	
	cSetValue(model, "d1", 1.0);
	cSetValue(model, "d2", 0.2);  
	cSetValue(model, "k0", 2.0);
	cSetValue(model, "k1", 1.0);
	cSetValue(model, "h", 4.0);  
	cSetValue(model, "Kd", 1.0);
	cSetValue(model, "leak", 0.1);  
	
	//reactions -- make sure all parameters or species are defined BEFORE this step
	R1 = cCreateReaction(model, "R1");  //  mRNA production
	cAddProduct(R1, "mRNA", 1.0);
	cSetReactionRate(R1, "leak + k0 * (Protein^h) / (Kd + (Protein^h))");

	R2 = cCreateReaction(model, "R2");  // Protein production
	cAddProduct(R2, "Protein", 1.0);
	cSetReactionRate(R2, "k1*mRNA");

	R3 = cCreateReaction(model, "R3"); // mRNA degradation
	cAddReactant(R3, "mRNA", 1.0);
	cSetReactionRate(R3, "d1*mRNA");
	
	R4 = cCreateReaction(model, "R4"); // Protein degradation
	cAddReactant(R4, "Protein", 1.0);
	cSetReactionRate(R4, "d2*Protein");
	return model;
}

// eigenvalues
void eigen(copasi_model model, const char* param)
{
	int i, j,k;
	double p;
	FILE * outfile;
	tc_matrix ss;
	tc_matrix output;
	
	//steady states
	
	for (i=0; i < 100; ++i)
	{
		p = (double)(i + 1)/10.0;
		k = cSetValue( model, param, p );
		
		if (k)
			printf("calculating steady state for %s = %lf\n",param, p);
		
		ss = cGetEigenvalues(model);
		//ss = cGetSteadyState(model);

		if (i == 0)
		{
			output = tc_createMatrix(100, ss.rows+1);
			tc_setColumnName(output, 0, param);
			for (j=0; j < output.cols; ++j)
				tc_setColumnName(output, j+1, tc_getRowName(ss, j));
		}
		
		tc_setMatrixValue(output, i, 0, p);
		for (j=0; j < output.cols; ++j)
			tc_setMatrixValue(output, i, j+1, tc_getMatrixValue(ss, j, 0));
		
		tc_deleteMatrix(ss);
	}
	
	//output
	tc_printMatrixToFile("output.tab", output);
	
	printf("\noutput.tab contains the final output\n\n");

	tc_deleteMatrix(output);
}
