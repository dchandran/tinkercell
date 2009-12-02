import pytc
import numpy
import pysces
import tc2pysces

mod = tc2pysces.load();
mod.doState();
mod.doEigenShow();
print mod.jacobian;
names = mod.jacobian_row;
items = pytc.find(names);
values = mod.data_sstate.getSpecies();
pytc.setInitialValues(items,values.tolist());
