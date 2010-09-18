#include "BioCoder.h"

void main()
{
	start_protocol("Splinkerette PCR - Methods");

	Fluid dna = new_fluid("Genomic DNA");
	Fluid sau3a1 = new_fluid("Sau3A1 enzyme");
	Fluid water = new_fluid("Double-distilled water");
	Fluid neb = new_fluid("NEB buffer 1");
	Fluid bsa = new_fluid("Acetylated BSA");
	Fluid adaptor_mix = new_fluid("adaptor mix");
	Fluid ligase = new_fluid("T4 DNA-ligase");
	Fluid ligase_buff = new_fluid("T4 DNA-ligase buffer");
	Fluid neb3 = new_fluid("NEB buffer 3");
	Fluid ecorv = new_fluid("EcoRV");
	Fluid primer1 = new_fluid("Splink1 primer");
	Fluid primer2 = new_fluid("U3LTR#5 primer");
	Fluid dntp = new_fluid("dNTPs");
	Fluid pfu = new_fluid("<i>Pfu</i> Turbo Hotstart DNA polymerase");
	Fluid pfu_buff = new_fluid("<i>Pfu</i> buffer");
	Fluid mastermix = new_fluid("Qiagen multiplex mastermix");

	Tissue mouse_tumor = new_solid("mouse tumor tissue");

	Container cryo_vial = new_container(CRYO_VIAL);
	Container sterile_microfuge_tube = new_container(STERILE_MICROFUGE_TUBE);
	Container sterile_microfuge_tube1 = new_container(STERILE_MICROFUGE_TUBE, adaptor_mix);
	Container sterile_microfuge_tube2 = new_container(STERILE_MICROFUGE_TUBE);
	Container sterile_microfuge_tube3 = new_container(STERILE_MICROFUGE_TUBE);
	Container sterile_microfuge_tube4 = new_container(STERILE_MICROFUGE_TUBE);
	Container sterile_microfuge_tube5 = new_container(STERILE_MICROFUGE_TUBE);
	Container rxn_tube = new_container(RXN_TUBE);

	first_step("Tumor collection and storage");
	collect_tissue(mouse_tumor, cryo_vial);
	store(sterile_microfuge_tube, -80);

	next_step("Extraction of tumor genomic DNA (not shown)");

	next_step("Digestion of tumor genomic DNA with restriction enzyme");
	{
		Fluid fluid_array[5]={dna, sau3a1, neb, bsa, water};
		char* (initial_conc)[5]={"--","20 U µl<sup>-1</sup>","10X","10 mg ml<sup>-1</sup>","--"};
		char* (final_conc)[5]={"2 µg","40 U","1X","0.33 µg µl<sup>-1</sup>","--"};
		Volume* volumes[5]={vol(1, UL), vol(2, UL), vol(3, UL), vol(1, UL), vol(23, UL)};
		mixing_table_pcr(6, vol(30, UL), fluid_array, initial_conc, final_conc, volumes, sterile_microfuge_tube);
	}

	next_step();
	incubate(sterile_microfuge_tube, 37, time_range(12, 16, HRS));

	next_step("Splinkerette adaptor ligation");
	store_until(sterile_microfuge_tube1, 0, THAW_ICE);
	measure_fluid(sterile_microfuge_tube1, vol(1, UL), rxn_tube);
	pcr_init_denat(rxn_tube, 95, time(5, MINS));
	thermocycler_anneal(rxn_tube, RT, 1, time(15, SECS));
	store(rxn_tube, 0);
	name_sample(rxn_tube,"Adaptor mix");


	next_step();
	name_sample(sterile_microfuge_tube, "Sau3AI-digested genomic DNA");
	measure_fluid(sterile_microfuge_tube.contents, vol(5, UL));
	store_for(sterile_microfuge_tube, 65, time(20, MINS));
	store(sterile_microfuge_tube, 0);


	next_step();
	{
		Fluid fluid_array[5] = {sterile_microfuge_tube.contents, rxn_tube.contents, ligase, ligase_buff, water};
		char*(initial_conc)[5] = {"2 µg in 30 µl", "25 µM", "20 U µl<sup>-1</sup>", "10X", "--"};
		char*(final_conc)[5] = {"300 ng", "25 pmol", "20 U", "1X", "--"};
		Volume* volumes[5] = {vol(4.5, UL), vol(1, UL), vol(1, UL), vol(4, UL), vol(29.5, UL)};
		mixing_table_pcr(6, vol(40, UL), fluid_array, initial_conc, final_conc, volumes, sterile_microfuge_tube2);
	}

	next_step();
	incubate( sterile_microfuge_tube2, 4, time_range(12, 16, HRS));

	next_step();
	store_for(sterile_microfuge_tube2, 65, time(20, MINS));
	name_sample(sterile_microfuge_tube2, "Splinkerette adaptor ligation");


	next_step("Digestion to remove genomic DNA fragments with splinkerette adaptor ligated on both ends");
	{
		Fluid fluid_array[5] = {sterile_microfuge_tube2.contents, neb3, ecorv, bsa, water};
		char* (initial_conc)[5] = {"--", "10X", "20 U µl<sup>-1</sup>", "10 mg ml<sup>-1</sup>", "--"};
		char* (final_conc)[5] = {"--", "1X", "20 U", "0.1 µg µl<sup>-1</sup>", "--"};
		Volume* volumes[5]= {vol(40, UL), vol(10, UL), vol(1, UL), vol(1, UL), vol(48, UL)};
		mixing_table_pcr(6, vol(100, UL), fluid_array, initial_conc, final_conc, volumes, sterile_microfuge_tube3);
	}


	next_step();
	incubate(sterile_microfuge_tube3, 37, time(6, HRS));

	next_step();
	store_for(sterile_microfuge_tube3, 65, time(20, MINS));

	next_step("Clean up the EcoRV digestion reaction using the QIAquick Gel Extraction Kit (not shown)");
	name_sample(sterile_microfuge_tube3, "Purified splinkerette adaptor ligated genomic DNA");

	next_step();
	nanodrop(sterile_microfuge_tube3);

	next_step("Primary PCR amplification");
	{
		Fluid fluid_array[7] = {sterile_microfuge_tube3.contents, primer1, primer2, dntp, pfu_buff, pfu, water};
		char* (initial_conc)[7] = {"--", "0.1 µg µl<sup>-1</sup>", "0.1 µg µl<sup>-1</sup>", "25 mM", "10X", "2.5 U µl<sup>-1</sup>", "--"};
		char* (final_conc)[7] = {"--", "0.1 µg", "0.1 µg", "0.25 mM", "1X", "1.25 U", "--"};
		Volume* volumes[7] = {vol(5, UL), vol(1, UL), vol(1, UL), vol(0.5, UL), vol(5, UL), vol(0.5, UL), vol(37, UL)};
		mixing_table_pcr(8, vol(50, UL), fluid_array, initial_conc, final_conc, volumes, sterile_microfuge_tube4);
	}	

	next_step();
	pcr_init_denat(sterile_microfuge_tube4, 94, time(3, MINS));
	thermocycler(sterile_microfuge_tube4, 29, 94, time(15, SECS), 68, time(30, SECS), 72, time(3, MINS), NORMAL);
	pcr_final_ext(sterile_microfuge_tube4, 72, time(5, MINS), 4, time(15, MINS), 4);
	name_sample(sterile_microfuge_tube4, "Primary PCR product");

	next_step("Secondary PCR amplification");
	{
		Fluid fluid_array[5] = {sterile_microfuge_tube4.contents, primer1, primer2, mastermix, water};
		char* (initial_conc)[5] = {"--", "0.1 µg µl<sup>-1</sup>", "0.1 µg µl<sup>-1</sup>", "--", "--"};
		char* (final_conc)[5] = {"--",  "0.1 µg", "0.1 µg", "--", "--"};
		Volume* volumes[5] = {vol(1, UL), vol(1, UL), vol(1, UL), vol(25, UL), vol(22, UL)};
		mixing_table_pcr(6, vol(50, UL), fluid_array, initial_conc, final_conc, volumes, sterile_microfuge_tube5);
	}

	next_step();
	pcr_init_denat(sterile_microfuge_tube5, 94, time(15, MINS));
	thermocycler(sterile_microfuge_tube5, 25, 94, time(15, SECS), 60, time(30, SECS), 72, time(1, MINS), NORMAL);
	pcr_final_ext(sterile_microfuge_tube5, 72, time(5, MINS), 4, time(15, MINS), 4);

	next_step("Agarose-gel analysis of splinkerette-PCR reactions");
	name_sample(sterile_microfuge_tube5, "results of the secondary splinkerette-PCR reaction");
	electrophoresis(sterile_microfuge_tube5, 2);

	end_protocol();
}