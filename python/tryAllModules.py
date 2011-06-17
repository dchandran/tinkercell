"""
category: Simulation
name: Hypothesis generator
descr: Try all possible models for the given diagram and see possible simulation results
menu: yes
icon: module.png
tool: no
"""
from tinkercell import *

def runAllHelper( listOfModules, n , indep, start, end, filenames, allfilenames):
    if n >= len(listOfModules):
        if indep.lower() == "time":
            m = tc_simulateDeterministic(start,end,100)
            tc_plot(m, "")
        else:
            m = tc_steadyStateScan(indep, start,end,10)
            tc_plot(m, "")
        allfilenames.append(filenames)
        return allfilenames
    item = listOfModules[n]
    submodels = tc_listOfPossibleModels(item)
    home = tc_homeDir()
    for i in range(0, submodels.length):
        s = tc_getString(submodels, i)
        tc_substituteModel(listOfModules[n], s)
        s.replace(home,"")
        filenames2 = filenames
        filenames2.append(s)
        allfilenames = runAllHelper(listOfModules, n+1, indep, start, end, filenames2, allfilenames)
    return allfilenames

def runAll(indep, start, end, nclusters):
    items = tc_allItems()
    listOfModules = []
    
    tc_holdPlot(1)
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
            allfilenames = runAllHelper(listOfModules, 0, indep, start, end)
            m = tc_clusterPlots( nclusters )
            fout = open("modules.txt","w")
            for i in range(0, nclusters):
                s = "========\nCluster " + str(i+1) + "\n========\n"
                for j in range(0,len(allfilenames)):
                    if tc_getMatrixValue(m, j, 0) == i+1:
                        s += allfilenames[j].join(" , ")
                        s += "\n"
                s += "\n"
                fout.write(s)
            fout.close()
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
params = tc_getParameters(tc_allItems())
list = fromTC(params.rownames)
list.insert(0,"Time")

tc_createInputWindowForScript(inputWindow, "Explore possibile models", "runAll")
tc_addInputWindowOptions("Explore possibile models", 0, 0, toTC(list))

