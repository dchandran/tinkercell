#include <stdlib.h>
#include <stdio.h>
#include "copasi_api.h"

copasi_model model1(); //oscillation
copasi_model model2(); //positive feebdack gene regulation
copasi_model model3();
void eigen(copasi_model, const char*); //compute eigenvalues by changing parameters (similar to root-locus)

int main()
{
	tc_matrix efm, output, params;
	copasi_model m1, m2;
	
	m1 = model1();
    //m1 = cReadSBMLFile("model1.sbml");
    
	output = cSimulateDeterministic(m1, 0, 10, 100);  //model, start, end, num. points
	printf("rows = %i\n",output.rows);
	tc_printMatrixToFile("output.tab", output);
	tc_deleteMatrix(output);
	//printf("%s\n",m1.errorMessage);
	/*params = tc_createMatrix(3,3);
	tc_setRowName(params,0,"k1");
	tc_setRowName(params,1,"k2");
	tc_setRowName(params,2,"k3");
	tc_setMatrixValue(params, 0, 0, 1);
	tc_setMatrixValue(params, 0, 1, 0.0);
	tc_setMatrixValue(params, 0, 2, 5.0);
	tc_setMatrixValue(params, 1, 0, 1);
	tc_setMatrixValue(params, 1, 1, 0.0);
	tc_setMatrixValue(params, 1, 2, 5.0);
	tc_setMatrixValue(params, 2, 0, 1);
	tc_setMatrixValue(params, 2, 1, 0.0);
	tc_setMatrixValue(params, 2, 2, 5.0);
	
	cSetValue(m1,"k1",2.0);
	cSetValue(m1,"k2",1.0);
	cSetValue(m1,"k3",1.0);*/
	
	//cSetOptimizerIterations(10);
	//output = cOptimize(m1, "output.tab", params);
	//tc_printMatrixToFile("params.out", output);
	//tc_deleteMatrix(output);

	//cleanup	
	cRemoveModel(m1);
	copasi_end();
	return 0;
}

copasi_model model1()
{
	//model named M
	copasi_model model = cCreateModel("M");
	copasi_reaction R1, R2, R3;
	copasi_compartment cell;
	
	//species
	cell = cCreateCompartment(model, "cell", 1.0);
	cCreateSpecies(cell, "A", 4);
	cCreateSpecies(cell, "B", 3);
	cCreateSpecies(cell, "C", 2);
	
	//parameters
	cSetValue(model, "k1", 1);   //k1
	cSetValue(model, "k2", 1);   //k2
	cSetValue(model, "k3", 1);   //k3
	
	//reactions -- make sure all parameters or species are defined BEFORE this step
	R1 = cCreateReaction(model, "R1");  // A+B -> 2B
	cAddReactant(R1, "A", 1.0);
	cAddReactant(R1, "B", 1.0);
	cAddProduct(R1, "B", 2.0);
	cSetReactionRate(R1, "k1*A*B");

	R2 = cCreateReaction(model, "R2");  //B+C -> 2C
	cAddReactant(R2, "B", 1.0);
	cAddReactant(R2, "C", 1.0);
	cAddProduct(R2, "C", 2.0);
	cSetReactionRate(R2, "k2*B*C");

	R3 = cCreateReaction(model, "R3"); //C+A -> 2A
	cAddReactant(R3, "C", 1.0);
	cAddReactant(R3, "A", 1.0);
	cAddProduct(R3, "A", 2.0);
	cSetReactionRate(R3, "k3*C*A");

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

copasi_model model3()
{
	copasi_model model = cCreateModel("M");
	copasi_compartment DefaultCompartment;
	copasi_reaction r0,r1,r2,r3;
DefaultCompartment = cCreateCompartment(model,"DefaultCompartment",1);
cCreateSpecies(DefaultCompartment,"dr1_Monomer",0);
cCreateSpecies(DefaultCompartment,"rs2",1);
cCreateSpecies(DefaultCompartment,"cod1",1);
cCreateSpecies(DefaultCompartment,"OUTPUT",5);
cCreateSpecies(DefaultCompartment,"cod2",1);
//cSetGlobalParameter(model,"INPUT",12);
cCreateSpecies(DefaultCompartment,"INPUT",5);
cCreateSpecies(DefaultCompartment,"as1",1);
cCreateSpecies(DefaultCompartment,"as2",1);
cCreateSpecies(DefaultCompartment,"pro1",1);
cCreateSpecies(DefaultCompartment,"pro2",1);
cCreateSpecies(DefaultCompartment,"rbs1",1);
cCreateSpecies(DefaultCompartment,"rbs2",1);
cCreateSpecies(DefaultCompartment,"ter1",1);
cCreateSpecies(DefaultCompartment,"ter2",1);
cSetGlobalParameter(model,"OUTPUT_degradation_rate",0.1);
cSetGlobalParameter(model,"dr1_degradation_rate",0.1);
cSetGlobalParameter(model,"dr1_Kd",12);
cSetGlobalParameter(model,"dr1_h",4);
cSetGlobalParameter(model,"pro1_strength",5);
cSetGlobalParameter(model,"pro2_strength",12);
cSetGlobalParameter(model,"ta1_Kd",5);
cSetGlobalParameter(model,"ta1_h",4);
cSetGlobalParameter(model,"ta2_Kd",2);
cSetGlobalParameter(model,"ta2_h",5);
cSetAssignmentRule(model, "as1","((1+((INPUT/ta1_Kd)^ta1_h))-1)/((1+((INPUT/ta1_Kd)^ta1_h)))");
cSetAssignmentRule(model, "as2","((1+((INPUT/ta2_Kd)^ta2_h))-1)/((1+((INPUT/ta2_Kd)^ta2_h)))");
cSetAssignmentRule(model, "cod1","pro1_strength * (as1)");
cSetAssignmentRule(model, "cod2","pro2_strength * (( as1 + as2) *(rs2))");
cSetAssignmentRule(model, "rs2","1/(dr1_Kd+dr1_Monomer^dr1_h)");
r0 = cCreateReaction(model, "dr1_v1");
cSetReactionRate(r0,"cod1");
cAddProduct(r0,"dr1_Monomer",1);
r1 = cCreateReaction(model, "dr1_v2");
cSetReactionRate(r1,"dr1_degradation_rate*dr1_Monomer");
cAddReactant(r1,"dr1_Monomer",1);
r2 = cCreateReaction(model, "pp1_v1");
cSetReactionRate(r2,"cod2");
cAddProduct(r2,"OUTPUT",1);
r3 = cCreateReaction(model, "pp1_v2");
cSetReactionRate(r3,"OUTPUT_degradation_rate*OUTPUT");
cAddReactant(r3,"OUTPUT",1);
//cSetBoundarySpecies(model, "INPUT",1);
	return model;
}




