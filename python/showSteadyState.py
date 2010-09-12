"""
category: Steady state
name: Compute steady state
description: compute steady state and eigenvalues using PySCeS
icon: pysces.png
menu: yes
"""


import numpy
import pysces
from tc2pysces import *

mod = getPyscesModel();
mod.doState();
mod.doEigenShow();
print mod.jacobian;
names = toTC( mod.jacobian_row );
items = fromTC( tc_findItems(names) );
values = mod.data_sstate.getSpecies();
minV = abs(values[0]);
maxV = abs(values[0]);
for i in values:
    if minV > abs(i):
        minV = abs(i);
    if maxV < abs(i):
        maxV = abs(i);
for i in range(0,len(items)):
    tc_displayNumber(items[i],values[i]);
    tc_setSize(items[i], 10.0 + 100*(abs(values[i]) - minV)/(0.1 + maxV - minV) , 10.0 + 100*(abs(values[i]) - minV)/(0.1 + maxV - minV), 0);

