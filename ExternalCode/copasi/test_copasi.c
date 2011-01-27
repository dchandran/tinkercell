#include <stdlib.h>
#include <stdio.h>
#include "copasi_api.h"

copasi_model model1(); //oscillation
copasi_model model2(); //positive feebdack gene regulation
void eigen(copasi_model, const char*); //compute eigenvalues by changing parameters (similar to root-locus)

int main()
{
	tc_matrix efm = tc_createMatrix(0,0);
	copasi_model m1, m2;
	
	m1 = readSBMLFile("bug.xml");
	if (m1.errorMessage)
	{
		printf("%s\n", m1.errorMessage);
	}
	
	efm = getGammaMatrix(m1);
	tc_printOutMatrix(efm);
	removeCopasiModel(m1);
	
	m2 = model1();
	//tc_matrix output = simulateDeterministic(m2, 0, 200, 100);  //model, start, end, num. points
	tc_matrix output = simulateTauLeap(m2, 0, 200000, 5000);  //model, start, end, num. points
	tc_printMatrixToFile("output.tab", output);	
	tc_deleteMatrix(output);
	printf("\noutput.tab contains the final output\n\n");
	removeCopasiModel(m2);

	//cleanup
	tc_deleteMatrix(efm);
	copasi_end();
	return 0;
}

copasi_model model1()
{
	//model named M
	copasi_model model = createCopasiModel("M");
	copasi_reaction R1, R2, R3;
	
	//species
	copasi_compartment cell = createCompartment(model, "cell", 1.0);
	copasi_compartment out = createCompartment(model, "out", 1.0);
	createSpecies(cell, "A", 2);
	createSpecies(out, "A", 1);
	createSpecies(cell, "C", 3);
	
	//parameters	
	setValue(model, "k1", 0.1);   //k1
	setValue(model, "k2", 0.2);   //k2
	setValue(model, "k3", 0.3);   //k3
	
	//reactions -- make sure all parameters or species are defined BEFORE this step
	R1 = createReaction(model, "R1");  // A+B -> 2B
	
	addReactant(R1, "cell_A", 1.0);
	addReactant(R1, "out_A", 1.0);
	addProduct(R1, "out_A", 2.0);
	setReactionRate(R1, "k1*cell_A*out_A");

	R2 = createReaction(model, "R2");  //B+C -> 2C
	addReactant(R2, "out_A", 1.0);
	addReactant(R2, "C", 1.0);
	addProduct(R2, "C", 2.0);
	setReactionRate(R2, "k2*out_A*C");

	R3 = createReaction(model, "R3"); //C+A -> 2A
	addReactant(R3, "C", 1.0);
	addReactant(R3, "cell_A", 1.0);
	addProduct(R3, "cell_A", 2.0);
	setReactionRate(R3, "k3*C*cell_A");

	//assignment rule -- make sure all parameters or species are defined BEFORE this step
	createVariable(model, "prod1","sin(time)");
	createVariable(model, "prod2","prod1 * prod1");
	//createEvent(model, "event1", "ge(Time,5)", "C", "C/2.0");
	return model;
}

copasi_model model2()
{
	//model named M
	copasi_model model = createCopasiModel("M");
	copasi_compartment cell;
	copasi_reaction R1, R2, R3, R4;
	
	//species
	cell = createCompartment(model, "cell", 1.0);
	createSpecies(cell, "mRNA", 0);
	createSpecies(cell, "Protein", 0);
	
	//parameters	
	setValue(model, "d1", 1.0);
	setValue(model, "d2", 0.2);  
	setValue(model, "k0", 2.0);
	setValue(model, "k1", 1.0);
	setValue(model, "h", 4.0);  
	setValue(model, "Kd", 1.0);
	setValue(model, "leak", 0.1);  
	
	//reactions -- make sure all parameters or species are defined BEFORE this step
	R1 = createReaction(model, "R1");  //  mRNA production
	addProduct(R1, "mRNA", 1.0);
	setReactionRate(R1, "leak + k0 * (Protein^h) / (Kd + (Protein^h))");

	R2 = createReaction(model, "R2");  // Protein production
	addProduct(R2, "Protein", 1.0);
	setReactionRate(R2, "k1*mRNA");

	R3 = createReaction(model, "R3"); // mRNA degradation
	addReactant(R3, "mRNA", 1.0);
	setReactionRate(R3, "d1*mRNA");
	
	R4 = createReaction(model, "R4"); // Protein degradation
	addReactant(R4, "Protein", 1.0);
	setReactionRate(R4, "d2*Protein");
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
		k = setValue( model, param, p );
		
		if (k)
			printf("calculating steady state for %s = %lf\n",param, p);
		
		ss = getEigenvalues(model);
		//ss = getSteadyState(model);

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
