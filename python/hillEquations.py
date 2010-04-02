"""
category: Generate kinetics
name: Hill equations
description: automatically generate the equilibrium rate equation for transcription
icon: Plugins/c/hillequation.png
menu: yes
specific for: Synthesis 
tool: yes
"""

from tinkercell import *
items = tc_selectedItems();
synthesis = [];
for i in range(0,items.length):
	if tc_isA( nthItem(items,i),"Synthesis"):
		synthesis.append( nthItem(items,i) );

if (len(synthesis) > 0):
	strList = toStings(("Auto","Activation","Repression","AND","OR","NOR","XOR"));
	k = tc_getFromList("Select the logical function to approximate:",strList);
	deleteArrayOfStrings(strList);
	if k > -1:
		for i in synthesis:
			fracs = [];
			indiv = [];
			connectors = [];
			promotername = "";
			genes = tc_getConnectedNodesIn( nthItem(synthesis,i) );
			if genes.length > 0 and tc_isA( nthItem(genes,0) ,"Part"):
				upstream = tc_partsUpstream( nthItem(genes,0) );
				for j in range(0,upstream.length):
					p = nthItem(upstream,j);
					if tc_isA(p,"Promoter"): promotername = tc_getName(p);
					connectors = tc_getConnectionsIn(p);
					isRepressor = False;
					for k in range(0,connectors.length):
						c = nthItem(connectors,k);
						isRepressor = (k==0 and tc_isA(c,"Transcription Repression"));
						cname = tc_getName(c);
						parts = tc_getConnectedNodesIn(c);
						pnames = tc_getNames(parts);
						for n in range(0,pnames.length):
							s = "((" + nthString(pnames,n) + "/" + cname + ".Kd)^" + cname + ".h)";
							if not isRepressor:
								indiv.append(s);
							s = "(1+" + s + ")";
							fracs.append(s);
				p = genes[0];
				if tc_isA(p,"Promoter"): promotername = tc_getName(p);
			rate = "0.0";			
			if len(promotername) > 0:
				rate = "";
				if k == 0 or k == 3:
					if len(indiv) < 1:
						indiv.append("1.0");
					rate = " * ".join(indiv) + "/(" + "*".join(fracs) + ")";
				elif k == 4 or k == 1:
					rate = "(" + " * ".join(fracs) + "- 1)/(" + "*".join(fracs) + ")";
					for j in range(0,connectors.length):
						c = nthItem(connectors,j);
						tc_changeArrowHead(c,"ArrowItems/TranscriptionActivation.xml");
				elif k == 2 or k == 5:
					rate = " 1.0/(" + "*".join(fracs) + ")";
					for j in range(0,connectors.length):
						c = nthItem(connectors,j);
						tc_changeArrowHead(c,"ArrowItems/TranscriptionRepression.xml");
				elif k == 6:
					rate = "(" + " + ".join(indiv) + ")/(" + "*".join(fracs) + ")";
					for j in range(0,connectors.length):
						c = nthItem(connectors,j);
						tc_changeArrowHead(c,"ArrowItems/TranscriptionRegulation.xml");
				name = tc_getName( nthItem(synthesis,i) );
				if rate == "1.0/()":
					rate = promotername + ".strength";
				else:
					rate = promotername + ".strength*" + rate;
				tc_print(name + " has rate : " + rate+"\n");
				tc_setRate(i,rate);
			else:
				tc_print("no promoter found for this transcription reaction\n");
else:
	tc_errorReport("please select at least one transcription reaction");
