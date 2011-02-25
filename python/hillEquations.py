"""
category: Generate kinetics
name: Hill equations
description: automatically generate the equilibrium rate equation for transcription
icon: hillequation.png
menu: yes
specific for: Coding
tool: yes
"""

from tinkercell import *
from tc2py import *

items = tc_selectedItems();
genes = [];
for i in range(0,items.length):
	if tc_isA( tc_getItem(items,i),"Coding"):
		genes.append( tc_getItem(items,i) );
tc_deleteItemsArray(items);

if (len(genes) > 0):
	strList = toStrings(("AND","OR","XOR"));
	t = tc_getStringFromList("Select the logical function to approximate:",strList,"Auto");
	tc_deleteStringsArray(strList);
	if t > -1:
		for i in genes:
			opnames = [];
			opname = "";
			promoter = "";
			upstream = tc_partsUpstream(i);
			for j in range(0,upstream.length):
				p = tc_getItem(upstream,j);
				if tc_isA(p,"Operator"):
					opname = tc_getUniqueName(p);
					if tc_isA(p,"Promoter"):
						promoter = opname;
					if tc_getConnections(p).length > 0:
						opnames.append(opname);
			rate = "0.0";	
			if len(promoter) > 0:
				if len(opnames) < 1:
					rate = promoter + ".strength";
				else:
					if t == 0: #AND
						rate = " * ".join(opnames);
					elif t == 1: #OR
						rate = " + ".join(opnames) + " - " + " * ".join(opnames);
					elif t == 2: #XOR
						rate = " + ".join(opnames) + " - 2 * " + " * ".join(opnames);
					rate = promoter + ".strength * (" + rate + ")";
				name = tc_getUniqueName(i)
				tc_print(name + " has rate : " + rate + "\n");
				tc_addForcingFunction(i, name , rate);
			else:
				tc_print("no promoter found for this coding region\n");
else:
	tc_errorReport("please select a coding region");

