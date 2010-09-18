#include "BioCoder.h"

void main()
{
	start_protocol("DNA Precipitation");

	Fluid naac = new_fluid("3M Sodium Acetate solution");
	Fluid glycogen = new_fluid("Glycogen");
	Fluid etoh95 = new_fluid("95% EtOH");
	Fluid etoh70 = new_fluid("70% EtOH");
	Fluid water = new_fluid("water");
	Fluid buffer = new_fluid("buffer");
	Fluid dna = new_fluid("DNA sample", vol(20, UL));

	Container eppendorf1 = new_container(EPPENDORF, dna);

	// 1.  Add 0.1 volumes of 3M Sodium Acetate solution to 1 volume of DNA sample.
	first_step();
	measure_prop(eppendorf1, naac, 0.1);

	// 2. Add 1ul Glycogen to the DNA sample.
	next_step();
	measure_fluid(glycogen, vol(1, UL), eppendorf1);

	// 3. Add 2 volumes of 95% EtOH to the DNA Sample.
	next_step();
	measure_prop(eppendorf1, etoh95, 2);

	// 4. Store the solution overnight at -20°C or for 30 minutes at -80°C.
	next_step();
	first_option();
	store_for(eppendorf1, -20, time(12, HRS));
	next_option();
	store_for(eppendorf1, -80, time(30, MINS));
	end_option();

	// 5. Centrifuge the solution at maximum speed for least 15 minutes.
	// 6. Decant and discard the supernatant.
	next_step();
	centrifuge_pellet(eppendorf1, speed(SPEED_MAX, RPM), RT, min_time(15, MINS));

	// 7. (Optional) Add 1 ml of 70% EtOH to the pellet and let sit for 5 minutes.
	optional_step();
	measure_fluid(etoh70, vol(1, ML), eppendorf1);
	store_for(eppendorf1, RT, time(5, MINS));

	// 8. (Optional) Centrifuge the sample at maximum spped for 5 minutes.
	// 9. (Optional) Decant and Discard the supernatant.
	optional_step();
	centrifuge_pellet(eppendorf1, speed(SPEED_MAX, RPM), RT, time(5, MINS));

	//10. Air-dry the pellet for 10-15 minutes at room temperature until all liquid is gone.
	next_step();
	dry_pellet(eppendorf1, IN_AIR, time_range(10, 15, MINS));
	comment("Dry until all the liquid is gone.");

	//11. Resuspend in desired volume of water or buffer 
	next_step();
	first_option();
	measure_fluid(water, eppendorf1);
	next_option();
	measure_fluid(buffer, eppendorf1);
	end_option();
	resuspend(eppendorf1);

	end_protocol();
}