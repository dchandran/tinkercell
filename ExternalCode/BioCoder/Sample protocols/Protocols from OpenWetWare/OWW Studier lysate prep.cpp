#include "BioCoder.h"

void main()
{
	start_protocol("Studier Lysate Prep");

	Fluid culture = new_fluid("fresh overnight culture", "BL21 if wild-type T7");
	Fluid t_broth = new_fluid("T broth");
	Solid plaque = new_solid("a single plaque");
	Solid nacl = new_solid("NaCl");

	Container flask = new_container(FLASK);

	//1.  Add 5mL fresh overnight (BL21 if wild-type T7) into 15 mL of T broth (125 mL flask).
	//       * I often use LB --Sri Kosuri 15:18, 3 Jun 2005 (EDT) 
	first_step();
	measure_fluid(t_broth, vol(15, ML), flask);
	measure_fluid(culture, vol(5, ML), flask);

	//2. Add a single plaque to above (or a drop of lysate)
	//3. Shake at 30°C until lysis is visually apparent (2-3hr)
	next_step();
	measure_solid(plaque, flask);
	incubate(flask, 30, time_range(2, 3, HRS), 200);
	comment("Lysis should become visually apparent");

	//4. As soon as lysis is observed and appears to be complete, add 1g of NaCL to the flask and dissolve by shaking.
	//       * Adding NaCl prevents phage particles from getting pulled down during centrifugation. 
	next_step();
	measure_solid(nacl, 1, GR, flask);
	dissolve(flask);
	comment("Adding NaCl prevents phage particles from getting pulled down during centrifugation.");

	//5. Once NaCl is dissolved, centrifuge for 10 min at 10,000 rpm at 4°C.
	//       * Studier has observed that leaving released phage in the cell debris causes a reduction in the kinetics of phage binding
	//(personal communication). It becomes difficult to get good time courses in this case. This can often happen even over the course of an hour. So it is important to catch lysis and spinning down the debris as soon as possible. It is recommended when making a large stock that will be used in timing experiments,
	//that you check that most of your phage adsorb to the cell in the first 30 seconds. 
	next_step();
	centrifuge(flask, speed(10000, RPM), 4, time(10, MINS));
	comment("Studier has observed that leaving released phage in the cell debris causes a reduction in the kinetics of phage binding (personal communication). It becomes difficult to get good time courses in this case. This can often happen even over the course of an hour. So it is important to catch lysis and spinning down the debris as soon as possible. It is recommended when making a large stock that will be used in timing experiments, that you check that most of your phage adsorb to the cell in the first 30 seconds.");

	//6. Store tube in fridge away from light (usually good indefinitely, e.g., retains absorption well). 
	next_step();
	store(flask, 4);
	comment("Use a fridge away from light (usually good indefinitely, e.g., retains absorption well).");

	end_protocol();
}