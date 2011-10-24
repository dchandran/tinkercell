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
	cCreateSpecies(DefaultCompartment,"sm1",0);
	cCreateSpecies(DefaultCompartment,"sm2",0);
	cCreateSpecies(DefaultCompartment,"sm3",0);
	cSetGlobalParameter(model,"br1_k0",0.1);
	cSetGlobalParameter(model,"br2_k0",0.1);
	cSetGlobalParameter(model,"deg1_k0",0.1);
	r0 = cCreateReaction(model, "br1");
	cSetReactionRate(r0,"br1_k0*sm1");
	cAddReactant(r0,"sm1",1);
	cAddProduct(r0,"sm2",1);
	r1 = cCreateReaction(model, "br2");
	cSetReactionRate(r1,"br2_k0*sm2");
	cAddReactant(r1,"sm2",1);
	cAddProduct(r1,"sm3",1);
	r2 = cCreateReaction(model, "deg1");
	cSetReactionRate(r2,"deg1_k0*sm3");
	cAddReactant(r2,"sm3",1);
	r3 = cCreateReaction(model, "pro1");
	cSetReactionRate(r3,"10");
	cAddProduct(r3,"sm1",1);
	cCreateEvent(model,"event0", "time > 6","sm1","0");
	return model;
}




