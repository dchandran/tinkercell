#include "BioCoder.h"

void main()
{
start_protocol("Preparation of Plasmid DNA by Alkaline Lysis with SDS- Minipreparation");

Fluid medium = new_fluid("rich medium (LB, YT, or Terrific medium)containing appropriate antibiotic", vol(2, ML));
Fluid sol1 = new_fluid("Alkaline Lysis Solution I","50 mM Glucose,25 mM Tris-HCl (pH 8.0), 10 mM EDTA (pH 8.0)");
Fluid sol2 = new_fluid("freshly prepared Alkaline Lysis Solution II","0.2 N NaOH, 1% SDS (w/v)");
Fluid sol3 = new_fluid("Alkaline Lysis Solution III","5 M sodium acetate, glacial acetic acid");
Fluid ethanol = new_fluid("ethanol at room temperature");
Fluid phe_chloro = new_fluid("phenol:chloroform","1:1, v/v");
Fluid te = new_fluid("TE containing 20 µg/ml DNase-free RNase (pancreatic RNase)");
Fluid eth_70 = new_fluid("70% ethanol");
Solid bacteria = new_solid("a single colony of transformed bacteria");

Container flask = new_container(FLASK, medium);
Container microfuge_tube = new_container(STERILE_MICROFUGE_TUBE);
Container sterile_microfuge_tube1 = new_container (STERILE_MICROFUGE_TUBE);
Container sterile_microfuge_tube2 = new_container (STERILE_MICROFUGE_TUBE);

first_step("Preparation of cells");
inoculation(flask, bacteria, 37, time(12, HRS), 1);
next_step();
measure_fluid(flask, vol(1.5, ML), microfuge_tube);
centrifuge_pellet(microfuge_tube, speed(SPEED_MAX, RPM), 4, time(30, SECS));
comment("Leave the pellet as dry as possible.");

next_step("Lysis of Cells");
measure_fluid(sol1, vol(100, UL), microfuge_tube);
resuspend(microfuge_tube);

next_step();
measure_fluid(sol2, vol(200, UL), microfuge_tube);
invert(microfuge_tube, 5);
comment("Do not vortex!");
store(microfuge_tube, ON_ICE);

next_step();
measure_fluid(sol3, vol(150, UL), microfuge_tube);
tap(microfuge_tube, PPT_STOPS_STICKING);
store_for(microfuge_tube, ON_ICE, time_range(3, 5, MINS));

next_step();
centrifuge_phases_top(microfuge_tube, speed(SPEED_MAX, RPM), 4, time(5, MINS), sterile_microfuge_tube1);

optional_step();
measure_prop(sterile_microfuge_tube1, phe_chloro, 1);
vortex(sterile_microfuge_tube1);
centrifuge_phases_top(sterile_microfuge_tube1, speed(SPEED_MAX, RPM), 4, time(2, MINS), sterile_microfuge_tube2);

next_step("Recovery of Plasmid DNA");
measure_prop(sterile_microfuge_tube2, ethanol, 2);
vortex(sterile_microfuge_tube2);
store_for(sterile_microfuge_tube2, RT, time(2, MINS));

next_step();
centrifuge_pellet(sterile_microfuge_tube2, speed(SPEED_MAX, RPM), 4, time(5, MINS));

next_step();
invert_dry(sterile_microfuge_tube2, RT);

next_step();
measure_fluid(eth_70, vol(1, ML), sterile_microfuge_tube2);
invert(sterile_microfuge_tube2);
centrifuge_pellet(sterile_microfuge_tube2, speed(SPEED_MAX, RPM), 4, time(2, MINS));
comment("Take care with this step, as the pellet sometimes does not adhere tightly to the tube.");

next_step();
name_sample(sterile_microfuge_tube2, "open tube");
comment("Remove any beads of ethanol that form on the sides of the tube.");
store_until(sterile_microfuge_tube2, RT, ETHANOL_EVAP, time_range(5, 10, MINS));

next_step();
measure_fluid(te, vol(50, UL), sterile_microfuge_tube2);
dissolve(sterile_microfuge_tube2);
store(sterile_microfuge_tube2,-20);
end_protocol();	
}
