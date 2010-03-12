#TINKERCELL HEADER BEGIN
#descr: compute the conservation laws using PySCeS
#category: Network structure
#name: Conservation laws
#icon: Plugins/c/pysces.png
#menu: yes
#TINKERCELL HEADER END

import pytc
import pysces
import tc2pysces
outputOff();
mod = tc2pysces.load();
outputOn();
mod.showNr();
mod.showConserved();
L0 = mod.L0matrix.getLabels();
if len(L0)==2 and len(L0[0]) < len(L0[1]) :
   for s in L0[0]:
      p = pytc.find(s);
      pytc.setColor(p,0,0,0);
   print "Dependent variables are colored black";
