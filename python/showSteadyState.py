"""
category: Steady state
name: Compute steady state
description: compute steady state and eigenvalues using PySCeS
icon: Plugins/c/pysces.png
menu: yes
"""


import numpy
import pysces
from tc2pysces import *

outputOff();
mod = getPyscesModel();
mod.doState();
outputOn();
mod.doEigenShow();
print mod.jacobian;
names = toStrings( mod.jacobian_row );
items = fromItems( tc_findItems(names) );
values = mod.data_sstate.getSpecies();
for i in range(0,len(items)):
    tc_displayNumber(items[i],values[i]);

outputOn();
