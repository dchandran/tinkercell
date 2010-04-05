"""
description: compute the conservation laws using PySCeS
category: Network structure
name: Conservation laws
icon: Plugins/c/pysces.png
menu: yes
"""

from tinkercell import *
import pysces
from tc2pysces import *

mod = getPyscesModel();

mod.showNr();
mod.showConserved();
L0 = mod.L0matrix.getLabels();
if len(L0)==2 and len(L0[0]) < len(L0[1]) :
   for s in L0[0]:
      p = tc_find(s);
      tc_setColor(p,0,0,0);
   print "Dependent variables are colored black";
