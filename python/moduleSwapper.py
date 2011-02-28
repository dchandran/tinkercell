"""
category: Simulation
name: Module Swap
descr: A convenient way to see the simulation result due to the affect of different submodels
menu: yes
icon: module.png
tool: no
"""
from tinkercell import *

listOfModules = []
allItems = tc_allItems()
homeDir = tc_homeDir() + "/Modules/"
hsz = len(homeDir)

for i in range(0, allItems.length):
    item = tc_getItem(allItems, i)
    submodels = tc_listOfPossibleModels(item)
    if submodels.length > 0 and tc_getParent(item) == 0:
        listOfModules.append(item)

inputWindow = tc_createMatrix( len(listOfModules), 1 )
for i in range( 0, len(listOfModules) ):
    tc_setRowName(inputWindow, i, tc_getUniqueName(listOfModules[i]))

tc_createInputWindowForScript(inputWindow, "Submodel Swapper", "substituteSubModel")

for i in range( 0, len(listOfModules) ):
    options = tc_listOfPossibleModels( listOfModules[i] )
    for j in range(0, options.length):
        s = tc_getString(options, j)
        if s[0:hsz] == homeDir:
            n = len(s)
            s = s[hsz:n]
            s = s.replace("_"," ")
            s = s.replace("/",": ")
            tc_setString(options, j, s)
    tc_addInputWindowOptions("Submodel Swapper", i, 0, options)

def substituteSubModel(*arg):
    global listOfModules
    homeDir = tc_homeDir() + "/Modules/"
    n = len(listOfModules)
    if len(arg) < n:
        n = len(arg)
    for i in range(0,n):
        s = arg[i]
        s = s.replace(": ","/")
        s = s.replace(" ","_")
        s = homeDir + s
        tc_substituteModel(listOfModules[i], s)

    m = tc_simulateDeterministic(0,100,200)
    tc_plot(m, "simulation")

