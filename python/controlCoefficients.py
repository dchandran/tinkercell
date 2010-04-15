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

mod = getPyscesModel();
mod.doMca();

tc_showProgress(60);

#get selected nodes, if any, and reactions
selected = tc_selectedItems();
N = tc_getStoichiometry(tc_allItems());

tc_showProgress(90);
mod.showCC();
names = [];

if selected.length == 1:   #if selected items
    s = nthItem(selected,0);
    name = tc_getUniqueName(s);
    inN = False;
    for i in range(0,N.rows):
        if getRowName(N,i) == name:
            inN = True;
            break;
    for i in range(0,N.cols):
        s = getColumnName(N,i);
        names.append(s);
        if s == name:
            inN = True;
    if inN:
        numbers = range(0, len(names));
        for i in range(0,len(names)):
            numbers[i] = mod.cc.__getattribute__(name + "_" + names[i]) 
        display.numbers(names,numbers);

tc_showProgress(100);

tc_zoom(0.99);
