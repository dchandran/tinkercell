"""
Not a useful plugin, so removed TinkerCell header
"""
from tinkercell import *

listOfModules = []
allItems = tc_allItems()
homeDir = tc_homeDir() + "/Modules/"
hsz = len(homeDir)
time = tc_getNumber("Simulation time")

for i in range(0, allItems.length):
    item = tc_getItem(allItems, i)
    submodels = tc_listOfPossibleModels(item)
    if submodels.length > 0 and tc_getParent(item) == 0:
        listOfModules.append(item)
        #print tc_getUniqueName(item) + " : " + str(submodels.length)

inputWindow = tc_createMatrix( len(listOfModules), 2 )
tc_setColumnName(inputWindow, 0, "possible models")
tc_setColumnName(inputWindow, 1, "remove from model")
options = toTC(["Keep", "Remove"])
for i in range( 0, inputWindow.rows ):
    tc_setRowName(inputWindow, i, tc_getUniqueName(listOfModules[i]))

tc_createInputWindowForScript(inputWindow, "Submodel Swapper", "substituteSubModel")
for i in range( 0, inputWindow.rows ):
    tc_addInputWindowOptions("Submodel Swapper", i, 1, options)
    
for i in range( 0, len(listOfModules) ):
    options = tc_listOfPossibleModels( listOfModules[i] )
    for j in range(0, options.length):
        s = tc_getString(options, j)
        homeDir = tc_homeDir() + "/Modules/"
        if s.count(homeDir) > 0:
            s = s.replace(homeDir,"")
            s = s.replace("_"," ")
            s = s.replace("/",": ")
            s = s.replace(".tic","")
            tc_setString(options, j, s)
    tc_addInputWindowOptions("Submodel Swapper", i, 0, options)

def substituteSubModel(*arg):
    global listOfModules
    n = len(listOfModules)
    if len(arg) < (2*n):
        n = len(arg)/2
    for i in range(0,n):
        j = 2*i
        s = ""
        if arg[j+1] == "Keep":
            s = arg[j]
            s = s.replace(": ","/")
            s = s.replace(" ","_")
            if s.count(".tic") < 1:
                s = s + ".tic"
        tc_substituteModel(listOfModules[i], s)

    m = tc_simulateDeterministic(0,time,100)
    tc_plot(m, "simulation")

