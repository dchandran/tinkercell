#include "BioCoder.h"

#define Tm 58

void main()
{
	start_protocol("Touchdown PCR");

	Fluid pcr_buffer = new_fluid("PCR buffer", "10X");
	Fluid mgcl2 = new_fluid("MgCl<sub>2</sub>", "25 mM");
	Fluid template_dna = new_fluid("Template DNA");
	Fluid dntp = new_fluid("dNTP mix", "2 m");
	Fluid f_primer = new_fluid("Forward primer", "10 &#181M");
	Fluid r_primer = new_fluid("Reverse primer", "10 &#181M");
	Fluid dna_pol = new_fluid("DNA Polymerase", "5 U &#181l<sup>-1</sup>");
	Fluid ddw = new_fluid("dH<sub>2</sub>O");

	Container rxn_tube = new_container(RXN_TUBE);

	first_step("PCR Step:");
	{
		Fluid fluid_array[8]= {pcr_buffer, mgcl2, template_dna, dntp, f_primer, r_primer, dna_pol, ddw};
		char* (initial_conc)[8]={"10X", "25 mM", "--", "2 mM", "10 &#181M", "10 &#181M", "5 U &#181l<sup>-1</sup>", "--"};
		char* (final_conc)[8]={"1X", "1.5 mM", "--", "200 &#181M", "200 nM", "200 nM", "1.25 U per 50 &#181l", "--"};
		Volume* volumes[8]={vol(5, UL), vol(3, UL), vol(10, UL), vol(5, UL), vol(1, UL), vol(1, UL), vol(0.25, UL), vol(24.75, UL)};
		mixing_table_pcr(9, vol(50, UL), fluid_array, initial_conc, final_conc, volumes, rxn_tube);
	}

	next_step("Thermal cycling:");
	pcr_init_denat(rxn_tube, 95, time(3, MINS));
	thermocycler(rxn_tube, 15, 95, time(30, SECS), (Tm+10), time(45, SECS), 72, time(60, SECS), NORMAL);
	thermocycler(rxn_tube, 25, 95, time(30, SECS), Tm, time(45, SECS), 72, time(60, SECS), NORMAL);
	pcr_final_ext(rxn_tube, 72, time(5, MINS), 4, time(15, MINS), 23);

	next_step("Product electrophoresis:");
	name_sample(rxn_tube, "PCR products");
	electrophoresis(rxn_tube);

	end_protocol();
}