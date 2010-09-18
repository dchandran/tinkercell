#include "BioCoder.h"


void main()
{
	//This protocol has been adapted from Applied Biosystems(TM) Automated DNA Sequencing Guide protocol using BigDye Primer.
	start_protocol("DNA Sequencing Protocol");

	//delcaration of reagents/solution used in DNA sequecing
	Fluid dna=new_fluid("DNA template");
	Fluid eth70=new_fluid("70% ethanol");
	Fluid eth95=new_fluid("95% ethanol");
	Fluid sdw=new_fluid("sterile distilled water");
	Fluid tsr_fluid=new_fluid("template suppression reagent");
	Fluid rxn_mix=new_fluid("Ready Reaction mix");
	Fluid sep_buffer = new_fluid("separation medium");

	//declaration of containers required
	Container rxn_tube=new_container(RXN_TUBE);

	//preparing the cycle sequencing reactions
	first_step("Preparing the DNA Sequencing Reactions");
	measure_fluid(rxn_mix, vol(4, UL), rxn_tube);
	measure_fluid(dna, vol(1, UL), rxn_tube);
	tap(rxn_tube);
	comment("NOTE:4 different reactions will have to be set up for the four different bases-A, G, T and C.");


	//transfer of sample to the thermocycler 
	//For the first 15 steps, 3 cyclic steps of
	//96degC for 10 sec,
	//55degC for 5 sec and
	//70degC for 1 min
	next_step();
	thermocycler(rxn_tube, 15, 96, time(10, SECS), 55, time(5, SECS), 70, time(1, MINS), NORMAL);

	//For the next 15 steps, 2 cyclic steps of
	//96 degC for 10 sec
	//70degC for 1 min
	next_step();
	thermocycler(rxn_tube, 15, 96, time(10, SECS), 70, time(1, MINS));

	//set the temperature of product obtained to 4deg C until further use
	next_step();
	name_sample(rxn_tube, "product obtained");
	use_or_store(rxn_tube, time(30, MINS), 4, -20);

	//95% ethanol precipitation of samples for purification
	next_step();
	measure_fluid(sdw, vol(16, UL), rxn_tube);
	measure_fluid(eth95, vol(2, ML), rxn_tube);
	tap(rxn_tube);
	store_for(rxn_tube, RT, time(15, MINS));

	//centrifuge the sample for 20 mins at SPEED_MAX and room temperature to obtain the pellet
	next_step();
	centrifuge_pellet(rxn_tube, speed(SPEED_MAX, RPM), RT, time(20, MINS));

	//95% ethanol precipitation of samples for purification
	next_step();
	measure_fluid(eth70, vol(1, ML), rxn_tube);
	tap(rxn_tube, PELLET_DISLODGES);

	//centrifuge the sample for 10 mins at SPEED_MAX and room temperature to obtain the pellet
	next_step();
	centrifuge_pellet(rxn_tube, speed(SPEED_MAX, RPM), RT, time(10, MINS));

	//dry the pellet
	next_step();
	invert_dry(rxn_tube, RT);

	//preparing dye primer reaction products for electrophoresis(combining the four products)
	next_step("Sequencing and detection");
	//ethanol precipitation
	measure_fluid(eth95, vol(1, ML), rxn_tube);
	comment("NOTE: combine all the 4 products (A, G, T and C) from the preparation step.");
	dissolve(rxn_tube);
	store_for(rxn_tube, RT, time(10, MINS));

	//centrifugation at SPEED_MAX for 15 mins at room temperature to obtain the pellet
	next_step();
	centrifuge_pellet(rxn_tube, speed(SPEED_MAX, RPM), RT, time(15, MINS));

	//dry pellet
	next_step();
	invert_dry(rxn_tube, RT);

	//preparing and loading samples for capillary electrophoresis
	//addition of template suppression reagent and heating for 2 minutes at 95deg C
	next_step();
	measure_fluid(tsr_fluid, vol(12.25, UL), rxn_tube);
	dissolve(rxn_tube);
	incubate(rxn_tube, 95, time(2, MINS));

	//set the temperature of sample to 4 degC
	next_step();
	name_sample(rxn_tube, "solution");
	set_temp(rxn_tube, 4);
	//vortexing the sample and spinning briefly
	vortex(rxn_tube);

	//cooling the sample to 4 degC
	next_step();
	use_or_store(rxn_tube, time(30, MINS), 4, -20);

	//CE of sample
	next_step();
	ce_detect(rxn_tube, 5, 236, sep_buffer, time(3, MINS));

	end_protocol();
}

