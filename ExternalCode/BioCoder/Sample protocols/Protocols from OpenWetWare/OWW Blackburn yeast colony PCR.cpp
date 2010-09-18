#include "BioCoder.h"
#define X 60

void main()
{
	start_protocol("Blackburn - Yeast Colony PCR");

	Fluid naoh = new_fluid("0.02M NaOH");
	Fluid q_solution = new_fluid("Q-solution", "5X");
	Fluid pcr_buffer = new_fluid("PCR buffer", "10X");
	Fluid dntp = new_fluid("dNTPs", "10mM each");
	Fluid f_primer = new_fluid("Forward primer", "100µM");
	Fluid r_primer = new_fluid("Reverse primer");
	Fluid taq = new_fluid("Taq");
	Fluid water = new_fluid("ddH<sub>2</sub>O");
	Solid colony = new_solid("a small colony");

	Container pcr_tube1 = new_container(STERILE_PCR_TUBE);
	Container pcr_tube2 = new_container(STERILE_PCR_TUBE);
	Container pcr_tube3 = new_container(STERILE_PCR_TUBE);

	//Yeast Cell Lysis
	//   1. Aliquot 10uL of 0.02M NaOH into PCR tubes.
	first_step("Yeast Cell Lysis");
	first_sub_step();
	measure_fluid(naoh, vol(10, UL), pcr_tube1);

	//   2. Using a sterile pipette tip, pick a small colony and resuspend in NaOH.
	//          * If the solution is cloudy, you've added enough cells.
	//          * I have been told adding too much yeast can inhibit the reaction. 
	next_sub_step();
	measure_solid(colony, pcr_tube1);
	name_sample(pcr_tube1, "colony");
	resuspend(pcr_tube1);
	comment("If the solution is cloudy, you've added enough cells.");
	comment("I have been told adding too much yeast can inhibit the reaction.");

	//   3. Boil the samples on a PCR machine by incubating the tubes at 99C for 10 minutes.
	//          * In the mean time, prepare the master mix for the PCR reaction.
	//          * The boiled samples are stable at room temp for some time. Keep on ice or freeze for longer. 
	next_sub_step();
	thermocycler(pcr_tube1, 99, time(10, MINS));
	comment("In the mean time, prepare the master mix for the PCR reaction.");
	comment("The boiled samples are stable at room temp for some time. Keep on ice or freeze for longer.");

	//PCR
	//   1. Prepare the master mix solution containing:
	//          * 2uL 5X Q-solutioCn
	//          * 1uL 10X PCR Buffer
	//          * 0.2uL dNTPs (10mM each)
	//          * 0.2uL foward primer (100uM)
	//          * 0.2uL reverse primer (100uM)
	//          * 0.1uL Taq
	//          * 5.3uL ddH2O 
	next_step("PCR");
	first_sub_step();
	Fluid fluid_array[7] = {q_solution, pcr_buffer, dntp, f_primer, r_primer, taq, water};
	Volume* volumes[7] = {vol(2, UL), vol(1, UL) , vol(0.2, UL), vol(0.2, UL), vol(0.2, UL), vol(0.1, UL), vol(5.3, UL)};
	char* tube[1] = {"Colony PCR"};
	mixing_table(2, 8, fluid_array, tube, volumes, vol(9, UL), pcr_tube2);

	//   2. Aliquot 9uL of the master mix solution into fresh PCR tubes.
	next_sub_step();
	name_container(pcr_tube3, "Master Mix aliquot");
	name_sample(pcr_tube2, "master mix solution");
	measure_fluid(pcr_tube2, vol(9, UL), pcr_tube3);

	//   3. Transfer 1uL of boiled samples to the master mix aliquots (a multi-channel pipette is helpful here).
	next_sub_step();
	name_sample(pcr_tube1, "boiled sample");
	measure_fluid(pcr_tube1, vol(1, UL), pcr_tube3);
	comment("A multichannel pipette is helpful here.");

	//   4. Run the following PCR cycle:
	//         1. 5 min at 95C
	//         2. 30 cycles of:
	//               1. 10 sec at 95C
	//               2. 10 sec at 50C (or appropriate annealing temperature)
	//               3. 1 min/kbp at 72C (I generally do 30 sec) 
	//         3. 10 min at 72C (I don't think this step is critical) 
	next_sub_step();
	pcr_init_denat(pcr_tube3, 95, time(5, MINS));
	thermocycler(pcr_tube3, 30, 95, time(10, SECS), 50, time(10, SECS), 72, time(X, SECS), NORMAL); 
	comment("Elongation time : 1 min/kbp.I generally do 30 sec elongation.");
	pcr_final_ext(pcr_tube3, 72, time(10, MINS), 4);
	comment("I don't think this step is critical.");

	end_protocol();
}
