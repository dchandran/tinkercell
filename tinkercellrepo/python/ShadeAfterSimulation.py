"""
category: Animation
name: Shade Simulated Results
description: Shade components of a diagram based on the current plot
icon: eye.png
menu: yes
tool: no
"""

import time

dat = tc_getPlotData(0)

if dat.rows == 0:
    tc_messageDialog("Please perform a simulation before running this code")

itemsToColor = []
maxValues = []

for i in range(1,dat.cols):
    maxVal = tc_getMatrixValue(dat, dat.rows/2, i)    
    for j in range(dat.rows/2,dat.rows):
        if maxVal < tc_getMatrixValue(dat, j, i):
            maxVal = tc_getMatrixValue(dat, j, i)
    maxValues.append(maxVal)
    s = tc_getColumnName(dat, i)
    a = tc_find(s)
    lst = []
    if a != 0:
        lst.append(a)
        connections = tc_getConnectionsWithRole(a, "regulator")
        for k in range(0, connections.length):
            lst.append( tc_getItem(connections,k) )
        connections = tc_getConnectionsWithRole(a, "reactant")
        for k in range(0, connections.length):
            lst.append( tc_getItem(connections,k) )
        connections = tc_getConnectionsWithRole(a, "substrate")
        for k in range(0, connections.length):
            lst.append( tc_getItem(connections,k) )
        connections = tc_getConnectionsWithRole(a, "catalyst")
        for k in range(0, connections.length):
            lst.append( tc_getItem(connections,k) )        
        connections = tc_getConnectionsWithRole(a, "activator")
        for k in range(0, connections.length):
            lst.append( tc_getItem(connections,k) )       
        connections = tc_getConnectionsWithRole(a, "repressor")
        for k in range(0, connections.length):
            lst.append( tc_getItem(connections,k) )
    itemsToColor.append(lst)

def setShadeForTime(x):
    k = 0
    for i in range(0,dat.rows):
        if tc_getMatrixValue(dat, i, 0) >= x:
            k = i
            break
    for j in range(0, dat.cols-1):
        val = tc_getMatrixValue(dat, k, j+1)
        if val > maxValues[j]:
            val = 1.0
        elif maxValues[j] > 0:
            val = 0.1 + (val/maxValues[j]) * (0.9)
        else:
            val = 0.1
        for item in itemsToColor[j]:
            tc_setAlpha( item, val )

#d = tc_getNumber("Set delay (ms) during simulation")
#if d == 0:
#    d = 0.1;
#for i in range(0,dat.rows):
#    tc_showProgress("Animating...", 100 * i/dat.rows)
#    for j in range(0, dat.cols-1):
#        val = tc_getMatrixValue(dat, i, j+1)
#        if maxValues[j] > 0:
#            val = 0.1 + (val/maxValues[j]) * (0.9)
#        else:
#            val = 0.1
#        for item in itemsToColor[j]:
#            tc_setAlpha( item, val )
#    time.sleep(d)

#tc_showProgress("Animating...", 100)
m = tc_createMatrix(1,2)
tc_setRowName(m, 0, tc_getColumnName(dat,0))
tc_setMatrixValue(m, 0, 0, tc_getMatrixValue(dat, 0, 0))
tc_setMatrixValue(m, 0, 1, tc_getMatrixValue(dat, dat.rows-1, 0))

tc_createSlidersForScript(m,"setShadeForTime")

