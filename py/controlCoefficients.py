import pytc
import numpy
import pysces
import tc2pysces

#modelstring = tc2pysces.load();
#pytc.showProgress(30);

#generate pysces model
#mod = pysces.model("model",loader='string', fString=modelstring);

#load pysces model and do MCA
#mod.doLoad();
mod = tc2pysces.load();
mod.doMca();

pytc.showProgress(60);

#get selected nodes, if any, and reactions
selected = pytc.selectedItems();
N = pytc.stoichiometry(pytc.allItems());

pytc.showProgress(90);

mod.showCC();

if len(selected) == 1:   #if selected items
   s = selected[0];
   name = pytc.name(s);
   inN = False;
   for i in N[0]:
      if i == name:
         inN = True;
         break;
   if not inN:
      for i in N[1]:
        if i == name:
           inN = True;
           break;
   if inN:
     fluxes = pytc.itemsOfFamily('connection');  #get flux names
     fluxNames = pytc.names( fluxes );
     n = min(len(fluxes),len(fluxNames)); #both should be the same, but just in case
     s = selected[0];
     prefix = '';
     if pytc.isA(s,'connection'):
        prefix = 'mod.' + 'ccJ' + name + '_';
     if pytc.isA(s,'part'):
        prefix = 'mod.' + 'cc' + name + '_';
     ccs = [];
     if len(prefix) > 0:
        for i in range(0,n):
           z = 0;
           for j in N[1]:
              if j == fluxNames[i]:
                  z = eval( prefix + fluxNames[i] );
                  break;
           ccs.append(z);

        zmax = numpy.max(numpy.abs(ccs));

        for i in range(0,n):
           z = ccs[i];
           pytc.displayNumber( fluxes[i], z );
           z /= zmax;  #scale so that everything is between -1 and 1
           if z == 0:
              pytc.setColor( fluxes[i], 230, 230, 230 );
           elif z < 0:
              pytc.setColor( fluxes[i], 250.0, 0.0, 0.0 );
           else:
              pytc.setColor( fluxes[i], 0.0, 0.0, 250.0 );
           pytc.setLineWidth(fluxes[i], 1 + abs(z) * 10.0); 
   else:   
     print "selected item does not occur in the stoichiometry matrix";
else:   
   print "select a single item to see the related control coefficients on the screen";

pytc.showProgress(100);

pytc.zoom(0.99);
