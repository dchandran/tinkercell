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
deleteArrayOfItems(items);

if (len(synthesis) > 0):
	strList = toStrings(("Auto","Activation","Repression","AND","OR","NOR","XOR"));
	t = tc_getStringFromList("Select the logical function to approximate:",strList,"Auto");
	deleteArrayOfStrings(strList);
	if t > -1:
		for i in synthesis:
			fracs = [];
			indiv = [];
			indiv2 = [];
			connectors = [];
			promotername = "";
			genes = tc_getConnectedNodesIn( i );
			if genes.length > 0 and tc_isA( nthItem(genes,0) ,"Part" ):
				upstream = tc_partsUpstream( nthItem(genes,0) );
				for j in range(0,upstream.length):
					p = nthItem(upstream,j);
					if tc_isA(p,"Promoter"): promotername = tc_getUniqueName(p);
					connectors2 = tc_getConnectionsIn(p);
					isRepressor = False;
					for k in range(0,connectors2.length):
						c = nthItem(connectors2,k);
						connectors.append(c);
						isRepressor = (t==0 and tc_isA(c,"Transcription Repression"));
						cname = tc_getUniqueName(c);
						parts = tc_getConnectedNodesIn(c);
						pnames = tc_getUniqueNames(parts);
						for n in range(0,pnames.length):
							s = "((" + nthString(pnames,n) + "/" + cname + ".Kd)^" + cname + ".h)";
							if not isRepressor:
								indiv.append(s);
							indiv2.append(s);
							s = "(1+" + s + ")";
							fracs.append(s);
					deleteArrayOfItems(connectors2);
				p = nthItem(genes,0);
				if tc_isA(p,"Promoter"):
					promotername = tc_getUniqueName(p);
			deleteArrayOfItems(genes);
			rate = "0.0";			
			if len(promotername) > 0:
				rate = "";
				if t == 0:
					if len(indiv) < 1:
						indiv.append("1.0");
					rate = " * ".join(indiv) + "/(" + "*".join(fracs) + ")";
				elif t == 3:
					if len(indiv2) < 1:
						indiv2.append("1.0");
					rate = " * ".join(indiv2) + "/(" + "*".join(fracs) + ")";
					for c in connectors:
						tc_changeArrowHead(c,"ArrowItems/TranscriptionActivation.xml");
						tc_setColor(c,"#049102",1);
				elif t == 4 or t == 1:
					rate = "(" + " * ".join(fracs) + "- 1)/(" + "*".join(fracs) + ")";
					for c in connectors:
						tc_changeArrowHead(c,"ArrowItems/TranscriptionActivation.xml");
						tc_setColor(c,"#049102",1);
				elif t == 2 or t == 5:
					rate = " 1.0/(" + "*".join(fracs) + ")";
					for c in connectors:
						tc_changeArrowHead(c,"ArrowItems/TranscriptionRepression.xml");
						tc_setColor(c,"#C30000",1);
				elif t == 6:
					rate = "(" + " + ".join(indiv) + ")/(" + "*".join(fracs) + ")";
					for c in connectors:
						tc_changeArrowHead(c,"ArrowItems/TranscriptionRegulation.xml");
						tc_setColor(c,"#3232FF",1);
				name = tc_getUniqueName( i );
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
