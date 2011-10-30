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
	copasi_compartment DefaultCompartment;
	copasi_reaction r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17,r18,r19,r20,r21,r22,r23,r24,r25,r26,r27,r28,r29,r30,r31,r32,r33,r34,r35;
	DefaultCompartment = cCreateCompartment(model,"DefaultCompartment",1);
	cCreateSpecies(DefaultCompartment,"GFP",0);
	cCreateSpecies(DefaultCompartment,"X",1);
	cCreateSpecies(DefaultCompartment,"Y",5);
	cCreateSpecies(DefaultCompartment,"as1",0);
	cCreateSpecies(DefaultCompartment,"as2",0);
	cCreateSpecies(DefaultCompartment,"cod1",0);
	cCreateSpecies(DefaultCompartment,"if1_as1",1);
	cCreateSpecies(DefaultCompartment,"if1_as2",1);
	cCreateSpecies(DefaultCompartment,"if1_cod1",1);
	cCreateSpecies(DefaultCompartment,"if1_cod2",1);
	cCreateSpecies(DefaultCompartment,"if1_dr1_Monomer",0);
	cCreateSpecies(DefaultCompartment,"if1_pro1",1);
	cCreateSpecies(DefaultCompartment,"if1_pro2",1);
	cCreateSpecies(DefaultCompartment,"if1_rbs1",1);
	cCreateSpecies(DefaultCompartment,"if1_rbs2",1);
	cCreateSpecies(DefaultCompartment,"if1_rs2",1);
	cCreateSpecies(DefaultCompartment,"if1_ter1",1);
	cCreateSpecies(DefaultCompartment,"if1_ter2",1);
	cCreateSpecies(DefaultCompartment,"if2_as1",1);
	cCreateSpecies(DefaultCompartment,"if2_as2",1);
	cCreateSpecies(DefaultCompartment,"if2_cod1",1);
	cCreateSpecies(DefaultCompartment,"if2_cod2",1);
	cCreateSpecies(DefaultCompartment,"if2_dr1_Monomer",0);
	cCreateSpecies(DefaultCompartment,"if2_pro1",1);
	cCreateSpecies(DefaultCompartment,"if2_pro2",1);
	cCreateSpecies(DefaultCompartment,"if2_rbs1",1);
	cCreateSpecies(DefaultCompartment,"if2_rbs2",1);
	cCreateSpecies(DefaultCompartment,"if2_rs2",1);
	cCreateSpecies(DefaultCompartment,"if2_ter1",1);
	cCreateSpecies(DefaultCompartment,"if2_ter2",1);
	cCreateSpecies(DefaultCompartment,"pro1",0);
	cCreateSpecies(DefaultCompartment,"rbs1",0);
	cCreateSpecies(DefaultCompartment,"tf2",0);
	cCreateSpecies(DefaultCompartment,"tf4",0);
	cSetGlobalParameter(model,"GFP_degradation_rate",0.1);
	cSetGlobalParameter(model,"if1_dr1_degradation_rate",0.1);
	cSetGlobalParameter(model,"if1_dr1_Kd",12);
	cSetGlobalParameter(model,"if1_dr1_h",4);
	cSetGlobalParameter(model,"if1_pro1_strength",5);
	cSetGlobalParameter(model,"if1_pro2_strength",12);
	cSetGlobalParameter(model,"if1_ta1_Kd",5);
	cSetGlobalParameter(model,"if1_ta1_h",4);
	cSetGlobalParameter(model,"if1_ta2_Kd",2);
	cSetGlobalParameter(model,"if1_ta2_h",5);
	cSetGlobalParameter(model,"if2_dr1_degradation_rate",0.1);
	cSetGlobalParameter(model,"if2_dr1_Kd",12);
	cSetGlobalParameter(model,"if2_dr1_h",4);
	cSetGlobalParameter(model,"if2_pro1_strength",5);
	cSetGlobalParameter(model,"if2_pro2_strength",12);
	cSetGlobalParameter(model,"if2_ta1_Kd",5);
	cSetGlobalParameter(model,"if2_ta1_h",4);
	cSetGlobalParameter(model,"if2_ta2_Kd",2);
	cSetGlobalParameter(model,"if2_ta2_h",5);
	cSetGlobalParameter(model,"pro1_strength",1);
	cSetGlobalParameter(model,"ta1_Kd",1);
	cSetGlobalParameter(model,"ta1_h",2);
	cSetGlobalParameter(model,"ta2_Kd",1);
	cSetGlobalParameter(model,"ta2_h",2);
	cSetGlobalParameter(model,"tf2_degradation_rate",0.1);
	cSetGlobalParameter(model,"tf4_degradation_rate",0.1);
	cSetAssignmentRule(model, "as1","((1+((tf4/ta2_Kd)^ta2_h))-1)/((1+((tf4/ta2_Kd)^ta2_h)))");
	cSetAssignmentRule(model, "as2","((1+((tf2/ta1_Kd)^ta1_h))-1)/((1+((tf2/ta1_Kd)^ta1_h)))");
	cSetAssignmentRule(model, "cod1","pro1_strength*(as1*as2)");
	cSetAssignmentRule(model, "if1_as1","((1+((X/if1_ta1_Kd)^if1_ta1_h))-1)/((1+((X/if1_ta1_Kd)^if1_ta1_h)))");
	cSetAssignmentRule(model, "if1_as2","((1+((X/if1_ta2_Kd)^if1_ta2_h))-1)/((1+((X/if1_ta2_Kd)^if1_ta2_h)))");
	cSetAssignmentRule(model, "if1_cod1","if1_pro1_strength * (if1_as1)");
	cSetAssignmentRule(model, "if1_cod2","if1_pro2_strength * (( if1_as1 + if1_as2) *(if1_rs2))");
	cSetAssignmentRule(model, "if1_rs2","1/(if1_dr1_Kd+if1_dr1_Monomer^if1_dr1_h)");
	cSetAssignmentRule(model, "if2_as1","((1+((Y/if2_ta1_Kd)^if2_ta1_h))-1)/((1+((Y/if2_ta1_Kd)^if2_ta1_h)))");
	cSetAssignmentRule(model, "if2_as2","((1+((Y/if2_ta2_Kd)^if2_ta2_h))-1)/((1+((Y/if2_ta2_Kd)^if2_ta2_h)))");
	cSetAssignmentRule(model, "if2_cod1","if2_pro1_strength * (if2_as1)");
	cSetAssignmentRule(model, "if2_cod2","if2_pro2_strength * (( if2_as1 + if2_as2) *(if2_rs2))");
	cSetAssignmentRule(model, "if2_rs2","1/(if2_dr1_Kd+if2_dr1_Monomer^if2_dr1_h)");
	r0 = cCreateReaction(model, "if1_dr1_v1");
	cSetReactionRate(r0,"if1_cod1");
	cAddProduct(r0,"if1_dr1_Monomer",1);
	r1 = cCreateReaction(model, "if1_dr1_v2");
	cSetReactionRate(r1,"if1_dr1_degradation_rate*if1_dr1_Monomer");
	cAddReactant(r1,"if1_dr1_Monomer",1);
	r2 = cCreateReaction(model, "if1_pp1_v1");
	cSetReactionRate(r2,"if1_cod2");
	cAddProduct(r2,"tf2",1);
	r3 = cCreateReaction(model, "if1_pp1_v2");
	cSetReactionRate(r3,"tf2_degradation_rate*tf2");
	cAddReactant(r3,"tf2",1);
	r4 = cCreateReaction(model, "if2_dr1_v1");
	cSetReactionRate(r4,"if2_cod1");
	cAddProduct(r4,"if2_dr1_Monomer",1);
	r5 = cCreateReaction(model, "if2_dr1_v2");
	cSetReactionRate(r5,"if2_dr1_degradation_rate*if2_dr1_Monomer");
	cAddReactant(r5,"if2_dr1_Monomer",1);
	r6 = cCreateReaction(model, "if2_pp1_v1");
	cSetReactionRate(r6,"if2_cod2");
	cAddProduct(r6,"tf4",1);
	r7 = cCreateReaction(model, "if2_pp1_v2");
	cSetReactionRate(r7,"tf4_degradation_rate*tf4");
	cAddReactant(r7,"tf4",1);
	r8 = cCreateReaction(model, "pp1_v1");
	cSetReactionRate(r8,"cod1");
	cAddProduct(r8,"GFP",1);
	r9 = cCreateReaction(model, "pp1_v2");
	cSetReactionRate(r9,"GFP_degradation_rate*GFP");
	cAddReactant(r9,"GFP",1);
	return model;
}




