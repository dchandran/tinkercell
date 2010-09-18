#include "BioCoder.h"


//paper "Transcriptional Instability is Not a Universal Attribute of Aging"-Quake et al., Department of Bioengineering, Stanford University.
void main()
{	
	start_protocol("Methods in Transcriptional instability paper");

	//declaration of reagents used in the protocol
	Fluid blood_sample = new_fluid("Blood sample");
	Fluid lysis_buffer = new_fluid("ACK lysis buffer", "150 mM NH4Cl, 10 mM KHCO3, 0.1 mM EDTA, pH 7.2");
	Fluid pbs = new_fluid("PBS", ICE_COLD);
	Fluid edta = new_fluid("EDTA");
	Fluid staining_buffer = new_fluid("staining buffer");
	Fluid mab = new_fluid("fluorescence-conjugated monoclonal antibody");
	Fluid block = new_fluid("blocking antibody");
	Fluid secondab = new_fluid("second antibody");
	Fluid cell_lysis_buffer = new_fluid("cell lysis buffer");
	Fluid rttaq = new_fluid("RT/TAQ blend");
	Fluid multiplex_primer = new_fluid("multiplex primer");
	Fluid te = new_fluid("TE buffer");

	Container rxn_tube = new_container(RXN_TUBE);

	first_step("Dilution of blood samples:");
	measure_fluid(blood_sample, vol(10, UL), rxn_tube);
	measure_fluid(pbs, vol(89, UL), rxn_tube);
	measure_fluid(edta, vol(1, UL), rxn_tube);
	tap(rxn_tube);

	//dilution of blood samples with cold PBS supplemented with 10mM EDTA 
	next_step("Lysis of red cells:");
	measure_fluid(lysis_buffer, vol(200, UL), rxn_tube);
	//lysis of red cells with ACK lysis buffer
	vortex(rxn_tube);

	//This is followed by staining of remaining cells and HSCs with fluorescence-conjugated antibodies
	next_step("Fluorescence staining:");
	measure_fluid(staining_buffer, vol(200, UL), rxn_tube);
	vortex(rxn_tube);

	//centrifugation, discarding the supernatant and resuspension of cells in staining buffer
	next_step();
	centrifuge_pellet(rxn_tube, speed(SPEED_MAX, RPM), 4, time(5, MINS));

	next_step();
	measure_fluid(staining_buffer, vol(150, UL), rxn_tube);
	vortex(rxn_tube);
	//aliquoating 100ul of the cells into FACS tube(not shown)
	//addition of 5 ul of blocking antibody

	next_step();
	measure_fluid(block, vol(5, UL), rxn_tube);
	vortex(rxn_tube);
	//incubation for 2 MINS at room temperature
	store_for(rxn_tube, RT, time(2, MINS));

	next_step();
	measure_fluid(mab, vol(10, UL), rxn_tube);
	vortex(rxn_tube);
	//addition of monoclonal antibodies and incubation for 30 MINS at 4 deg C
	store_for(rxn_tube, 4, time(30, MINS));

	next_step();
	measure_fluid(staining_buffer, vol(2, ML), rxn_tube);
	vortex(rxn_tube);

	//centrifuging at speed_max for 5 MINS at 4 deg C
	next_step();
	centrifuge_pellet(rxn_tube, speed(SPEED_MAX, RPM), 4, time(5, MINS));

	//discarding sup and resuspension of pellet in 100ul staining buffer
	next_step();
	measure_fluid(staining_buffer, vol(100, UL), rxn_tube);
	resuspend(rxn_tube);
	//addition of 20ul second antibody
	measure_fluid(secondab, vol(20, UL), rxn_tube);
	vortex(rxn_tube);
	//incubation at 4 deg fro 30 MINS
	incubate(rxn_tube, 4, time(30, MINS));

	//addition of 2 ml of staining buffer and subsequent washing
	next_step();
	measure_fluid(staining_buffer, vol(2, ML), rxn_tube);
	vortex(rxn_tube);

	//centrifuging at speed_max for 5 MINS at 4 deg C
	next_step();
	centrifuge_pellet(rxn_tube, speed(SPEED_MAX, RPM), 4, time(5, MINS));

	next_step();
	measure_fluid(staining_buffer, vol(500, UL), rxn_tube);
	resuspend(rxn_tube);
	store(rxn_tube, 4);

	next_step();
	measure_fluid(cell_lysis_buffer, vol(10, UL), rxn_tube);
	vortex(rxn_tube);

	next_step();
	use_or_store(rxn_tube, time(20, MINS), 4, -20);

	//single cell sort
	next_step("Single cell sort:");
	facs(rxn_tube);

	//reverse transcription and pre-amplification
	next_step("Reverse transcription and pre-amplification:");
	measure_fluid(multiplex_primer, vol(5, UL), rxn_tube);
	vortex(rxn_tube);

	//brief spinning-down in centrifuge
	next_step();
	centrifuge_pellet(rxn_tube, speed(1000, RPM), RT, time(5, SECS));

	//addition of RT/TAQ blend
	next_step();
	measure_fluid(rttaq, vol(5, UL), rxn_tube);
	invert(rxn_tube);

	//RT-PCR thermal profile:
	//15-minute 55 deg RT incubation
	store_for(rxn_tube, RT, time(15, MINS));

	//followed by 3-minute 95 deg hot start
	next_step();
	pcr_init_denat(rxn_tube, 95, time(3, MINS));
	//followed by 15 cycles of PCR amplification -->
	//15s, 95 deg denaturation step;60s, 60deg annealing/extension step
	thermocycler(rxn_tube, 15, 95, time(15, SECS), 60, time(60, SECS));

	//dilution of PCR products in 200ul of TE
	next_step();
	measure_fluid(te, vol(200, UL), rxn_tube);
	invert(rxn_tube);
	//storage at -20 deg for analysis
	use_or_store(rxn_tube, time(30, MINS), 4, -20);

	end_protocol();

}