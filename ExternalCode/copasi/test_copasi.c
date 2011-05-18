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
	
	//m1 = model3();
    m1 = cReadSBMLFile("model1.sbml");
    
	output = cSimulateDeterministic(m1, 0, 10, 200);  //model, start, end, num. points
	tc_printMatrixToFile("output.tab", output);	
	tc_deleteMatrix(output);
	printf("%s\n",m1.errorMessage);
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
	
	//species
	copasi_compartment cell = cCreateCompartment(model, "cell", 1.0);
	cCreateSpecies(cell, "A", 2);
	cCreateSpecies(cell, "B", 1);
	cCreateSpecies(cell, "C", 3);
	
	//parameters	
	cSetValue(model, "k1", 0.1);   //k1
	cSetValue(model, "k2", 0.2);   //k2
	cSetValue(model, "k3", 0.3);   //k3
	
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
	copasi_reaction r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17,r18,r19,r20,r21,r22,r23,r24,r25,r26,r27,r28,r29,r30,r31,r32,r33,r34,r35,r36,r37,r38,r39;
DefaultCompartment = cCreateCompartment(model,"DefaultCompartment",1);
cCreateSpecies(DefaultCompartment,"m10",2.81634);
cCreateSpecies(DefaultCompartment,"m1",7.48329);
cCreateSpecies(DefaultCompartment,"m2",6.48978);
cCreateSpecies(DefaultCompartment,"m3",3.36843);
cCreateSpecies(DefaultCompartment,"m4",1.97428);
cCreateSpecies(DefaultCompartment,"m5",11.7727);
cCreateSpecies(DefaultCompartment,"m6",15.4771);
cCreateSpecies(DefaultCompartment,"m7",16.2133);
cCreateSpecies(DefaultCompartment,"m8",5.54916);
cCreateSpecies(DefaultCompartment,"m9",5.80386);
cCreateSpecies(DefaultCompartment,"p10",41.7024);
cCreateSpecies(DefaultCompartment,"p1",54.9835);
cCreateSpecies(DefaultCompartment,"p2",67.6917);
cCreateSpecies(DefaultCompartment,"p3",20.4195);
cCreateSpecies(DefaultCompartment,"p4",44.8398);
cCreateSpecies(DefaultCompartment,"p5",51.0505);
cCreateSpecies(DefaultCompartment,"p6",91.8392);
cCreateSpecies(DefaultCompartment,"p7",95.2534);
cCreateSpecies(DefaultCompartment,"p8",17.3931);
cCreateSpecies(DefaultCompartment,"p9",13.6023);
cSetGlobalParameter(model,"vmax_m1",18.4903);
cSetGlobalParameter(model,"alpha_m1",1);
cSetGlobalParameter(model,"alpha_m1p10",0.410013);
cSetGlobalParameter(model,"K_m1p10",5.97609);
cSetGlobalParameter(model,"n_m1p10",2.71985);
cSetGlobalParameter(model,"alpha_m1p5",0.398096);
cSetGlobalParameter(model,"K_m1p5",7.18846);
cSetGlobalParameter(model,"n_m1p5",3.63584);
cSetGlobalParameter(model,"rho_complex_m1",1);
cSetGlobalParameter(model,"decayconst_m1",0.45081);
cSetGlobalParameter(model,"vmax_m2",11.2904);
cSetGlobalParameter(model,"alpha_m2",0.0371501);
cSetGlobalParameter(model,"alpha_m2p7",1);
cSetGlobalParameter(model,"K_m2p7",15.6074);
cSetGlobalParameter(model,"n_m2p7",2.44743);
cSetGlobalParameter(model,"decayconst_m2",1.87103);
cSetGlobalParameter(model,"vmax_m3",15.4085);
cSetGlobalParameter(model,"alpha_m3",0.445049);
cSetGlobalParameter(model,"alpha_m3p8",1);
cSetGlobalParameter(model,"K_m3p8",5.58721);
cSetGlobalParameter(model,"n_m3p8",3.26311);
cSetGlobalParameter(model,"alpha_m3p9",0.191068);
cSetGlobalParameter(model,"K_m3p9",10.9831);
cSetGlobalParameter(model,"n_m3p9",2.05307);
cSetGlobalParameter(model,"rho_complex_m3",1);
cSetGlobalParameter(model,"decayconst_m3",2.13039);
cSetGlobalParameter(model,"vmax_m4",18.2999);
cSetGlobalParameter(model,"alpha_m4",1);
cSetGlobalParameter(model,"alpha_m4p7",0.465478);
cSetGlobalParameter(model,"K_m4p7",8.94677);
cSetGlobalParameter(model,"n_m4p7",3.75974);
cSetGlobalParameter(model,"alpha_m4p10",0.396798);
cSetGlobalParameter(model,"K_m4p10",13.7289);
cSetGlobalParameter(model,"n_m4p10",2.68788);
cSetGlobalParameter(model,"rho_complex_m4",1);
cSetGlobalParameter(model,"decayconst_m4",1.77792);
cSetGlobalParameter(model,"vmax_m5",14.748);
cSetGlobalParameter(model,"alpha_m5",0.151304);
cSetGlobalParameter(model,"alpha_m5p10",1);
cSetGlobalParameter(model,"K_m5p10",13.7457);
cSetGlobalParameter(model,"n_m5p10",3.28897);
cSetGlobalParameter(model,"decayconst_m5",1.16391);
cSetGlobalParameter(model,"vmax_m6",12.3838);
cSetGlobalParameter(model,"alpha_m6",0.102833);
cSetGlobalParameter(model,"alpha_m6p9",1);
cSetGlobalParameter(model,"K_m6p9",5.68656);
cSetGlobalParameter(model,"n_m6p9",3.12116);
cSetGlobalParameter(model,"decayconst_m6",0.2996);
cSetGlobalParameter(model,"vmax_m7",7.02574);
cSetGlobalParameter(model,"alpha_m7",0.542631);
cSetGlobalParameter(model,"alpha_m7p10",1);
cSetGlobalParameter(model,"K_m7p10",9.23352);
cSetGlobalParameter(model,"n_m7p10",2.21934);
cSetGlobalParameter(model,"alpha_m7p9",0.099976);
cSetGlobalParameter(model,"K_m7p9",11.8863);
cSetGlobalParameter(model,"n_m7p9",3.66599);
cSetGlobalParameter(model,"rho_complex_m7",1);
cSetGlobalParameter(model,"decayconst_m7",1.6004);
cSetGlobalParameter(model,"vmax_m8",14.3471);
cSetGlobalParameter(model,"alpha_m8",0.567977);
cSetGlobalParameter(model,"alpha_m8p5",1);
cSetGlobalParameter(model,"K_m8p5",15.2628);
cSetGlobalParameter(model,"n_m8p5",3.39226);
cSetGlobalParameter(model,"alpha_m8p10",0.0117862);
cSetGlobalParameter(model,"K_m8p10",6.14516);
cSetGlobalParameter(model,"n_m8p10",2.24997);
cSetGlobalParameter(model,"rho_complex_m8",1);
cSetGlobalParameter(model,"decayconst_m8",0.57728);
cSetGlobalParameter(model,"vmax_m9",13.3206);
cSetGlobalParameter(model,"alpha_m9",0.0679016);
cSetGlobalParameter(model,"alpha_m9p5",1);
cSetGlobalParameter(model,"K_m9p5",15.1405);
cSetGlobalParameter(model,"n_m9p5",2.55734);
cSetGlobalParameter(model,"decayconst_m9",2.36358);
cSetGlobalParameter(model,"vmax_m10",9.18929);
cSetGlobalParameter(model,"alpha_m10",0.164146);
cSetGlobalParameter(model,"alpha_m10p8",0.506507);
cSetGlobalParameter(model,"K_m10p8",9.20669);
cSetGlobalParameter(model,"n_m10p8",2.23557);
cSetGlobalParameter(model,"alpha_m10p7",0.556343);
cSetGlobalParameter(model,"K_m10p7",13.561);
cSetGlobalParameter(model,"n_m10p7",2.1898);
cSetGlobalParameter(model,"alpha_complex_m10",1);
cSetGlobalParameter(model,"rho_complex_m10",1);
cSetGlobalParameter(model,"decayconst_m10",0.569026);
cSetGlobalParameter(model,"synthconst_p1",1.23342);
cSetGlobalParameter(model,"decayconst_p1",0.986429);
cSetGlobalParameter(model,"synthconst_p2",1.04139);
cSetGlobalParameter(model,"decayconst_p2",0.681243);
cSetGlobalParameter(model,"synthconst_p3",1.04527);
cSetGlobalParameter(model,"decayconst_p3",1.93728);
cSetGlobalParameter(model,"synthconst_p4",2.35914);
cSetGlobalParameter(model,"decayconst_p4",0.375074);
cSetGlobalParameter(model,"synthconst_p5",0.496429);
cSetGlobalParameter(model,"decayconst_p5",1.78578);
cSetGlobalParameter(model,"synthconst_p6",1.64585);
cSetGlobalParameter(model,"decayconst_p6",1.57076);
cSetGlobalParameter(model,"synthconst_p7",0.651046);
cSetGlobalParameter(model,"decayconst_p7",1.81866);
cSetGlobalParameter(model,"synthconst_p8",1.24684);
cSetGlobalParameter(model,"decayconst_p8",0.542748);
cSetGlobalParameter(model,"synthconst_p9",2.05892);
cSetGlobalParameter(model,"decayconst_p9",0.404059);
cSetGlobalParameter(model,"synthconst_p10",1.05131);
cSetGlobalParameter(model,"decayconst_p10",1.04166);
r0 = cCreateReaction(model, "decay_m10_v");
cSetReactionRate(r0,"decayconst_m10*m10");
cAddReactant(r0,"m10",1);
r1 = cCreateReaction(model, "decay_m1_v");
cSetReactionRate(r1,"decayconst_m1*m1");
cAddReactant(r1,"m1",1);
r2 = cCreateReaction(model, "decay_m2_v");
cSetReactionRate(r2,"decayconst_m2*m2");
cAddReactant(r2,"m2",1);
r3 = cCreateReaction(model, "decay_m3_v");
cSetReactionRate(r3,"decayconst_m3*m3");
cAddReactant(r3,"m3",1);
r4 = cCreateReaction(model, "decay_m4_v");
cSetReactionRate(r4,"decayconst_m4*m4");
cAddReactant(r4,"m4",1);
r5 = cCreateReaction(model, "decay_m5_v");
cSetReactionRate(r5,"decayconst_m5*m5");
cAddReactant(r5,"m5",1);
r6 = cCreateReaction(model, "decay_m6_v");
cSetReactionRate(r6,"decayconst_m6*m6");
cAddReactant(r6,"m6",1);
r7 = cCreateReaction(model, "decay_m7_v");
cSetReactionRate(r7,"decayconst_m7*m7");
cAddReactant(r7,"m7",1);
r8 = cCreateReaction(model, "decay_m8_v");
cSetReactionRate(r8,"decayconst_m8*m8");
cAddReactant(r8,"m8",1);
r9 = cCreateReaction(model, "decay_m9_v");
cSetReactionRate(r9,"decayconst_m9*m9");
cAddReactant(r9,"m9",1);
r10 = cCreateReaction(model, "decay_p10_v");
cSetReactionRate(r10,"decayconst_p10*p10");
cAddReactant(r10,"p10",1);
r11 = cCreateReaction(model, "decay_p1_v");
cSetReactionRate(r11,"decayconst_p1*p1");
cAddReactant(r11,"p1",1);
r12 = cCreateReaction(model, "decay_p2_v");
cSetReactionRate(r12,"decayconst_p2*p2");
cAddReactant(r12,"p2",1);
r13 = cCreateReaction(model, "decay_p3_v");
cSetReactionRate(r13,"decayconst_p3*p3");
cAddReactant(r13,"p3",1);
r14 = cCreateReaction(model, "decay_p4_v");
cSetReactionRate(r14,"decayconst_p4*p4");
cAddReactant(r14,"p4",1);
r15 = cCreateReaction(model, "decay_p5_v");
cSetReactionRate(r15,"decayconst_p5*p5");
cAddReactant(r15,"p5",1);
r16 = cCreateReaction(model, "decay_p6_v");
cSetReactionRate(r16,"decayconst_p6*p6");
cAddReactant(r16,"p6",1);
r17 = cCreateReaction(model, "decay_p7_v");
cSetReactionRate(r17,"decayconst_p7*p7");
cAddReactant(r17,"p7",1);
r18 = cCreateReaction(model, "decay_p8_v");
cSetReactionRate(r18,"decayconst_p8*p8");
cAddReactant(r18,"p8",1);
r19 = cCreateReaction(model, "decay_p9_v");
cSetReactionRate(r19,"decayconst_p9*p9");
cAddReactant(r19,"p9",1);
r20 = cCreateReaction(model, "synth_m10_v");
cSetReactionRate(r20,"vmax_m10*(alpha_m10+alpha_m10p8*pow(p8/K_m10p8,n_m10p8)+alpha_m10p7*pow(p7/K_m10p7,n_m10p7)+alpha_complex_m10*rho_complex_m10*pow(p8/K_m10p8,n_m10p8)*pow(p7/K_m10p7,n_m10p7))/(1+pow(p8/K_m10p8,n_m10p8)+pow(p7/K_m10p7,n_m10p7)+rho_complex_m10*pow(p8/K_m10p8,n_m10p8)*pow(p7/K_m10p7,n_m10p7))");
cAddProduct(r20,"m10",1);
r21 = cCreateReaction(model, "synth_m1_v");
cSetReactionRate(r21,"vmax_m1*(alpha_m1+alpha_m1p10*pow(p10/K_m1p10,n_m1p10)+alpha_m1p5*pow(p5/K_m1p5,n_m1p5))/(1+pow(p10/K_m1p10,n_m1p10)+pow(p5/K_m1p5,n_m1p5)+rho_complex_m1*pow(p10/K_m1p10,n_m1p10)*pow(p5/K_m1p5,n_m1p5))");
cAddProduct(r21,"m1",1);
r22 = cCreateReaction(model, "synth_m2_v");
cSetReactionRate(r22,"vmax_m2*(alpha_m2+alpha_m2p7*pow(p7/K_m2p7,n_m2p7))/(1+pow(p7/K_m2p7,n_m2p7))");
cAddProduct(r22,"m2",1);
r23 = cCreateReaction(model, "synth_m3_v");
cSetReactionRate(r23,"vmax_m3*(alpha_m3+alpha_m3p8*pow(p8/K_m3p8,n_m3p8)+alpha_m3p9*pow(p9/K_m3p9,n_m3p9))/(1+pow(p8/K_m3p8,n_m3p8)+pow(p9/K_m3p9,n_m3p9)+rho_complex_m3*pow(p8/K_m3p8,n_m3p8)*pow(p9/K_m3p9,n_m3p9))");
cAddProduct(r23,"m3",1);
r24 = cCreateReaction(model, "synth_m4_v");
cSetReactionRate(r24,"vmax_m4*(alpha_m4+alpha_m4p7*pow(p7/K_m4p7,n_m4p7)+alpha_m4p10*pow(p10/K_m4p10,n_m4p10))/(1+pow(p7/K_m4p7,n_m4p7)+pow(p10/K_m4p10,n_m4p10)+rho_complex_m4*pow(p7/K_m4p7,n_m4p7)*pow(p10/K_m4p10,n_m4p10))");
cAddProduct(r24,"m4",1);
r25 = cCreateReaction(model, "synth_m5_v");
cSetReactionRate(r25,"vmax_m5*(alpha_m5+alpha_m5p10*pow(p10/K_m5p10,n_m5p10))/(1+pow(p10/K_m5p10,n_m5p10))");
cAddProduct(r25,"m5",1);
r26 = cCreateReaction(model, "synth_m6_v");
cSetReactionRate(r26,"vmax_m6*(alpha_m6+alpha_m6p9*pow(p9/K_m6p9,n_m6p9))/(1+pow(p9/K_m6p9,n_m6p9))");
cAddProduct(r26,"m6",1);
r27 = cCreateReaction(model, "synth_m7_v");
cSetReactionRate(r27,"vmax_m7*(alpha_m7+alpha_m7p10*pow(p10/K_m7p10,n_m7p10)+alpha_m7p9*pow(p9/K_m7p9,n_m7p9))/(1+pow(p10/K_m7p10,n_m7p10)+pow(p9/K_m7p9,n_m7p9)+rho_complex_m7*pow(p10/K_m7p10,n_m7p10)*pow(p9/K_m7p9,n_m7p9))");
cAddProduct(r27,"m7",1);
r28 = cCreateReaction(model, "synth_m8_v");
cSetReactionRate(r28,"vmax_m8*(alpha_m8+alpha_m8p5*pow(p5/K_m8p5,n_m8p5)+alpha_m8p10*pow(p10/K_m8p10,n_m8p10))/(1+pow(p5/K_m8p5,n_m8p5)+pow(p10/K_m8p10,n_m8p10)+rho_complex_m8*pow(p5/K_m8p5,n_m8p5)*pow(p10/K_m8p10,n_m8p10))");
cAddProduct(r28,"m8",1);
r29 = cCreateReaction(model, "synth_m9_v");
cSetReactionRate(r29,"vmax_m9*(alpha_m9+alpha_m9p5*pow(p5/K_m9p5,n_m9p5))/(1+pow(p5/K_m9p5,n_m9p5))");
cAddProduct(r29,"m9",1);
r30 = cCreateReaction(model, "synth_p10_v");
cSetReactionRate(r30,"synthconst_p10*m10");
cAddProduct(r30,"p10",1);
r31 = cCreateReaction(model, "synth_p1_v");
cSetReactionRate(r31,"synthconst_p1*m1");
cAddProduct(r31,"p1",1);
r32 = cCreateReaction(model, "synth_p2_v");
cSetReactionRate(r32,"synthconst_p2*m2");
cAddProduct(r32,"p2",1);
r33 = cCreateReaction(model, "synth_p3_v");
cSetReactionRate(r33,"synthconst_p3*m3");
cAddProduct(r33,"p3",1);
r34 = cCreateReaction(model, "synth_p4_v");
cSetReactionRate(r34,"synthconst_p4*m4");
cAddProduct(r34,"p4",1);
r35 = cCreateReaction(model, "synth_p5_v");
cSetReactionRate(r35,"synthconst_p5*m5");
cAddProduct(r35,"p5",1);
r36 = cCreateReaction(model, "synth_p6_v");
cSetReactionRate(r36,"synthconst_p6*m6");
cAddProduct(r36,"p6",1);
r37 = cCreateReaction(model, "synth_p7_v");
cSetReactionRate(r37,"synthconst_p7*m7");
cAddProduct(r37,"p7",1);
r38 = cCreateReaction(model, "synth_p8_v");
cSetReactionRate(r38,"synthconst_p8*m8");
cAddProduct(r38,"p8",1);
r39 = cCreateReaction(model, "synth_p9_v");
cSetReactionRate(r39,"synthconst_p9*m9");
cAddProduct(r39,"p9",1);
	return model;
}




