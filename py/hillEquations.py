import pytc
items = pytc.selectedItems();
promoters = [];
for i in items:
   if pytc.isA(i,"Gene") or pytc.isA(i,"Transcription Regulator"):
      promoters.append(i);

if (len(promoters) > 0):
	k = pytc.getFromList("Select the logical function to approximate:",("Auto","Activation","Repression","AND","OR","NOR","XOR"));
	if k > -1:
		for i in promoters:
			fracs = [];
			indiv = [];
			connectors = pytc.getConnectionsIn(i);
			products = pytc.getConnectionsOut(i);
			
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
			rate = "";
			if k == 0 or k == 3:
				if len(indiv) < 1:
					indiv.append("1.0");
				rate = " * ".join(indiv) + "/(" + "*".join(fracs) + ")";
			elif k == 4 or k == 1:
				rate = "(" + " * ".join(fracs) + "- 1)/(" + "*".join(fracs) + ")";
			elif (k == 2 or k == 5):
				rate = " 1.0/(" + "*".join(fracs) + ")";
			elif k == 6:
				rate = "(" + " + ".join(indiv) + ")/(" + "*".join(fracs) + ")";
			name = pytc.getName(i);
			if rate == "1.0/()":
				rate = name + ".strength";
			else:
				rate = name + ".strength*" + rate;
			if (len(indiv) > 0):
				pytc.write(name + " has rate : " + rate+"\n");
				pytc.setForcingFunction(i,"rate",rate); #pytc.getName(i)
				for j in products:
					pytc.setRate(j,(name + ".rate"));
else:
	pytc.errorReport("no regulatory element selected");
