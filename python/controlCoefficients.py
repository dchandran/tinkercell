"""
category: Analysis
name: Control coefficients
description: compute control coefficients using PySCeS
icon: Plugins/c/pysces.png
"""

from tinkercell import *
import numpy
import pysces
from tc2pysces import * 
import display

outputOff();
mod = getPyscesModel();
outputOn();
mod.doMca();

tc_showProgress(60);

#get selected nodes, if any, and reactions
selected = tc_selectedItems();
N = tc_stoichiometry(tc_allItems());

tc_showProgress(90);
mod.showCC();
names = [];

if len(selected) == 1:   #if selected items
    s = selected[0];
    name = tc_getName(s);
    inN = False;
    for i in N[0]:
        if i == name:
            inN = True;
            break;
    for i in N[1]:
        names.append(i);
        if i == name:
            inN = True;
    if inN:
        numbers = range(0, len(names));
        for i in range(0,len(names)):
            numbers[i] = mod.cc.__getattribute__(name + "_" + names[i]) 
        display.numbers(names,numbers);

tc_showProgress(100);

tc_zoom(0.99);
