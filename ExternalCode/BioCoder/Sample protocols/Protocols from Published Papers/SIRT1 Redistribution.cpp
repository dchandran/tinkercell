#include "BioCoder.h"


//from the paper "SIRT1 Redistribution on Chromatin Promotes Genomic Stability but Alters Gene Expression during Aging"
//-Sinclair et al., DOI 10.1016/j.cell.2008.10.025
Fluid transfect_dna, nam, pbs, chloroform, isopropanol, ethanol, depc_water, rt_primer, rttaq, rtpcr_buffer, te ;

void main()
{
	start_protocol("Methods in SIRT1 Redistribution");

	//declaration of reagents needed for the protocol
	Fluid yeast_cul = new_fluid("yeast culture", "2% glucose");
	Fluid ypd = new_fluid("yeast peptone dextrose medium");
	Fluid hy_per= new_fluid("hyrogen peroxide");
	Fluid gelatin = new_fluid("Gelatin");
	Fluid wortmannin = new_fluid("wortmannin");
	Fluid sirtinol = new_fluid("sirtinol");
	Fluid formaldehyde = new_fluid("formaldehyde");
	Fluid glycine = new_fluid("glycine");
	Fluid trypsin = new_fluid("trypsin");
	Fluid rab_ab = new_fluid("rabbit antibody");
	Fluid trizol = new_fluid("trizol");
	Fluid dnase = new_fluid("DNase");
	Fluid es_med = new_fluid("DMEM without LIF");
	Fluid transfect_med = new_fluid("transfection agent");
	Fluid transfect_dna = new_fluid("DNA to be transfected", "pCBASce/pCAGGS-Dsred");
	Fluid nam = new_fluid("NAM");
	Fluid pbs = new_fluid("PBS", ICE_COLD);
	Fluid chloroform = new_fluid("chloroform");
	Fluid isopropanol = new_fluid("isopropanol");
	Fluid ethanol = new_fluid("75% ethanol");
	Fluid depc_water = new_fluid("DEPC treated water");
	Fluid rt_primer = new_fluid("random hexamers and oligo dT primers");
	Fluid rtpcr_buffer = new_fluid("RT-PCR buffer");
	Fluid rttaq = new_fluid("RT/TAQ blend");
	Fluid te = new_fluid("TE buffer");

	Solid mouse_es = new_solid("mouse embryonic stem cells");

	Container sterile_microfuge_tube = new_container(STERILE_MICROFUGE_TUBE);
	Container sterile_microfuge_tube1 = new_container(STERILE_MICROFUGE_TUBE);
	Container sterile_microfuge_tube2 = new_container(STERILE_MICROFUGE_TUBE);
	Container cell_culture_chamber = new_container(CELL_CULT_CHAMBER, mouse_es);

	//YEAST EXPERIMENTS
	first_step("Yeast Experiments");
	measure_fluid(yeast_cul, vol(1.5, ML), sterile_microfuge_tube);

	//pelleting down of yeast cells
	next_step();
	centrifuge_pellet(sterile_microfuge_tube, speed(SPEED_MAX, RPM), RT, time(2, MINS));

	//exposure to H2O2 for 30 MINS
	next_step();
	measure_fluid(hy_per, vol(200, UL), sterile_microfuge_tube);
	resuspend(sterile_microfuge_tube);

	next_step();
	store_for(sterile_microfuge_tube, RT, time(30 ,MINS));

	//recovery of yeast cells for 4 hours in YPD medium
	next_step();
	centrifuge_pellet(sterile_microfuge_tube, speed(SPEED_MAX, RPM), RT, time(2, MINS));

	next_step();
	measure_fluid(ypd, vol(1000, UL), sterile_microfuge_tube);
	resuspend(sterile_microfuge_tube);
	store_for(sterile_microfuge_tube, RT, time(4, HRS));
	//analysis of cells by FACS
	facs(sterile_microfuge_tube);

	//CELL CULTURE TREATMENTS
	//culture of mouse embryonic stem cells
	next_step("Cell culture treatments");
	cell_culture(cell_culture_chamber, es_med, SPEED_MAX, RT, 3*24*60*60, 15, trypsin, gelatin, trypsin, 2*60*60);
	//incubation with NAM 2 hours prior to transfection
	measure_fluid(nam, vol(15, ML), cell_culture_chamber);
	store_for(cell_culture_chamber, RT, time(2, HRS));


	//transfection of cultured cells with pCBASce/pCAGGS-Dsred in the presence of Fugene 6 transfection agent
	next_step();
	transfection(cell_culture_chamber, transfect_med, transfect_dna);

	//FACS of transfected cells after 48 HRS

	next_step("FACS");
	incubate(cell_culture_chamber, RT, time(48, HRS));
	facs(cell_culture_chamber);

	//CHROMATIN IMMUNOPRECIPITATION
	next_step("Chromatin immunoprecipitation");
	measure_fluid(formaldehyde, vol(15, ML), cell_culture_chamber);

	//cross-linking of cultured cells with 1% fromaldehyde and then quenching with glycine
	next_step("Cross-linking of cultured cells");
	store_for(cell_culture_chamber, 37, time(15, MINS));
	measure_fluid(glycine, vol(15, ML), cell_culture_chamber);


	// this is followed by sonication and immunoprecipitation(not shown)
	next_step("Sonication and Immunoprecipitation (not shown)");


	//RNA ISOLATION, REVERSE TRANSCRIPTION AND PCR ANALYSIS
	next_step("RNA isolation, reverse transcription and PCR analysis");
	//homogenization of cells with trizol
	measure_fluid(pbs, vol(15, ML), cell_culture_chamber);
	measure_fluid(trizol, vol(15, ML), cell_culture_chamber);
	vortex(cell_culture_chamber);

	next_step("Phase separation");
	//phase separation with chloroform
	measure_fluid(chloroform, vol(0.2*15, ML), cell_culture_chamber);
	store_for(cell_culture_chamber, RT, time(3, MINS));
	transfer(cell_culture_chamber, sterile_microfuge_tube1);
	centrifuge_phases_top(sterile_microfuge_tube1, speed(SPEED_MAX, RPM), 4, time(15, MINS),sterile_microfuge_tube2);
	//aqueous phase contains the RNA

	next_step("RNA precipitation");
	//precipitation of RNA with isopropanol
	measure_fluid(isopropanol, vol(1, ML), sterile_microfuge_tube2);
	invert(sterile_microfuge_tube2);

	next_step();
	store_for(sterile_microfuge_tube2, RT, time(10, MINS));

	next_step();
	centrifuge_pellet(sterile_microfuge_tube2, speed(SPEED_MAX, RPM), 4, time(10, MINS));

	//RNA wash with 75% ethanol
	next_step();
	measure_fluid(ethanol, vol(1, ML), sterile_microfuge_tube2);
	resuspend(sterile_microfuge_tube2);

	next_step();
	centrifuge_pellet(sterile_microfuge_tube2, speed(SPEED_MAX, RPM), 4, time(10, MINS));

	//air drying the RNA pellet for 5-10 MINS
	next_step();
	dry_pellet(sterile_microfuge_tube2, IN_AIR, time(5, MINS));

	next_step();
	measure_fluid(depc_water, vol(20, UL), sterile_microfuge_tube2);
	dissolve(sterile_microfuge_tube2);
	use_or_store(sterile_microfuge_tube2, time(20, MINS), 4, -20);

	//reverse transcription of RNA
	next_step();
	measure_fluid(rt_primer, vol(5, UL), sterile_microfuge_tube2);
	vortex(sterile_microfuge_tube2);

	//addition of RT/TAQ blend
	next_step();
	measure_fluid(rttaq, vol(5, UL), sterile_microfuge_tube2);
	tap(sterile_microfuge_tube2);

	//RT-PCR thermal profile:
	//15-minute 55 deg RT incubation
	next_step("RT incubation");
	store_for(sterile_microfuge_tube2, 55, time(15, MINS));

	//followed by 3-minute 95 deg hot start
	next_step("PCR");
	pcr_init_denat(sterile_microfuge_tube2, 95, time(3, MINS));
	//followed by 15 cycles of PCR amplification -->
	//15s, 95 deg denaturation step;60s, 60deg annealing/extension step
	thermocycler(sterile_microfuge_tube2, 15, 95, time(15, SECS), 60, time(60, SECS));

	//dilution of PCR products in 200ul of TE
	next_step();
	measure_fluid(te, vol(200, UL), sterile_microfuge_tube2);
	tap(sterile_microfuge_tube2);
	//storage at -20 deg for analysis
	use_or_store(sterile_microfuge_tube2, time(30, MINS), 4, -20);

	end_protocol();
}