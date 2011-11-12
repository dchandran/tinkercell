"""
category: Model Variations
name: Automatic Model Generator
descr: Try all possible models for the given diagram and see possible simulation results
menu: yes
icon: module.png
tool: no
"""
from tinkercell import *

def runAllHelper( total, listOfModules, n , indep, start, end, filenames=[], allfilenames=[]):
    if n >= len(listOfModules):
        if indep.lower() == "time":
            m = tc_simulateDeterministic(start,end,100)
            tc_plot(m, str(len(allfilenames)))
        else:
            m = tc_steadyStateScan(indep, start,end,10)
            tc_plot(m, str(len(allfilenames)))
        L = filenames[:]
        allfilenames.append(L)
        tc_showProgress("Try Model Variations", int(100 * len(allfilenames)/total))
        return allfilenames
    item = listOfModules[n]
    submodels = tc_listOfPossibleModels(item)
    home = tc_homeDir()
    while len(filenames) < len(listOfModules):
         filenames.append("")
    for i in range(0, submodels.length):
        s = tc_getString(submodels, i)
        tc_substituteModel(listOfModules[n], s)
        filenames[n] = s.replace(home,"")
        allfilenames = runAllHelper(total, listOfModules, n+1, indep, start, end, filenames, allfilenames)
    return allfilenames

def runAll(indep, start, end, nclusters):
    items = tc_allItems()
    listOfModules = []

    if (nclusters > 1):   
        tc_holdPlot(1)
    else:
        tc_multiplot(int(nclusters/4), 4)
    total = 1
    for i in range(0, items.length):
        item = tc_getItem(items, i)
        if tc_getParent(item) == 0:
            submodels = tc_listOfPossibleModels(item)
            if submodels.length > 0:
                listOfModules.append(item)
                total *= submodels.length
    n = len(listOfModules)
    if n > 0:
        if tc_askQuestion(str(n) + " submodels and " + str(total) + " possible models ... continue?") > 0:
            tc_showProgress("Try Model Variations", 0)
            allfilenames = runAllHelper(total, listOfModules, 0, indep, start, end)
            fout = open("modules.txt","w")
            if (nclusters > 1):
                m = tc_clusterPlots( nclusters )
                for i in range(0, nclusters):
                    s = "========\nCluster " + str(i+1) + "\n========\n"
                    for j in range(0,len(allfilenames)):
                        if tc_getMatrixValue(m, j, 0) == i+1:
                            s += " ; ".join(allfilenames[j])
                            s += "\n"
                    s += "\n"
                    fout.write(s)
            else:
                for j in range(0,len(allfilenames)):
                    s = "========\nPlot " + str(j+1) + "\n========\n"
                    s += "\n".join(allfilenames[j])
                    s += "\n\n"
                    fout.write(s)
            fout.close()
            tc_showProgress("Try Model Variations", 100)
            tc_openUrl("modules.txt")
    else:
        if indep.lower() == "time":
            m = tc_simulateDeterministic(start,end,100)
            tc_plot(m, "steady state scan")
        else:
            m = tc_steadyStateScan(indep, start,end,10)
            tc_plot(m, "simulation")
    return

inputWindow = tc_createMatrix( 4, 1 )
tc_setMatrixValue(inputWindow, 0, 0, 0)
tc_setMatrixValue(inputWindow, 1, 0, 0.0)
tc_setMatrixValue(inputWindow, 2, 0, 200.0)
tc_setMatrixValue(inputWindow, 3, 0, 4)
tc_setRowName(inputWindow, 0, "Independent variable")
tc_setRowName(inputWindow, 1, "Start")
tc_setRowName(inputWindow, 2, "Stop")
tc_setRowName(inputWindow, 3, "Clusters")
allitems = tc_allItems()
params = tc_getParameters(allitems)
iv = tc_getInitialValues(allitems)
list = fromTC(params.rownames)
list2 = fromTC(iv.rownames)
for s in list2:
    list.append(s)
list.insert(0,"Time")

tc_createInputWindowForScript(inputWindow, "Explore possibile models", "runAll")
tc_addInputWindowOptions("Explore possibile models", 0, 0, toTC(list))

