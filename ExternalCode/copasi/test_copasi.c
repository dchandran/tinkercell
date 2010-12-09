#include <stdlib.h>
#include <stdio.h>
#include "copasi_api.h"

int main()
{
	int i, j;
	FILE * outfile;
	char * A_str, * B_str, * C_str, *c;
	int s1, s2, s3;

	copasi_init();
	
	//model
	copasi_model model = createCopasiModel("M");
	
	//species
	copasi_compartment cell = createCompartment(model, "cell", 1.0);
	copasi_species A = createSpecies(cell, "A", 2);
	copasi_species B = createSpecies(cell, "B", 1);
	copasi_species C = createSpecies(cell, "C", 3);
	
	//parameters
	setGlobalParameter(model, "k1", 0.1);   //k1
	setGlobalParameter(model, "k2", 0.2);   //k2
	setGlobalParameter(model, "k3", 0.3);   //k3
	
	//reactions
	copasi_reaction R1 = createReaction(model, "R1");  // A+B -> 2B
	addReactant(R1, A, 1.0);
	addReactant(R1, B, 1.0);
	addProduct(R1, B, 2.0);
	setReactionRate(R1, "k1*A*B");

	copasi_reaction R2 = createReaction(model, "R2");  //B+C -> 2C
	addReactant(R2, B, 1.0);
	addReactant(R2, C, 1.0);
	addProduct(R2, C, 2.0);
	setReactionRate(R2, "k2*B*C");

	copasi_reaction R3 = createReaction(model, "R3"); //C+A -> 2A
	addReactant(R3, C, 1.0);
	addReactant(R3, A, 1.0);
	addProduct(R3, A, 2.0);
	setReactionRate(R3, "k3*C*A");

	/*get the full-names for each species
	A_str = getCopasiSpeciesID(A, (&s1));
	B_str = getCopasiSpeciesID(B, (&s2));
	C_str = getCopasiSpeciesID(C, (&s3));
	
	/*create variable prod=A+B+C, but replace A,B,C with full-names
	c = (char*)malloc((s1+s2+s3+10) * sizeof(char));
	sprintf(c, "<%s>*<%s>*<%s>\0", A_str, B_str, C_str);
	
	createVariable(model, "prod",c);  //assignment rule "prod = A+B+C"
	*/
	createVariable(model, "prod","A+B+C");
	
	free(c);
	free(A_str);
	free(B_str);
	free(C_str);
	
	compileCopasiModel(model);
	//run
	tc_matrix output = simulateDeterministic(model, 0, 30, 1000);  //model, start, end, num. points
	
	//output
	tc_printMatrix("output.tab", output);
	
	printf("\noutput.tab contains the final output\n\n");

	copasi_end();
}

