"""
category: Database
name: Search E.coli parts (RegulonDB)
description: search the RegulonDB database for E.coli parts
icon: Plugins/c/database.png
menu: yes
specific for: Part 
tool: yes
"""

from tinkercell import *

try:
    RegulonDB.ECOLI_PROMOTERS.has_key("hello");
except:
    tc_print("RegulonDB is being loaded for the first time...");
    import sys
    if tc_isWindows():
        sys.path.append(tc_appDir().replace("/","\\") + "\\DB\\Regulon");
    else:
        sys.path.append(tc_appDir() + "/DB/Regulon/");
    import RegulonDB
    tc_print("RegulonDB tables loaded");

items = tc_selectedItems();

if items.length < 1:
    tc_errorReport('No items selected');
else:

    promoters = [];
    coding = [];
    rbs = [];
    terminators = [];
    tfs = [];

    for n in range(0,items.length):
        i = nthItem(items,n);
        
        if tc_isA(i,"Promoter"):
            promoters.append(i);
            
        elif tc_isA(i,"Coding"):
            coding.append(i);
            
        elif tc_isA(i,"RBS"):
            rbs.append(i);
            
        elif tc_isA(i,"Terminator"):
            terminators.append(i);
            
        elif tc_isA(i,"Transcription Factor"):
            tfs.append(i);

    promoterNames = fromStrings( tc_getUniqueNames( toItems(promoters) ) );
    rbsNames = fromStrings( tc_getUniqueNames( toItems(rbs) ) );
    codingNames = fromStrings( tc_getUniqueNames( toItems(coding) ) );
    terminatorNames = fromStrings( tc_getUniqueNames( toItems(terminators) ) );
    tfNames = fromStrings( tc_getUniqueNames( toItems(tfs) ) );
    ignoreList = []; #list of names not found in the DB

    for i in range(0,len(promoters)): #for each selected promoter
        
        p = promoters[i]
        name = promoterNames[i];
        regulators = [];
        connections = tc_getConnectionsIn(p);
        
        for j in range(0, connections.length):  #look at parts regulating the promoter
            parts = tc_getConnectedNodesIn( nthItem(connections,j) );
            pnames = tc_getUniqueNames(parts);
            for k in range(0, parts.length ):
                if tc_isA( nthItem(parts,k) ,"Protein"): #if a protein regulator
                    regulators.append( nthString(pnames,k) );  #get its name
        
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
            k = tc_getStringFromList("Available regulatory sites from Regulon DB: ", toStrings(displayList) ,"",0);
            if k > -1:
                key = displayList[k];
        else:
            k = tc_getStringFromList("Available sites regulated by " + ", ".join(regulators) + " : ", toStrings(displayList) ,"",0);
            if k > -1:
                key = displayList0[k];
        
        if k > -1 and len(key) > 0 and RegulonDB.ECOLI_BINDING_SITES_SEQUENCE.has_key(key):
            list = RegulonDB.ECOLI_BINDING_SITES_SEQUENCE[key];
            tc_rename(p,key);
            tc_setTextAttribute(p,"RegulonID",list[0]);
            tc_setTextAttribute(p,"Sequence",list[1]);
            tc_print(key + " has been updated with RegulonDB ID and sequence information");
            
    for i in range(0,len(tfs)): #for each selected transcription factor
        
        p = tfs[i]
        name = tfNames[i];
        promoters = [];
        connections = tc_getConnectionsOut(p);
        
        for j in range(0,connections.length):  #look at parts regulated by this tf
            parts = tc_getConnectedNodesOut( nthItem(connections,j) );
            pnames = tc_getUniqueNames(parts);
            for k in range(0,parts.length):
                if tc_isA( nthItem(parts,k) ,"Regulator"): #if regulatory element
                    promoters.append( nthString(pnames,k));  #get its name
        
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
            k = tc_getStringFromList("Available transcription factors from Regulon DB: ", toStrings(displayList),"",0);
            if k > -1:
                key = displayList[k];
        else:
            k = tc_getStringFromList("Transcription factors known to bind " + ", ".join(promoters) + " : ", toStrings(displayList),"",0);
            if k > -1:
                key = displayList0[k];
        
        if k > -1 and len(key) > 0:
            tc_rename(p,key);
            if RegulonDB.ECOLI_TF.has_key(key):
                regid = RegulonDB.ECOLI_TF[key];
                if RegulonDB.ECOLI_BINDING_SITES_INTERACTIONS1.has_key(key):
                    s = ",".join(RegulonDB.ECOLI_BINDING_SITES_INTERACTIONS1[key].keys());
                    tc_setTextAttribute(p,"targetPromoter",s);
                tc_setTextAttribute(p,"RegulonID",regid);
                tc_print(key + " has been updated with RegulonDB ID and target promoter(s) information");
    
    for i in range(0,len(rbs)): #for each selected promoter
        
        p = rbs[i]
        name = rbsNames[i];
        
        displayList = [];
        
        k = -1;
        key = "";
        displayList = RegulonDB.ECOLI_RBS.keys();
        k = tc_getStringFromList("Available RBS sites from Regulon DB: ",toStrings(displayList),"",0);
        if k > -1:
            key = displayList[k];
        
        if k > -1 and len(key) > 0:
            list = RegulonDB.ECOLI_RBS[key];
            tc_rename(p,key);
            tc_setTextAttribute(p,"RegulonRB",list[0]);
            tc_setTextAttribute(p,"Sequence",list[1]);
            tc_print(key + " has been updated with RegulonDB ID and sequence information");
    
    for i in range(0,len(terminators)): #for each selected promoter
        
        p = terminators[i]
        name = terminatorNames[i];
        
        displayList = [];
        k = -1;
        key = "";
        displayList = RegulonDB.ECOLI_TERMINATORS.keys();
        k = tc_getStringFromList("Available terminator sites \n (from listed orfs) from Regulon DB: ",toStrings(displayList),"",0);
        if k > -1:
            key = displayList[k];
        
        if k > -1 and len(key) > 0:
            list = RegulonDB.ECOLI_TERMINATORS[key];
            tc_rename(p,key);
            tc_setTextAttribute(p,"RegulonRB",list[0]);
            tc_setTextAttribute(p,"Rho Factor",list[1]);
            tc_setTextAttribute(p,"Sequence",list[2]);
            tc_print(key + " has been updated with RegulonDB ID, rho-factor, and sequence information");
    
    if len(ignoreList) > 0:
        print "The following items were not found in RegulonDB: " + ",".join(ignoreList);
