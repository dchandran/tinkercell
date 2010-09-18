#include "BioCoder.h"

void main()
{
	start_protocol("Small-scale plasmid isolation - Maxiprep");

	Fluid culture = new_fluid("overnight culture");
	Fluid sol1 = new_fluid("Solution I", ICE_COLD);
	Fluid sol2 = new_fluid("Solution II");
	Fluid sol3 = new_fluid("Solution III");
	Fluid phenol = new_fluid("phenol");
	Fluid chloroform = new_fluid("chloroform");
	Fluid rnase = new_fluid("RNase to a final concentration of 3痢/ml", vol(2, UL));
	Fluid isoprop = new_fluid("isopropanol", ICE_COLD);
	Fluid etoh70 = new_fluid("70% ethanol");
	Fluid water = new_fluid("sterile distilled water");

	Container tube1 = new_container(CENTRI_TUBE_50ML);
	Container tube2 = new_container(CENTRIFUGE_TUBE_15ML);
	Container tube3 = new_container(CENTRIFUGE_TUBE_15ML);
	Container tube4 = new_container(CENTRIFUGE_TUBE_15ML);

	//Cell lysis:
	first_step("Cell Lysis");
	//   1. Take the overnight culture and spin down 1.5mL (50mL) of cells for 3 (5) minutes.
	first_sub_step();
	measure_fluid(culture, vol(50, ML), tube1);
	centrifuge_pellet(tube1, speed(SPEED_MAX, G), RT, time(5, MINS));

	//   3. Re-suspend the pellet in 100無 (1mL) of ice cold Solution I.
	next_sub_step();
	measure_fluid(sol1, vol(1, ML), tube1);
	resuspend(tube1);

	//   4. Add 200無 (2mL) of Solution II and gently invert the tube (roll the falcon) to mix. Leave on ice for 5 minutes.
	next_sub_step();
	measure_fluid(sol2, vol(2, ML), tube1);
	invert(tube1);
	store_for(tube1, ON_ICE, time(5, MINS));

	//   5. Add 150無 (1.5mL) of Solution III and shake vigorously. Leave on ice for 10 minutes.
	next_sub_step();
	measure_fluid(sol3, vol(1.5, ML), tube1);
	vortex(tube1);
	store_for(tube1, ON_ICE, time(10, MINS));

	//   6. Centrifuge for 5 minutes. 
	next_sub_step();
	centrifuge_phases_top(tube1, speed(SPEED_MAX, RPM), RT, time(5, MINS), tube2);

	//Phenol/Chloroform Cleanup:
	next_step("Phenol/Chloroform Cleanup");
	//   1. Transfer the supernatant by pipetting to a clean MCT (15mL falcon).
	//   2. Add 0.5 volumes of phenol and 0.5 volumes of chloroform to 1 volume of supernatant ~250:250->500無 (2.5:2.5->5mL).
	first_sub_step();
	measure_prop(tube2, phenol, 0.5);
	measure_prop(tube2, chloroform, 0.5);

	//   3. Mix well by vortexing and centrifuge for 5 minutes.
	//   4. Carefully take the top aqueous phase and transfer to a new tube. 
	next_sub_step();
	vortex(tube2);
	centrifuge_phases_top(tube2, speed(SPEED_MAX, RPM), RT, time(5, MINS), tube3);

	//RNase (optional):
	optional_step("RNase");
	//   1. Add RNase to a final concentration of 3痢/mL (5無 of 3mg/mL diluted stock / 5無 of 30mg/mL stock).
	first_sub_step();
	measure_fluid(rnase, tube3);

	//   2. Incubate in a 37蚓 water bath for 30-45 minutes.
	next_sub_step();
	incubate(tube3, 37, time_range(30, 45, MINS));
	comment("Use a water bath.");

	//   3. Repeat the phenol/chloroform step. 
	next_sub_step();
	measure_prop(tube3, phenol, 0.5);
	measure_prop(tube3, chloroform, 0.5);
	vortex(tube3);
	centrifuge_phases_top(tube3, speed(SPEED_MAX, RPM), RT, time(5, MINS), tube4);

	//Alcohol Precipitation / Purification:
	next_step("Alcohol Precipitation / Purification");
	//   1. Add an equal volume of ice cold isopropanol to the aqueous solution.
	first_sub_step();
	measure_prop(tube4, isoprop, 1);

	//   2. Mix well by inversion and leave on ice for 10 minutes.
	next_sub_step();
	invert(tube4);
	store_for(tube4, ON_ICE, time(10, MINS));

	//   3. Centrifuge at maximum speed for 15 minutes.
	//   4. Carefully pour off the isopropanol leaving the DNA pellet.
	next_sub_step();
	centrifuge_pellet(tube4, speed(SPEED_MAX, RPM), RT, time(15, MINS));


	//   5. Wash the pellet with 200無 (2mL) of 70% ethanol by pipetting.
	next_sub_step();
	measure_fluid(etoh70, vol(2, ML), tube4);
	pipet(tube4);

	//   6. Centrifuge for 5 minutes.
	next_sub_step();
	centrifuge_pellet(tube4, speed(SPEED_MAX, G), RT, time(5, MINS));

	//   7. Remove all the alcohol by pipetting and air dry the pellet for up to 30 minutes.
	next_sub_step();
	dry_pellet(tube4, IN_AIR, max_time(30, MINS));

	//   8. Re-suspend the DNA pellet in 30-50無 (200-300無) sdH2O.
	next_sub_step();
	measure_fluid(water, vol_range(200, 300, UL), tube4);
	resuspend(tube4);

	//   9. Store the samples at -20蚓. 
	next_sub_step();
	store(tube4, -20);

	end_protocol();
}