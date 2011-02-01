"""
category: Generate kinetics
name: Hill equations
description: automatically generate the equilibrium rate equation for transcription
icon: hillequation.png
menu: yes
specific for: Production
tool: yes
"""

from tinkercell import *
from tc2py import *

items = tc_selectedItems();
synthesis = [];
for i in range(0,items.length):
	if tc_isA( tc_getItem(items,i),"Production"):
		synthesis.append( tc_getItem(items,i) );
tc_deleteItemsArray(items);

if (len(synthesis) > 0):
	strList = toStrings(("AND","OR","XOR"));
	t = tc_getStringFromList("Select the logical function to approximate:",strList,"Auto");
	tc_deleteStringsArray(strList);
	if t > -1:
		for i in synthesis:
			promoternames = [];
			promotername = "";
			genes = tc_getConnectedNodesWithRole( i , "Template");
			if genes.length > 0 and tc_isA( tc_getItem(genes,0) ,"Part" ):
				upstream = tc_partsUpstream( tc_getItem(genes,0) );
				for j in range(0,upstream.length):
					p = tc_getItem(upstream,j);
					if tc_isA(p,"Operator"):
						promotername = tc_getUniqueName(p);
						promoternames.append(promotername);
					"""
					connectors2 = tc_getConnectionsWithRole(p,"Target");
					isRepressor = False;
					for k in range(0,connectors2.length):
						c = tc_getItem(connectors2,k);
						connectors.append(c);
						cname = tc_getUniqueName(c);
						parts = tc_getConnectedNodesWithRole(c,"Repressor");
						isRepressor = (parts.length > 0);
						if not isRepressor: parts = tc_getConnectedNodesWithRole(c,"Activator");
						pnames = tc_getUniqueNames(parts);
						for n in range(0,pnames.length):
							s = "((" + tc_getString(pnames,n) + "/" + cname + ".Kd)^" + cname + ".h)";
							if not isRepressor:
								indiv.append(s);
							indiv2.append(s);
							s = "(1+" + s + ")";
							fracs.append(s);
					tc_deleteItemsArray(connectors2);
				p = tc_getItem(genes,0);
				if tc_isA(p,"Promoter"):
					promotername = tc_getUniqueName(p);"""
			tc_deleteItemsArray(genes);
			rate = "0.0";	
			if len(promoternames) > 0:
				rate = "";
				if t == 0: #AND
					rate = " + ".join(promoternames);
				elif t == 1: #OR
					rate = " * ".join(promoternames);
				elif t == 2: #XOR
					rate = " + ".join(promoternames) - " * ".join(promoternames);
				tc_print(name + " has rate : " + rate+"\n");
				tc_setRate(i,rate);
			else:
				tc_print("no promoter found for this transcription reaction\n");
else:
	tc_errorReport("please select at least one transcription reaction");

