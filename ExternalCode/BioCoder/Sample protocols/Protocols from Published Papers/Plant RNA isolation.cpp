#include "BioCoder.h"


void main()
{
	start_protocol("Plant RNA Isolation");

	Fluid db_buffer = new_fluid("DB homogenization buffer","0.4 M ammonium thiocyanate, 0.8 M guanidine thiocyanate, 38% v/v phenol, 0.1 M sodium acetate(pH 5.5), 5% v/v glycerol");
	Fluid chloroform = new_fluid("chloroform");
	Fluid acid_phe_chloro = new_fluid("acid phenol-chloroform");
	Fluid pot_acetate = new_fluid("potassium acetate", "3 M", ICE_COLD);
	Fluid scp = new_fluid("SCP solution", "1.2 M sodium chloride, 0.8M sodium citrate");
	Fluid etoh = new_fluid("EtOH");
	Fluid etoh_70pc = new_fluid("70% EtOH");
	Fluid water = new_fluid("RNase-free water");

	Tissue tissue1 = new_solid("frozen plant tissue");

	Container sterile_microfuge_tube = new_container(STERILE_MICROFUGE_TUBE, tissue1);
	Container plg = new_container(PLG);
	Container plg1 = new_container(PLG);
	Container oakridge = new_container(OAKRIDGE);

	first_step("Tissue collection and homogenization");
	homogenize_tissue(sterile_microfuge_tube);

	next_step();
	weigh(sterile_microfuge_tube);
	name_sample(sterile_microfuge_tube, "ground tissue");
	comment("The weight of ground tissue is estimated as 1 g(say).");
	sterile_microfuge_tube.volume = 100;
	measure_prop(sterile_microfuge_tube, db_buffer, 4);

	next_step();
	transfer(sterile_microfuge_tube, plg);

	next_step("RNA extraction");
	measure_prop(plg, chloroform, 0.2);

	next_step();
	invert(plg);
	comment("Do not vortex to prevent phase lock gel from breaking into small pieces");

	next_step();
	centrifuge_phases_top(plg, speed(2000, G), 4, time(10, MINS), plg1);

	next_step();
	comment("The volume of the aqueous top layer is estimated as 5 ml.");
	measure_prop(plg1, acid_phe_chloro, 1);
	measure_prop(plg1, pot_acetate, 0.33);
	invert(plg1);

	next_step();
	store_for(plg1, 0, time(30, MINS));

	next_step();
	centrifuge_phases_top(plg1, speed(2000, G), 4, time(10, MINS), oakridge);


	next_step("RNA precipitation");
	comment("The volume of the aqueous top layer is estimated as 5 ml.");
	measure_prop(oakridge, scp, 0.25);

	next_step();
	vortex(oakridge);

	next_step();
	measure_prop(oakridge, etoh, 0.25);

	next_step();
	vortex(oakridge);
	incubate(oakridge, -20, min_time(1, HRS));

	next_step("Washing RNA");
	centrifuge_pellet(oakridge, speed(11000, G), 4, time(10, MINS));

	next_step();
	measure_fluid(etoh_70pc, vol(2, ML), oakridge);
	dissolve(oakridge);

	next_step();
	centrifuge_pellet(oakridge, speed(11000, G), 4, time(10, MINS));

	next_step();
	measure_fluid(etoh_70pc, vol(2, ML), oakridge);
	dissolve(oakridge);
	centrifuge_pellet(oakridge, speed(11000, G), 4, time(10, MINS));

	next_step("Dissolving RNA");
	invert_dry(oakridge, RT, min_time(15, MINS));

	next_step();
	measure_fluid(water, vol(50, UL), oakridge);
	dissolve(oakridge);

	end_protocol();
}