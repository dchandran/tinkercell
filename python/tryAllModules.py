"""
category: Simulation
name: Hypothesis generator
descr: Try all possible models for the given diagram and see possible simulation results
menu: yes
icon: module.png
tool: no
"""
from tinkercell import *

def runAllHelper( listOfModules, n , time):
    if n >= len(listOfModules):
        m = tc_simulateDeterministic(0,time,100)
        tc_plot(m, "")
        return
    item = listOfModules[n]
    submodels = tc_listOfPossibleModels(item)
    for i in range(0, submodels.length):
        s = tc_getString(submodels, i)
        tc_substituteModel(listOfModules[n], s)
        runAllHelper(listOfModules, n+1, time)
    return

def runAll(*arg):
    items = tc_allItems()
    listOfModules = []
    time = arg[2]
    #tc_enableClustering(4)
    total = 0
    for i in range(0, items.length):
        item = tc_getItem(items, i)
        if tc_getParent(item) == 0:
            submodels = tc_listOfPossibleModels(item)
            if submodels.length > 0:
                listOfModules.append(item)
                total += submodels.length
    n = len(listOfModules)
    if n > 0:
        if tc_askQuestion(str(n) + " submodels and " + str(total) + " possible models ... continue?") > 0:
            runAllHelper(listOfModules, 0, time)
    else:
        m = tc_simulateDeterministic(0,time,100)
        tc_plot(m, "simulation")
    return

inputWindow = tc_createMatrix( 3, 1 )
tc_setMatrixValue(inputWindow, 0, 0, 0)
tc_setMatrixValue(inputWindow, 1, 0, 0.0)
tc_setMatrixValue(inputWindow, 2, 0, 200.0)
tc_setRowName(inputWindow, 0, "Independent variable")
tc_setRowName(inputWindow, 1, "Start")
tc_setRowName(inputWindow, 2, "Stop")
params = tc_getParameters(tc_allItems())
list = fromTC(params.rownames)
list.insert(0,"Time")

tc_createInputWindowForScript(inputWindow, "Explore possibile models", "runAll")
tc_addInputWindowOptions("Explore possibile models", 0, 0, toTC(list))
