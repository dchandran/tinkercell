#include <stdlib.h>
#include <stdio.h>
#include "copasi_api.h"

copasi_model model1(); //oscillation
copasi_model model2(); //positive feebdack gene regulation
copasi_model model3(); //from tinkercell
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
	
	m2 = model3();
	//tc_matrix output = simulateDeterministic(m2, 0, 200, 100);  //model, start, end, num. points
	tc_matrix output = simulateTauLeap(m2, 0, 20000, 2000);  //model, start, end, num. points
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

copasi_model model3()
{
	copasi_reaction r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13;
	copasi_compartment DefaultCompartment;
	copasi_model model = createCopasiModel("ComK");
	DefaultCompartment = createCompartment(model,"DefaultCompartment",1);
	createSpecies(DefaultCompartment,"comk_mrna",0);
	createSpecies(DefaultCompartment,"ComK",2000);
	createSpecies(DefaultCompartment,"comk",0);
	createSpecies(DefaultCompartment,"coms_mrna",0);
	createSpecies(DefaultCompartment,"ComS",100);
	createSpecies(DefaultCompartment,"coms",0);
	createSpecies(DefaultCompartment,"coms_meka_complex",0);
	createSpecies(DefaultCompartment,"MekA",500);
	createSpecies(DefaultCompartment,"comk_meka_complex",0);
	createSpecies(DefaultCompartment,"pComK",0);
	createSpecies(DefaultCompartment,"pComS",0);
	setGlobalParameter(model,"k1",0.00022);
	setGlobalParameter(model,"k2",0.19);
	setGlobalParameter(model,"k3",0.2);
	setGlobalParameter(model,"k4",0);
	setGlobalParameter(model,"k5",0.0015);
	setGlobalParameter(model,"k6",0.2);
	setGlobalParameter(model,"k7",0.005);
	setGlobalParameter(model,"k8",0.0001);
	setGlobalParameter(model,"k9",0.005);
	setGlobalParameter(model,"k10",0.0001);
	setGlobalParameter(model,"k11",2e-06);///6.0221415e23);
	setGlobalParameter(model,"k12",0.05);
	setGlobalParameter(model,"k13",4.5e-06);///6.0221415e23);
	setGlobalParameter(model,"k14",4e-05);
	setGlobalParameter(model,"k11n",0.0005);
	setGlobalParameter(model,"k13n",5e-05);
	setGlobalParameter(model,"kk",5000);
	setGlobalParameter(model,"ks",833);
	setGlobalParameter(model,"n",2);
	setGlobalParameter(model,"p",5);
	setAssignmentRule(model, "comk","k1+pComK");
	setAssignmentRule(model, "coms","k4+pComS");
	setAssignmentRule(model, "pComK","k2*(ComK^n)/(kk^n+ComK^n)");
	setAssignmentRule(model, "pComS","k5/(1+((ComK/ks)^p))");
	r0 = createReaction(model, "comk_mrna_degradation");
	setReactionRate(r0,"k7*comk_mrna");
	addReactant(r0,"comk_mrna",1);
	r1 = createReaction(model, "comk_protein_degradation");
	setReactionRate(r1,"k8*ComK");
	addReactant(r1,"ComK",1);
	r2 = createReaction(model, "comk_transcription");
	setReactionRate(r2,"comk");
	addProduct(r2,"comk_mrna",1);
	r3 = createReaction(model, "comk_translation");
	setReactionRate(r3,"k3*comk_mrna");
	addProduct(r3,"ComK",1);
	r4 = createReaction(model, "coms_mrna_degradation");
	setReactionRate(r4,"k9*coms_mrna");
	addReactant(r4,"coms_mrna",1);
	r5 = createReaction(model, "coms_protein_degradation");
	setReactionRate(r5,"k10*ComS");
	addReactant(r5,"ComS",1);
	r6 = createReaction(model, "coms_transcription");
	setReactionRate(r6,"coms");
	addProduct(r6,"coms_mrna",1);
	r7 = createReaction(model, "coms_translation");
	setReactionRate(r7,"k6*coms_mrna");
	addProduct(r7,"ComS",1);
	r8 = createReaction(model, "coms_meka_deg");
	setReactionRate(r8,"k14*coms_meka_complex");
	addReactant(r8,"coms_meka_complex",1);
	addProduct(r8,"MekA",1);
	r9 = createReaction(model, "coms_meka_forward");
	setReactionRate(r9,"k13*ComS*MekA");
	addReactant(r9,"ComS",1);
	addProduct(r9,"coms_meka_complex",1);
	addReactant(r9,"MekA",1);
	r10 = createReaction(model, "coms_meka_reverse");
	setReactionRate(r10,"k13n*coms_meka_complex");
	addProduct(r10,"ComS",1);
	addReactant(r10,"coms_meka_complex",1);
	addProduct(r10,"MekA",1);
	r11 = createReaction(model, "comk_meka_deg");
	setReactionRate(r11,"k12*comk_meka_complex");
	addProduct(r11,"MekA",1);
	addReactant(r11,"comk_meka_complex",1);
	r12 = createReaction(model, "comk_meka_forward");
	setReactionRate(r12,"k11*ComK*MekA");
	addReactant(r12,"ComK",1);
	addReactant(r12,"MekA",1);
	addProduct(r12,"comk_meka_complex",1);
	r13 = createReaction(model, "comk_meka_reverse");
	setReactionRate(r13,"k11n*comk_meka_complex");
	addProduct(r13,"ComK",1);
	addProduct(r13,"MekA",1);
	addReactant(r13,"comk_meka_complex",1);
	return model;
}
