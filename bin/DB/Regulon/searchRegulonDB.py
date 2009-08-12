import pytc

try:
	RegulonDB.ECOLI_PROMOTERS.has_key("hello");
except:
	pytc.write("RegulonDB is being loaded for the first time...");
	import sys
	if pytc.isWindows():
		sys.path.append(pytc.appDir().replace("/","\\") + "\\DB\\Regulon");
	else:
		sys.path.append(pytc.appDir() + "/DB/Regulon/");
	import RegulonDB
	pytc.write("RegulonDB tables loaded");

items = pytc.selectedItems();

if len(items) < 1:
	pytc.errorReport('No items selected');
else:

	promoters = [];
	coding = [];
	rbs = [];
	terminators = [];
	tfs = [];

	for i in items:
		if pytc.isA(i,"Regulator"):
			promoters.append(i);
			
		elif pytc.isA(i,"Coding"):
			coding.append(i);
			
		elif pytc.isA(i,"RBS"):
			rbs.append(i);
			
		elif pytc.isA(i,"Terminator"):
			terminators.append(i);
			
		elif pytc.isA(i,"Transcription Factor"):
			tfs.append(i);

	promoterNames = pytc.getNames(promoters);
	rbsNames = pytc.getNames(rbs);
	codingNames = pytc.getNames(coding);
	terminatorNames = pytc.getNames(terminators);
	tfNames = pytc.getNames(tfs);
	ignoreList = []; #list of names not found in the DB

	for i in range(0,len(promoters)): #for each selected promoter
		
		p = promoters[i]
		name = promoterNames[i];
		regulators = [];
		connections = pytc.getConnectionsIn(p);
		
		for j in connections:  #look at parts regulating the promoter
			parts = pytc.getConnectedPartsIn(j);
			pnames = pytc.getNames(parts);
			for k in range(0,len(parts)):
				if pytc.isA(parts[k],"Protein"): #if a protein regulator
					regulators.append(pnames[k]);  #get its name
		
		displayList = [];
		displayList0 = [];
		
		for j in regulators:  #get Regulon promoters for each protein regulating this promoter
			if RegulonDB.ECOLI_BINDING_SITES_INTERACTIONS1.has_key(j):
				displayList0 = RegulonDB.ECOLI_BINDING_SITES_INTERACTIONS1[j].keys();
				for k in displayList0:
					displayList.append(k + " (" + j + " is a " + RegulonDB.ECOLI_BINDING_SITES_INTERACTIONS1[j][k] + ")");
			elif RegulonDB.ECOLI_BINDING_SITES_INTERACTIONS1.has_key(j.lower()):
				displayList0 = RegulonDB.ECOLI_BINDING_SITES_INTERACTIONS1[j.lower()].keys();
				for k in displayList0:
					displayList.append(k + " (" + j + " is a " + RegulonDB.ECOLI_BINDING_SITES_INTERACTIONS1[j.lower()][k] + ")");
			else:
				ignoreList.append(j);
				
		k = -1;
		key = "";
		if len(displayList) == 0:
			displayList = RegulonDB.ECOLI_BINDING_SITES_INTERACTIONS2.keys();
			k = pytc.getFromList("Available regulatory sites from Regulon DB: ",displayList,0);
			if k > -1:
				key = displayList[k];
		else:
			k = pytc.getFromList("Available sites regulated by " + ", ".join(regulators) + " : ",displayList,0);
			if k > -1:
				key = displayList0[k];
		
		if k > -1 and len(key) > 0 and RegulonDB.ECOLI_BINDING_SITES_SEQUENCE.has_key(key):
			list = RegulonDB.ECOLI_BINDING_SITES_SEQUENCE[key];
			pytc.rename(p,key);
			pytc.setTextAttribute(p,"RegulonID",list[0]);
			pytc.setTextAttribute(p,"Sequence",list[1]);
			pytc.write(key + " has been updated with RegulonDB ID and sequence information");
			
	for i in range(0,len(tfs)): #for each selected transcription factor
		
		p = tfs[i]
		name = tfNames[i];
		promoters = [];
		connections = pytc.getConnectionsOut(p);
		
		for j in connections:  #look at parts regulated by this tf
			parts = pytc.getConnectedPartsOut(j);
			pnames = pytc.getNames(parts);
			for k in range(0,len(parts)):
				if pytc.isA(parts[k],"Regulator"): #if regulatory element
					promoters.append(pnames[k]);  #get its name
		
		displayList = [];
		displayList0 = [];
		targetPromoters = [];
		
		for j in promoters:  #get Regulon tfs for each regulatory element
			if RegulonDB.ECOLI_BINDING_SITES_INTERACTIONS2.has_key(j):
				displayList0 = RegulonDB.ECOLI_BINDING_SITES_INTERACTIONS2[j].keys();
				for k in displayList0:
					displayList.append(k + " (" + RegulonDB.ECOLI_BINDING_SITES_INTERACTIONS2[j][k] + ")");
			elif RegulonDB.ECOLI_BINDING_SITES_INTERACTIONS2.has_key(j.lower()):
				displayList0 = RegulonDB.ECOLI_BINDING_SITES_INTERACTIONS2[j.lower()].keys();
				for k in displayList0:
					displayList.append(k + " (" + RegulonDB.ECOLI_BINDING_SITES_INTERACTIONS2[j.lower()][k] + ")");
			else:
				ignoreList.append(j);
				
		k = -1;
		key = "";
		if len(displayList) == 0:
			displayList = RegulonDB.ECOLI_BINDING_SITES_INTERACTIONS1.keys();
			k = pytc.getFromList("Available transcription factors from Regulon DB: ",displayList,0);
			if k > -1:
				key = displayList[k];
		else:
			k = pytc.getFromList("Transcription factors known to bind " + ", ".join(promoters) + " : ",displayList,0);
			if k > -1:
				key = displayList0[k];
		
		if k > -1 and len(key) > 0:
			pytc.rename(p,key);
			if RegulonDB.ECOLI_TF.has_key(key):
				list = RegulonDB.ECOLI_TF[key];
				if RegulonDB.ECOLI_BINDING_SITES_INTERACTIONS1.has_key(key):
					s = ",".join(RegulonDB.ECOLI_BINDING_SITES_INTERACTIONS1[key].keys());
					pytc.setTextAttribute(p,"targetPromoter",s);
				pytc.setTextAttribute(p,"RegulonID",list);
				pytc.write(key + " has been updated with RegulonDB ID and target promoter(s) information");
	
	for i in range(0,len(rbs)): #for each selected promoter
		
		p = rbs[i]
		name = rbsNames[i];
		
		displayList = [];
		
		k = -1;
		key = "";
		displayList = RegulonDB.ECOLI_RBS.keys();
		k = pytc.getFromList("Available RBS sites from Regulon DB: ",displayList,0);
		if k > -1:
			key = displayList[k];
		
		if k > -1 and len(key) > 0:
			list = RegulonDB.ECOLI_RBS[key];
			pytc.rename(p,key);
			pytc.setTextAttribute(p,"RegulonRB",list[0]);
			pytc.setTextAttribute(p,"Sequence",list[1]);
			pytc.write(key + " has been updated with RegulonDB ID and sequence information");
	
	for i in range(0,len(terminators)): #for each selected promoter
		
		p = terminators[i]
		name = terminatorNames[i];
		
		displayList = [];
		
		k = -1;
		key = "";
		displayList = RegulonDB.ECOLI_TERMINATORS.keys();
		k = pytc.getFromList("Available terminator sites \n (from listed orfs) from Regulon DB: ",displayList,0);
		if k > -1:
			key = displayList[k];
		
		if k > -1 and len(key) > 0:
			list = RegulonDB.ECOLI_TERMINATORS[key];
			pytc.rename(p,key);
			pytc.setTextAttribute(p,"RegulonRB",list[0]);
			pytc.setTextAttribute(p,"Rho Factor",list[1]);
			pytc.setTextAttribute(p,"Sequence",list[2]);
			pytc.write(key + " has been updated with RegulonDB ID, rho-factor, and sequence information");
	
	if len(ignoreList) > 0:
		print "The following items were not found in RegulonDB: " + ",".join(ignoreList);