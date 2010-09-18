#include "BioCoder.h"


void main()
{	
	start_protocol("Methods in Molecular barcodes paper");

	//declare the reagents required for double digestion
	Fluid dig_rxn_buffer = new_fluid("10X reaction buffer");
	Fluid lig_buffer = new_fluid("10X ligation buffer");
	Fluid dna = new_fluid("genomic DNA");
	Fluid rxn_buffer = new_fluid("10X reaction buffer");
	Fluid datp = new_fluid("10mM dATP");
	Fluid dgtp = new_fluid("10mM dGTP");
	Fluid dttp = new_fluid("10mM dTTP");
	Fluid dctp = new_fluid("10mM dCTP");
	Fluid f_primer = new_fluid("forward primer");
	Fluid r_primer = new_fluid("reverse primer");
	Fluid taq = new_fluid("Taq DNA polymerase");
	Fluid rest_enzyme1 = new_fluid("DraII enzyme");
	Fluid rest_enzyme2 = new_fluid("AluI enzyme");
	Fluid sdw = new_fluid("sterile distilled water");
	Fluid hairpin = new_fluid("Hairpin linker");
	Fluid t4ligase = new_fluid("T4 DNA ligase");
	Fluid naoh = new_fluid("NaOH");
	Fluid sodbisul = new_fluid("sodium bisulphite");
	Fluid hydroxy = new_fluid("hydroxyquinone"); 

	Container rxn_tube = new_container(RXN_TUBE);
	Container rxn_tube1 = new_container(RXN_TUBE);
	Container rxn_tube2 = new_container(RXN_TUBE);
	Container rxn_tube3 = new_container(RXN_TUBE);
	Container rxn_tube4 = new_container(RXN_TUBE);

	//double digestion of DNA
	first_step("Double digestion:");
	{
		Fluid fluid_array[5] = {dig_rxn_buffer, dna, rest_enzyme1, rest_enzyme2, sdw};
		Volume* volumes[5] = {vol(2, UL), vol(1, UL), vol(0.5, UL), vol(0.5, UL), vol(16, UL)};
		char* tubes[1] = {"Double digest"};
		mixing_table(2, 6, fluid_array, tubes, volumes, vol(20, UL), rxn_tube);
	}

	//restriction digestion
	next_step();
	store_for(rxn_tube, 37, time(1, HRS));

	//enzyme inactvation
	next_step();
	store_for(rxn_tube, 65, time(20, MINS), ENZYME_INAC);
	name_sample(rxn_tube, "digested products");

	//ligation of haipin linker to cleaved DNA
	next_step("Ligation:");
	{
		Fluid fluid_array[5] = {rxn_tube.contents, lig_buffer, t4ligase, hairpin, sdw};
		char* tubes[1] = {"Ligation"};
		Volume* volumes[5] = {vol(3, UL), vol(2, UL), vol(1, UL), vol(6, UL), vol(8, UL)};
		mixing_table(2, 6, fluid_array, tubes, volumes, vol(20, UL), rxn_tube1);
	}

	//ligation of hairpin linker with cleaved DNA
	next_step();
	store_for(rxn_tube1, 20, time(15, MINS));

	//enzyme inactivation
	next_step();
	store_for(rxn_tube1, 65, time(20, MINS), ENZYME_INAC);
	name_sample(rxn_tube1, "ligated product");

	//thermal denaturation
	next_step("Thermal denaturation:");
	{
		Fluid fluid_array[5] = {rxn_tube1.contents, naoh, sodbisul, hydroxy, sdw};
		char* tubes[1] = {"Thermal denaturation"};
		Volume* volumes[5] = {vol(14, UL), vol(6, UL), vol(5.1, UL), vol(0.03, UL), vol(4.87, UL)};
		mixing_table(2, 6, fluid_array, tubes, volumes, vol(30, UL), rxn_tube2);
	}
	next_step();
	store_for(rxn_tube2, RT, time(20, MINS));

	next_step();
	store_for(rxn_tube2, 100, time(1, MINS));

	//thermal denatureation with sodium bisulphite and hydroxyquinone
	next_step();
	store_for(rxn_tube2, 55, time(6, HRS));

	next_step();
	thermocycler(rxn_tube2, 10, 55, time(35, MINS), 99, time(90, SECS));
	//additional incubation at 55deg C

	next_step();
	store_for(rxn_tube2, 55, time(6, HRS));

	//THIS IS FOLLOWED BY :
	//PURIFICATION WITH QIAquick PCR PURIFICATION COLUMNS (not shown)

	next_step("PURIFICATION WITH QIAquick PCR PURIFICATION COLUMNS (not shown)");
	name_sample(rxn_tube2, "purified products");

	//treatment with NaOH
	next_step();
	measure_fluid(naoh, vol(6, UL), rxn_tube3);
	measure_fluid(rxn_tube2.contents, vol(14, UL), rxn_tube3);

	next_step();
	tap(rxn_tube3);

	next_step();
	store_for(rxn_tube3, 37, time(20, MINS));

	next_step("PURIFICATION WITH microspin s-200 HR COLUMNS (not shown)");
	name_sample(rxn_tube3, "purified products");


	next_step("PCR of purified products:");
	//preparation of master mix
	{
		Fluid fluid_array[10] = {rxn_buffer, datp, dgtp, dttp, dctp, f_primer, r_primer, rxn_tube3.contents, taq, sdw};
		char*(initial_conc)[10] = {"10X", "10 mM", "10 mM", "10 mM", "10 mM", "1 mM", "1 mM", "X", "5 units/痞", "--"};
		char*(final_conc)[10] = {"1X", "200 然", "200 然", "200 然", "200 然", "0.5 然", "0.5 然", "1 ng/痞", "0.02 units/痞", "--"};
		Volume* volumes[10] = {vol(1, UL), vol(0.2, UL), vol(0.2, UL), vol(0.2, UL), vol(0.2, UL), vol(0.5, UL), vol(0.5, UL), vol(1, UL), vol(0.04, UL), vol(XVAL, UL)};
		mixing_table_pcr(11, vol(10, UL), fluid_array, initial_conc, final_conc, volumes, rxn_tube4);
	}

	//initial denaturation step of 15 MINS at 95deg C
	pcr_init_denat(rxn_tube4, 95, time(15 ,MINS));
	//38 cycles of denaturation at 95deg, 30s, annealing at 55deg, 30s, extension at 72deg, 45s
	next_step("38 cycles of PCR:");
	thermocycler(rxn_tube4, 38, 95, time(30, SECS), 58, time(30, SECS), 72, time(45, SECS), NORMAL);

	//final extension at 72deg for 5 min
	pcr_final_ext(rxn_tube4, 72, time(5, MINS), 4, time(15, MINS), 23);

	//ANALYSIS OF PCR PRODUCTS BY GEL ELECTROPHORESIS(not shown)
	next_step("Analysis of PCR products by gel electrophoresis");
	name_sample(rxn_tube4, "PCR products");
	electrophoresis(rxn_tube4);

	next_step();
	use_or_store(rxn_tube4, time(30, MINS), 4, -20);

	end_protocol();
}