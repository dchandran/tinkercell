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
	cCreateSpecies(DefaultCompartment,"A",5);
	cCreateSpecies(DefaultCompartment,"B",8);
	cCreateSpecies(DefaultCompartment,"C",10);
	cSetGlobalParameter(model,"J1_k0",0.2);
	cSetGlobalParameter(model,"J2_k0",0.1);
	cSetGlobalParameter(model,"J3_k0",0.5);
	r0 = cCreateReaction(model, "J1");
	cSetReactionRate(r0,"J1_k0*A*B");
	cAddReactant(r0,"A",1);
	cAddProduct(r0,"B",1);
	r1 = cCreateReaction(model, "J2");
	cSetReactionRate(r1,"J2_k0*C*B");
	cAddReactant(r1,"B",1);
	cAddProduct(r1,"C",1);
	r2 = cCreateReaction(model, "J3");
	cSetReactionRate(r2,"J3_k0*A*C");
	cAddProduct(r2,"A",1);
	cAddReactant(r2,"C",1);
	cCreateEvent(model,"event0", "time>5","J1_k0", "0");
	return model;
}




