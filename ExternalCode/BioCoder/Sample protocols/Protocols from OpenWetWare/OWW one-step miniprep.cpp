#include "BioCoder.h"

void main()
{
	start_protocol("One step miniprep method for the isolation of plasmid DNA");

	Fluid culture = new_fluid("overnight E.coli culture");
	Fluid phe_chloro_iaa = new_fluid ("phenol : chloroform : isoamyl alcohol(25:24:1)", "phenol saturated with TE(10mM Tris, 7.5, 1mM EDTA) prior to mixing with chloroform and isoamylalcohol");
	Fluid isoprop = new_fluid("isopropanol");
	Fluid eth70 = new_fluid("70% ethanol");
	Fluid rnase = new_fluid("100 µl/ml RNAse");

	Container flask = new_container(FLASK, culture);
	Container sterile_microfuge_tube = new_container(STERILE_MICROFUGE_TUBE);
	Container sterile_microfuge_tube2 = new_container(STERILE_MICROFUGE_TUBE);

	//1. Take 0.5ml of overnight E.coli culture in a microfuge tube. We routinely grow our cells in 'standard 1' bacteriological media supplied by Merck, Germany.
	first_step();
	measure_fluid(flask, vol(0.5, ML), sterile_microfuge_tube);
	comment("We routinely grow our cells in 'standard 1' bacteriological media supplied by Merck, Germany.");

	//2. Add 0.5ml of phenol:chloroform:isoamylalcohol (25:24:1). The phenol was saturated with TE (10mM Tris, 7.5, 1mM EDTA) prior to mixing with chloroform and isoamylalcohol.
	next_step();
	measure_fluid(phe_chloro_iaa, vol(0.5, ML), sterile_microfuge_tube);

	//3. Mix by vortexing at the maximum speed for 1 minute. Alternatively, vortex for 10 seconds and then transfer to eppendorf mixer or an over-the-top rotator for 5 minutes.
	next_step();
	vortex(sterile_microfuge_tube, time(1, MINS));
	comment("Vortex at maximum speed.");
	comment("Alternatively, vortex for 10 seconds and then transfer to eppendorf mixer or an over-the-top rotator for 5 minutes.");

	//4. Spin at 12,000g for 5 minutes. During the spin, prepare microfuge tubes with 0.5ml of isopropanol. After the spin, remove carefully about 0.45ml of the upper aqueous phase leaving the interphase undisturbed and add it to the isopropanol. Mix well and spin immediately at 12,000 g for 5 minutes. Addition of salt and cooling is unnecessary.
	next_step();
	centrifuge(sterile_microfuge_tube, speed(12000, G), RT, time(5, MINS));

	parallel_step();
	name_container(sterile_microfuge_tube2, "Tube I");
	measure_fluid(isoprop, vol(0.5, ML), sterile_microfuge_tube2);

	next_step();
	name_sample(sterile_microfuge_tube, "top aqueous phase obtained after centrifugation");
	measure_fluid(sterile_microfuge_tube, vol(0.45, ML), sterile_microfuge_tube2);
	vortex(sterile_microfuge_tube2);
	centrifuge_pellet(sterile_microfuge_tube2, speed(12000, G), RT, time(5, MINS));
	comment("Addition of salt and cooling is unnecessary.");

	//5. Pour off the supernatant, add carefully 0.5ml of 70% ethanol to the side of the tube, pour off. Repeat the washing once more. Vacuum dry the pellet and suspend in 100ul/ml RNAse). About 5-10ul of this DNA can now be cleaved with appropriate restriction enzyme(s) for analysis.
	next_step();
	measure_fluid(eth70, vol(0.5, ML), sterile_microfuge_tube2);
	comment("Add ethanol carefully to the side of the tube.");
	name_sample(sterile_microfuge_tube2, "solution");
	discard(sterile_microfuge_tube2);

	next_step();
	measure_fluid(eth70, vol(0.5, ML), sterile_microfuge_tube2);
	comment("Add ethanol carefully to the side of the tube.");
	name_sample(sterile_microfuge_tube2, "solution");
	discard(sterile_microfuge_tube2);
	measure_fluid(rnase, sterile_microfuge_tube2);
	resuspend(sterile_microfuge_tube2);
	comment("About 5-10ul of this DNA can now be cleaved with appropriate restriction enzyme(s) for analysis.");

	end_protocol();
}
