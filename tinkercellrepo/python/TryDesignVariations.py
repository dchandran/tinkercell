"""
category: Model Variations
name: Remove Regulations one-by-one
descr: Try removing each individual regulation one at a time and see how that affects the model
menu: yes
icon: module.png
tool: no
"""
from tinkercell import *

def runAll(indep, start, end, nclusters=0):
    items = tc_allItems()
    listOfModules = []
    moduleNames = []
    for i in range(0, items.length):
        item = tc_getItem(items, i)
        if tc_getParent(item) == 0:
            submodels = tc_listOfPossibleModels(item)
            if submodels.length > 0:
                listOfModules.append(item)
                moduleNames.append(tc_getUniqueName(item))
    n = len(listOfModules)
    if n > 0:
        tc_multiplot(2, int(1+n/2))
        tc_showProgress("Explore design variations", 0)
        for i in range(0,n):
            tc_substituteEmptyModel(listOfModules[i])
            if indep.lower() == "time":
                m = tc_simulateDeterministic(start,end,100)
                tc_plot(m, moduleNames[i] + " removed")
            else:
                m = tc_steadyStateScan(indep, start,end,10)
                tc_plot(m, moduleNames[i] + " removed")
            tc_substituteOriginalModel(listOfModules[i])
            tc_showProgress("Explore design variations", int(100*(i+1)/n))

        tc_showProgress("Explore design variations", 100)
        if nclusters > 1:
            m = tc_clusterPlots(nclusters)
            fout = open("designs.txt","w")
            for i in range(0, nclusters):
                s = "========\nCluster " + str(i+1) + "\n========\n"
                for j in range(0, n):
                    if tc_getMatrixValue(m, j, 0) == i+1:
                        s += " ,  " + moduleNames[j]
                        s += "\n"
                    s += "\n"
                fout.write(s)
            fout.close()
            tc_openUrl("designs.txt")
    else:
        if indep.lower() == "time":
            m = tc_simulateDeterministic(start,end,100)
            tc_plot(m, "steady state scan")
        else:
            m = tc_steadyStateScan(indep, start,end,10)
            tc_plot(m, "simulation")
    return

params = tc_getParametersAndFixedVariables(tc_allItems())
list = fromTC(params.rownames)
list.insert(0,"Time")
inputWindow = tc_createMatrix(3, 1)
tc_setMatrixValue(inputWindow, 0, 0, 0)
tc_setMatrixValue(inputWindow, 1, 0, 0.0)
tc_setMatrixValue(inputWindow, 2, 0, 200.0)
tc_setMatrixValue(inputWindow, 3, 0, 4)
tc_setRowName(inputWindow, 0, "Independent variable")
tc_setRowName(inputWindow, 1, "Start")
tc_setRowName(inputWindow, 2, "Stop")
tc_setRowName(inputWindow, 3, "Clusters")
tc_createInputWindowForScript(inputWindow, "Explore design variations", "runAll")
tc_addInputWindowOptions("Explore design variations", 0, 0, toTC(list))

