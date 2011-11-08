"""
category: Optimization
name: Fit time-series data
description: fit model parameters to time-series data and approximate sensitivities of parameters
icon: graph3.png
menu: yes
tool: no
"""
from tinkercell import *
from tc2py import *
from numpy import *
import PSO
import numpy.random

fname = tc_getFilename()
indx = []
headers = []
time = 0
numpts = 0
trueData = array([])

def FitData_Objective():
    global indx, headers, time, numpts, trueData
    sim = tc_simulateDeterministic(0, time, numpts)
    n = len(headers)
    #identify index mappings
    if n != len(indx):
        indx = range(0, sim.cols)
        for i in range(0, sim.cols):
            indx[i] = -1
            for j in range(0, len(headers)):
                if tc_getColumnName(sim, i) == headers[j]:
                    indx[i] = j
                    break
    #compute error
    total = 0
    for i in range(0,sim.cols):
        j = indx[i]
        if j > -1:
            for k in range(0,sim.rows):
                total += (tc_getMatrixValue(sim,k,i) - trueData[k,j])**2
    total /= (sim.rows * n)
    return sqrt(total)

if len(fname) > 0:
    tfile = open(fname, "r")
    line  = tfile.readline()
    line = line.replace('\n','')
    line = line.replace('#','')
    tfile.close()
    headers = line.split(',')
    trueData = genfromtxt(fname, type(0.0), comments='#', delimiter=',')
    numpts = len(trueData) - 1     #rows
    time = trueData[ numpts, 0 ]  #last time point
    res = ParticleSwarm(FitData_Objective, 1);


