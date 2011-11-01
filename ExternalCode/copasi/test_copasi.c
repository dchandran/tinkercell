#include <stdlib.h>
#include <stdio.h>
#include "copasi_api.h"

copasi_model model(); 

int main()
{
	tc_matrix efm, output, params, iv;
	copasi_model m;
	
	printf("creating model...\n");
	m = model();
	cCompileModel(m);
    
	printf("simulating...\n");
	output = cSimulateDeterministic(m, 0, 1, 2);  //model, start, end, num. points
	tc_deleteMatrix(output);	
	output = cSimulateDeterministic(m, 0, 20, 100);  //model, start, end, num. points
	iv = cGetFloatingSpeciesIntitialConcentrations(m);
	tc_printMatrixToFile("iv.txt",iv);
	printf("output.tab has %i rows and %i columns\n",output.rows, output.cols);
	tc_printMatrixToFile("output.tab", output);
	tc_deleteMatrix(output);

	//cleanup	
	cRemoveModel(m);
	copasi_end();
	return 0;
}

copasi_model model() //big genetic model
{
	copasi_model model = cCreateModel("M");
	copasi_compartment DefaultCompartment, cel1;
	copasi_reaction r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17,r18,r19,r20,r21,r22,r23,r24,r25,r26,r27,r28,r29,r30,r31,r32,r33,r34,r35;
	DefaultCompartment = cCreateCompartment(model,"DefaultCompartment",1);
	cel1 = cCreateCompartment(model,"cel1",1);
	cCreateSpecies(cel1,"cel1_LacI",10);
	cCreateSpecies(cel1,"cel1_LacZ",0);
	cCreateSpecies(cel1,"cel1_ai1_Inactive",0);
	cCreateSpecies(cel1,"cel1_cod1",0);
	cCreateSpecies(cel1,"cel1_lac",10);
	cCreateSpecies(cel1,"cel1_pro1",0);
	cCreateSpecies(cel1,"cel1_rbs1",0);
	cCreateSpecies(cel1,"cel1_rs1",0);
	cCreateSpecies(DefaultCompartment,"lac",10);
	cSetSpeciesType(model, "lac",1);
	cSetGlobalParameter(model,"cc1_k0",0.0072);
	cSetGlobalParameter(model,"cel1_ai1_Kd",0.1);
	cSetGlobalParameter(model,"cel1_cc1_Kcat",0.1);
	cSetGlobalParameter(model,"cel1_cc1_Km",1);
	cSetGlobalParameter(model,"cel1_pp1_degradation_rate",0.1);
	cSetGlobalParameter(model,"cel1_pro1_strength",5);
	cSetGlobalParameter(model,"cel1_tr1_Kd",0.0083);
	cSetGlobalParameter(model,"cel1_tr1_h",2);
	cSetAssignmentRule(model, "cel1_cod1","cel1_pro1_strength * (cel1_rs1)");
	cSetAssignmentRule(model, "cel1_rs1","1.0/((1+((cel1_LacI/cel1_tr1_Kd)^cel1_tr1_h)))");
	r0 = cCreateReaction(model, "cc1");
	cSetReactionRate(r0,"cc1_k0*cel1_lac");
	cAddReactant(r0,"cel1_lac",1);
	cAddProduct(r0,"lac",1);
	r1 = cCreateReaction(model, "cel1_ai1_back");
	cSetReactionRate(r1,"cel1_ai1_Inactive");
	cAddProduct(r1,"cel1_LacI",1);
	cAddReactant(r1,"cel1_ai1_Inactive",1);
	r2 = cCreateReaction(model, "cel1_ai1_forward");
	cSetReactionRate(r2,"1/cel1_ai1_Kd * cel1_LacI*cel1_lac");
	cAddReactant(r2,"cel1_LacI",1);
	cAddProduct(r2,"cel1_ai1_Inactive",1);
	r3 = cCreateReaction(model, "cel1_cc1");
	cSetReactionRate(r3,"cel1_cc1_Kcat * cel1_LacZ * lac/( cel1_cc1_Km + lac)");
	cAddProduct(r3,"cel1_lac",1);
	cAddReactant(r3,"lac",1);
	r4 = cCreateReaction(model, "cel1_pp1_v1");
	cSetReactionRate(r4,"cel1_cod1");
	cAddProduct(r4,"cel1_LacZ",1);
	r5 = cCreateReaction(model, "cel1_pp1_v2");
	cSetReactionRate(r5,"cel1_pp1_degradation_rate*cel1_LacZ");
	cAddReactant(r5,"cel1_LacZ",1);
	return model;
}




