#include <stdlib.h>
#include <stdio.h>
#include "copasi_api.h"

copasi_model comk_coms();

int main()
{
	copasi_model m = comk_coms();

	tc_matrix output = simulateDeterministic(m, 0, 200000, 5000);  //model, start, end, num. points
	//tc_matrix output = simulateTauLeap(m, 0, 200000, 5000);  //model, start, end, num. points
	tc_printMatrixToFile("output.tab", output);	
	tc_deleteMatrix(output);
	printf("\noutput.tab contains the final output\n\n");
	removeCopasiModel(m);

	//cleanup
	copasi_end();
	return 0;
}


copasi_model comk_coms()
{
	copasi_reaction r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13;
	copasi_compartment DefaultCompartment;
	copasi_model model = createCopasiModel("ComK");
	DefaultCompartment = createCompartment(model,"DefaultCompartment",1);
	createSpecies(DefaultCompartment,"comk_mrna",0);
	createSpecies(DefaultCompartment,"ComK",20000);
	createSpecies(DefaultCompartment,"comk",0);
	createSpecies(DefaultCompartment,"coms_mrna",0);
	createSpecies(DefaultCompartment,"ComS",10);
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

