from tinkercell import *
import pysces
from tc2pysces import *

mod = getPyscesModel();

mod.showNr();
mod.showConserved();
L0 = mod.L0matrix.getLabels();
if len(L0) > 0:
   for s in L0[0]:
      p = tc_find(s);
      tc_setColor(p,"#555555",0);
   print "Dependent variables are colored black";
