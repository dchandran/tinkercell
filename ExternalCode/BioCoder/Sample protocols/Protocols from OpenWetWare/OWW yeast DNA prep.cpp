#include "BioCoder.h"

void main()
{
	start_protocol("Yeast DNA Prep");

	Fluid yeast_cul = new_fluid("yeast culture grown up to appropriate density", "near saturation");
	Fluid break_buffer = new_fluid("breaking buffer", "2% (v/v) Triton X-100, 1% (w/v) SDS, 100 mM NaCl, 10 mM Tris-Cl, pH 8.0, 1 mM EDTA, pH 8.0");
	Fluid phe_chloro_iaa = new_fluid("phenol : chloroform : isoamyl alcohol", "25:24:1", ICE_COLD);
	Fluid te = new_fluid("TE buffer", "10 mM Tris-Cl, pH 8.0, 1 mM EDTA, pH 8.0");
	Fluid ethanol = new_fluid("95% ethanol", ICE_COLD);
	Fluid water = new_fluid("water");

	Solid glass_beads = new_solid("glass beads");

	Container eppendorf1 = new_container(EPPENDORF);
	Container eppendorf2 = new_container(EPPENDORF);

	//1. grow up yeast culture to appropriate density (near saturation)
	//2. spin 1.5 mls of culture for 1 min in microfuge and aspirate off supernatant
	first_step();
	measure_fluid(yeast_cul, vol(1.5, ML), eppendorf1);
	centrifuge_pellet(eppendorf1, speed(SPEED_MAX, RPM), RT, time(1, MINS));

	//3. resuspend pellet in 200 ul breaking buffer
	next_step();
	measure_fluid(break_buffer, vol(200, UL), eppendorf1);
	resuspend(eppendorf1);

	//4. wear gloves and add:
	//       * 200 ul phenol:choloroform:isoamyl alcohol (25:24:1)
	//       * 200 ul (@200 mg) glass beads 
	next_step();
	measure_fluid(phe_chloro_iaa, vol(200, UL), eppendorf1);
	measure_solid(glass_beads, 200, MG, eppendorf1);
	comment("Wear gloves for this step!");

	//5. close cap tightly and vortex for 2.5 min.
	//       * Be careful when vortexing; label can be dissolved by the phenol.
	//       * Hold cap tightly so it doesn't open or spill.
	next_step();
	vortex(eppendorf1, time(2.5, MINS));
	comment("Be careful when vortexing; label can be dissolved by the phenol.");
	comment("Hold cap tightly so it doesn't open or spill.");

	//6. add 200 ul TE buffer and spin for 5 min, in microfuge
	//7. transfer 350 ml aqueous (top) layer to fresh eppendorf.
	next_step();
	measure_fluid(te, vol(200, UL), eppendorf1);
	centrifuge_phases_top(eppendorf1, speed(SPEED_MAX, RPM), RT, time(5, MINS), vol(350, UL), eppendorf2);

	//8. add 1 ml 95% ethanol and mix well, let sit for 10 minutes
	next_step();
	measure_fluid(ethanol, vol(1, ML), eppendorf2);
	vortex(eppendorf2);
	store_for(eppendorf2, RT, time(10, MINS));

	//9. spin for 2 min, take off supernatant, and let dry upside down 10 min.
	next_step();
	centrifuge_pellet(eppendorf2, speed(SPEED_MAX, RPM), RT, time(2, MINS));
	invert_dry(eppendorf2, RT, time(10, MINS));

	//10. resuspend pellet in 50 ul TE buffer or water. 
	next_step();
	first_option();
	measure_fluid(te, vol(50, UL), eppendorf2);
	next_option();
	measure_fluid(water, vol(50, UL), eppendorf2);
	end_option();
	resuspend(eppendorf2);
	comment("You can now use 1-2 ul of this crude yeast plasmid DNA prep to transform E. coli.");

	end_protocol();
}