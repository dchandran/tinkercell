#TINKERCELL HEADER BEGIN
#category: Analysis
#name: Control coefficients
#descr: compute control coefficients using PySCeS
#icon: Plugins/c/pysces.png
#TINKERCELL HEADER END

import pytc
import numpy
import pysces
import tc2pysces
import display

outputOff();
mod = tc2pysces.load();
outputOn();
mod.doMca();

pytc.showProgress(60);

#get selected nodes, if any, and reactions
selected = pytc.selectedItems();
N = pytc.stoichiometry(pytc.allItems());

pytc.showProgress(90);
mod.showCC();
names = [];

if len(selected) == 1:   #if selected items
    s = selected[0];
    name = pytc.getName(s);
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

pytc.showProgress(100);

pytc.zoom(0.99);
