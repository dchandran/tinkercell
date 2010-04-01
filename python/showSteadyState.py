#TINKERCELL HEADER BEGIN
#category: Steady state
#name: Compute steady state
#descr: compute steady state and eigenvalues using PySCeS
#icon: Plugins/c/pysces.png
#menu: yes
#TINKERCELL HEADER END

import pytc
import numpy
import pysces
import tc2pysces

outputOff();
mod = tc2pysces.load();
mod.doState();
outputOn();
mod.doEigenShow();
print mod.jacobian;
names = mod.jacobian_row;
items = pytc.find(names);
values = mod.data_sstate.getSpecies();
for i in range(0,len(items)):
    pytc.displayNumber(items[i],values[i]);
#pytc.setInitialValues(items,values.tolist());
outputOn();
