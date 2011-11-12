"""
category: Perturbation
name: Perturbation Experiments
description: automatically perturb all parameters of promoters, RBS, or mRNA and simulate the model
icon: puzzle.png
menu: yes
tool: yes
"""

import numpy.random
import os
import time
import re

#this function generates files from simulation data
#for each simulated data, multiple output files are produced: mRNA data with noise and n-choose-2 pair-wise protein data with noise
#two mRNA data files are products -- high resolution and low resolution
#all files are places in the tinkercell home folder
def generatefiles(simdata, prefix, addnoise):

#get all protein and mrna names
    protein_items  = tc_itemsOfFamily("Protein")
    mrna_items = tc_itemsOfFamily("mRNA")

    #when using Antimony scripts, there is no ontology, so we use regex
    if protein_items.length == 0:
        protein_items  = tc_findItemsUsingRegexp("^p[\d+]$")
    if mrna_items.length == 0:
        mrna_items = tc_findItemsUsingRegexp("\\.mrna$")
    protein_names = fromTC( tc_getUniqueNames( protein_items ))
    mrna_names = fromTC( tc_getUniqueNames( mrna_items ))

#get indices from the names
    protein_indx = range(0,len(protein_names))
    mrna_indx = range(0,len(mrna_names))

    for i in range(0,len(protein_names)):
        protein_indx[i] = tc_getColumnIndex(simdata, protein_names[i])
        
    for i in range(0,len(mrna_names)):
        mrna_indx[i] = tc_getColumnIndex(simdata, mrna_names[i])

#print all mrna
    s = "Time\t" + "\t".join(mrna_names) + "\n"
    shalf = "Time\t" + "\t".join(mrna_names) + "\n"
    for i in range(0,simdata.rows):
        if (i % 2) == 0: s += str(tc_getMatrixValue(simdata, i, 0))
        if (i % 4) == 0: shalf += str(tc_getMatrixValue(simdata, i, 0))
        for j in mrna_indx:
            v  = tc_getMatrixValue(simdata, i, j)
            if addnoise:
                if v > 0: v += numpy.random.normal(0,v/5.0)
                v += numpy.random.normal(0,0.1)
            if v < 0: v = 0
            if (i % 2) == 0: s += "\t" + str(round(v,3))
            if (i % 4) == 0: shalf += "\t" + str(round(v,3))
        if (i % 2) == 0: s += "\n"
        if (i % 4) == 0: shalf += "\n"

    home = tc_homeDir()
    os.chdir(home)
    try:
        os.mkdir("perturbations")
#        os.chdir("perturbations")
#        os.mkdir("mRNA")
#        os.mkdir("protein")
#        os.chdir("mRNA")
#        os.mkdir("lowres")
#        os.mkdir("hires")
    except Exception,err:
        pass
    filename = home + "/perturbations/array_" + prefix + "_low.tab"
    FILE = open(filename,"w")
    FILE.write(shalf)
    FILE.close()

    filename = home + "/perturbations/array_" + prefix + "_high.tab"
    FILE = open(filename,"w")
    FILE.write(s)
    FILE.close()

#print all N choose 2 combinations of protein data
    for k1 in range(0, len(protein_indx)-1):
        for k2 in range(k1+1, len(protein_indx)):
            s = "Time\t" + protein_names[k1] + "\t" + protein_names[k2] + "\n"
            for i in range(0, simdata.rows):
                j1 = protein_indx[k1]
                j2 = protein_indx[k2]
                s += str(tc_getMatrixValue(simdata, i, 0))
                v  = tc_getMatrixValue(simdata, i, j1)
                if addnoise:
                    if v > 0: v += numpy.random.normal(0,v/5.0)
                    v += numpy.random.normal(0,0.1)
                if v < 0: v = 0
                s += "\t" + str(round(v,3))
                v  = tc_getMatrixValue(simdata, i, j2)
                if addnoise:
                    if v > 0: v += numpy.random.normal(0,v/5.0)
                    v += numpy.random.normal(0,0.1)
                if v < 0: v = 0
                s += "\t" + str(round(v,3))
                s += "\n"
            
            if protein_names[k1] < protein_names[k2]: 
                filename = home + "/perturbations/" +  protein_names[k1] + "_" + protein_names[k2] + "_" + prefix + ".tab"
            else:
                filename = home + "/perturbations/" +  protein_names[k2] + "_" + protein_names[k1] + "_" + prefix + ".tab"
            FILE = open(filename,"w")
            FILE.write(s)
            FILE.close()

def PerturbationExpr(family, factor, endtime, datagen, modelnum, addnoise, reorder):
    #assignment rules reordering bug fix
    if reorder == "NO":
        tc_enableAssignmentRulesReordering(0)
    else:
        tc_enableAssignmentRulesReordering(1)

    #get just relevant parameters
    parts_c = tc_itemsOfFamily(family)
    params = tc_createMatrix(0,0)

    if (parts_c.length == 0):  #when using Antimony scripts, there is no ontology
        if (family == "Promoter"):    params = tc_getNumericalValueUsingRegexp("^pro[\d+]_strength$")
        if (family == "mRNA"):    params = tc_getNumericalValueUsingRegex("mrna_degradation_rate$")
        if (family == "RBS"):    params = tc_getNumericalValueUsingRegex("^rbs[\d+]_strength$")
    else:
        params = tc_getParameters(parts_c)

    #perturbation
    tc_multiplot(3, int(params.rows/3 + 1))
    for n in range(0, params.rows):
        tc_showProgress("Perturbation Experiments", int(100 * n / params.rows))
        x = tc_getMatrixValue( params , n , 0)
        tc_setMatrixValue(params, n, 0, x * factor)
        tc_updateParameters(params) #temporary update (faster than setParameters)

        name = tc_getRowName(params,n)
        name = name.replace("_strength","")
        name = name.replace("_degradation_rate","")

        dat = tc_simulateDeterministic(0,endtime,2*endtime)
        tc_plot(dat, name + " changed")
        time.sleep(2)
        tc_setMatrixValue(params, n, 0, x)  #revert

        if datagen == "YES":
            s = name
            if factor == 0: s = "mod_" + str(modelnum) + "_del_" + name
            if factor > 0: 
                if family == "RBS": s = "mod_" + str(modelnum) + "_over_" + name
                if family == "mRNA": s = "mod_" + str(modelnum) + "_dwn_" + name
            if factor < 0: 
                if family == "RBS": s = "mod_" + str(modelnum) + "_dwn_" + name
                if family == "mRNA": s = "mod_" + str(modelnum) + "_over_" + name
            generatefiles(dat, s, addnoise == "YES")  #generate test data
    tc_updateParameters(params)
    dat = tc_simulateDeterministic(0,endtime,2*endtime)
    tc_plot(dat, "no perturbation")
    if datagen == "YES":
        s = "mod_" + str(modelnum) + "_wildtype"
        generatefiles(dat,s, addnoise == "YES")  #generate test data
    tc_updateParameters(params)
    tc_showProgress("Perturbation Experiments", int(100))
    tc_openUrl(tc_homeDir() + "/perturbations")

#GUI
input = tc_createMatrix(7,1)
tc_setRowName(input, 0, "What to perturb")
tc_setRowName(input, 1, "Perturb by a factor of")
tc_setRowName(input, 2, "Simulation time")
tc_setRowName(input, 3, "Write data to files")
tc_setRowName(input, 4, "Model number (for labeling files)")
tc_setRowName(input, 5, "Add noise (output files only)")
tc_setRowName(input, 6, "Order formulas correctly?")
tc_setMatrixValue(input, 2, 0, 20.0)
tc_setMatrixValue(input, 3, 0, 0.0)
tc_setMatrixValue(input, 4, 0, 1.0)
tc_setMatrixValue(input, 5, 0, 0.0)
tc_setMatrixValue(input, 6, 0, 0.0)
tc_createInputWindowForScript(input, "Perturbation experiments", "PerturbationExpr")
tc_addInputWindowOptions("Perturbation experiments", 0, 0, toTC(["Promoter","RBS","mRNA"]))
tc_addInputWindowOptions("Perturbation experiments", 3, 0, toTC(["YES", "NO"]))
tc_addInputWindowOptions("Perturbation experiments", 5, 0, toTC(["YES", "NO"]))
tc_addInputWindowOptions("Perturbation experiments", 6, 0, toTC(["YES", "NO"]))

