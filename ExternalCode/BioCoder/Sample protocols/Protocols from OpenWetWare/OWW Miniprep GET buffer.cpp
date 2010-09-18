#include "BioCoder.h"

void main()
{
	start_protocol("Miniprep - GET Buffer");

	Fluid culture = new_fluid("overnight culture");
	Fluid get = new_fluid("GET buffer", "50 mM glucose (MW 180), 10mM EDTA, 25 mM Tris-HCl pH 8", ICE_COLD);
	Fluid naoh = new_fluid("0.2M NaOH", RT);
	Fluid kac = new_fluid("potassium acetate solution", "3 M potassium acetate, 1.8 M acetic acid, no pH adjustment", ICE_COLD);
	Fluid etoh100 = new_fluid("95% / 100% ethanol");
	Fluid etoh70 = new_fluid("70% EtOH");
	Fluid te = new_fluid("TE buffer");
	Fluid water = new_fluid("distilled water");
	Fluid pca = new_fluid("PCA solution", "(optional)50 parts phenol, 49 parts chloroform, and 1 part amyl-alcohol");

	Solid sds = new_solid("SDS");
	Solid lysozyme = new_solid("lysozyme", "optional");

	Container sterile_microfuge_tube = new_container(STERILE_MICROFUGE_TUBE2ML);
	Container sterile_microfuge_tube1 = new_container(STERILE_MICROFUGE_TUBE2ML);
	Container sterile_microfuge_tube2 = new_container(STERILE_MICROFUGE_TUBE2ML);
	Container sterile_microfuge_tube3 = new_container(STERILE_MICROFUGE_TUBE2ML);

	//1. Pipet 2 ml of overnight culture into a 2 ml centrifuge tube.
	first_step();
	measure_fluid(culture, vol(2, ML), sterile_microfuge_tube);

	//2. Centrifuge for 1 minute at maximum speed and discard the supernatant.
	next_step();
	centrifuge_pellet(sterile_microfuge_tube, speed(SPEED_MAX, RPM), RT, time(1, MINS));

	//3. Add 100ul of refrigerated GET buffer to the pellet and vortex to resuspend.
	next_step();
	measure_fluid(get, vol(100, UL),sterile_microfuge_tube);
	resuspend(sterile_microfuge_tube);
	name_sample(sterile_microfuge_tube, "cell suspension");

    // After step 3 add 10mg lysozyme and incubate for 30 mins before proceeding with step 4. This step is essential for lysing gram-positive cells.
	optional_step();
	measure_solid(lysozyme, 10, MG, sterile_microfuge_tube);
	store_for(sterile_microfuge_tube, RT, time(30, MINS));
	comment("This step is essential for lysing gram-positive cells.");

	//4. Add 2mg of SDS to 200ul of room temperature 0.2M NaOH and vortex to mix.
	next_step();
	measure_fluid(naoh, vol(200, UL), sterile_microfuge_tube1);
	measure_solid(sds, 2, MG, sterile_microfuge_tube1);
	vortex(sterile_microfuge_tube1);
	name_sample(sterile_microfuge_tube1, "alkaline SDS solution");

	//5. Apply the alkaline SDS solution to the cell suspension and invert to mix. DO NOT VORTEX! The solution should become clear.
	next_step();
	measure_fluid(sterile_microfuge_tube1, sterile_microfuge_tube);
	invert(sterile_microfuge_tube);
	comment("DO NOT VORTEX! The solution should become clear.");

	//6. Add 150ul of refrigerated potassium acetate solution and invert gently to mix. DO NOT VORTEX! A precipitate should form.
	next_step();
	measure_fluid(kac, vol(150, UL), sterile_microfuge_tube);
	invert(sterile_microfuge_tube);
	comment("DO NOT VORTEX! A precipitate should form.");

	//7. Store the tube on ice for 3-5 minutes.
	next_step();
	store_for(sterile_microfuge_tube, ON_ICE, time_range(3, 5, MINS));

	//8. Centrifuge for 10 minutes at maximum speed.
	//9. Carefully pipet 400ul of the clean supernatant into a new tube. DO NOT PICK UP ANY PRECIPITATE!!!
	next_step();
	centrifuge_phases_top(sterile_microfuge_tube, speed(SPEED_MAX, RPM), RT, time(10, MINS), vol(400, UL), sterile_microfuge_tube2);
	comment("DO NOT PICK UP ANY PRECIPITATE!!!");

    //After step 9 add 400µl PCA solution to the tube, invert to mix, and centrifuge for 3 minutes at maximum speed. Collect the upper phase by pipetting into a new tube and proceed with step 10. This step helps remove any residual proteins. 
	optional_step();
	measure_fluid(pca, vol(400, UL), sterile_microfuge_tube2);
	invert(sterile_microfuge_tube2);
	centrifuge_phases_top(sterile_microfuge_tube2, speed(SPEED_MAX, RPM), RT, time(3, MINS), sterile_microfuge_tube3);
	comment("This helps remove any residual proteins.");
	sterile_microfuge_tube2 = sterile_microfuge_tube3;

	//10. Add 900ul of 100% (95% is ok too) EtOH to precipitate the plasmid DNA.
	next_step();
	measure_fluid(etoh100, vol(900, UL), sterile_microfuge_tube2);
	comment("This is to precipitate the plasmid DNA.");

	//11. Place the tubes in the -80 freezer for 30 minutes.
	next_step();
	store_for(sterile_microfuge_tube2, -80, time(30, MINS));
	comment("Use the -80°C freezer.");

	//12. Centrifuge the precipitated plasmid DNA 10 minutes at maximum speed and discard supernatant.
	next_step();
	centrifuge_pellet(sterile_microfuge_tube2, speed(SPEED_MAX, RPM), RT, time(10, MINS));

	//13. Carefully add 1ml 70% EtOH to the pellet and let sit for 3 minutes.
	next_step();
	measure_fluid(etoh70, vol(1, ML), sterile_microfuge_tube2);
	store_for(sterile_microfuge_tube2, RT, time(3, MINS));

	//14. Centrifuge at maximum speed for 3 minutes. Make sure the pellet is toward the outside.
	next_step();
	centrifuge_pellet(sterile_microfuge_tube2, speed(SPEED_MAX, RPM), RT, time(3, MINS));
	comment("Make sure the pellet is toward the outside.");

	//15. Discard the supernatant and air dry the pellet for 10-15 minutes.
	next_step();
	dry_pellet(sterile_microfuge_tube2, IN_AIR, time_range(10, 15, MINS));

	//16. Once the pellet is COMPLETELY DRY resuspend the plasmid DNA in 20 ul of TE or distilled water. The DNA will contain RNA contamination, which can be removed by resuspending in TE with RNAse
	next_step();
	comment("Make sure the pellet is completely dry before this step.");
	first_option();
	measure_fluid(te, vol(20, UL), sterile_microfuge_tube2);
	next_option();
	measure_fluid(water, vol(20, UL), sterile_microfuge_tube2);
	end_option();
	resuspend(sterile_microfuge_tube2);
	comment(" The DNA will contain RNA contamination, which can be removed by resuspending in TE with RNAse.");

	end_protocol();
}

