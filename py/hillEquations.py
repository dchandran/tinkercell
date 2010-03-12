#TINKERCELL HEADER BEGIN
#category: Generate kinetics
#name: Hill equations
#descr: automatically generate the equilibrium rate equation for transcription
#icon: Plugins/c/hillequation.png
#menu: yes
#specific for: Synthesis 
#tool: yes
#TINKERCELL HEADER END

import pytc
items = pytc.selectedItems();
synthesis = [];
for i in items:
	if pytc.isA(i,"Synthesis"):
		synthesis.append(i);

if (len(synthesis) > 0):
	k = pytc.getFromList("Select the logical function to approximate:",("Auto","Activation","Repression","AND","OR","NOR","XOR"));
	if k > -1:
		for i in synthesis:
			fracs = [];
			indiv = [];
			connectors = [];
			promotername = "";
			genes = pytc.getConnectedNodesIn(i);
			if len(genes) > 0 and pytc.isA(genes[0],"Part"):
				upstream = pytc.partsUpstream(genes[0]);
				for p in upstream:
					if pytc.isA(p,"Promoter"): promotername = pytc.getName(p);
					connectors = pytc.getConnectionsIn(p);
					isRepressor = False;
					for c in connectors:
						isRepressor = (k==0 and pytc.isA(c,"Transcription Repression"));
						cname = pytc.getName(c);
						parts = pytc.getConnectedNodesIn(c);
						pnames = pytc.getNames(parts);
						for n in pnames:
							s = "((" + n + "/" + cname + ".Kd)^" + cname + ".h)";
							if not isRepressor:
								indiv.append(s);
							s = "(1+" + s + ")";
							fracs.append(s);
				p = genes[0];
				if pytc.isA(p,"Promoter"): promotername = pytc.getName(p);
			rate = "0.0";			
			if len(promotername) > 0:
				rate = "";
				if k == 0 or k == 3:
					if len(indiv) < 1:
						indiv.append("1.0");
					if k == 0:
						rate = " + ".join(indiv) + "/(" + "*".join(fracs) + ")";
					if k == 3:
						rate = " * ".join(indiv) + "/(" + "*".join(fracs) + ")";
						for c in connectors:
							pytc.changeArrowHead(c,"ArrowItems/TranscriptionActivation.xml");
				elif k == 4 or k == 1:
					rate = "(" + " * ".join(fracs) + "- 1)/(" + "*".join(fracs) + ")";
					for c in connectors:
						pytc.changeArrowHead(c,"ArrowItems/TranscriptionActivation.xml");
				elif (k == 2 or k == 5):
					rate = " 1.0/(" + "*".join(fracs) + ")";
					for c in connectors:
						pytc.changeArrowHead(c,"ArrowItems/TranscriptionRepression.xml");
				elif k == 6:
					rate = "(" + " + ".join(indiv) + ")/(" + "*".join(fracs) + ")";
					for c in connectors:
						pytc.changeArrowHead(c,"ArrowItems/TranscriptionRegulation.xml");
				name = pytc.getName(i);
				if rate == "1.0/()":
					rate = promotername + ".strength";
				else:
					rate = promotername + ".strength*" + rate;
				pytc.write(name + " has rate : " + rate+"\n");
				pytc.setRate(i,rate);
			else:
				pytc.write("no promoter found for this transcription reaction\n");
else:
	pytc.errorReport("please select at least one transcription reaction");
