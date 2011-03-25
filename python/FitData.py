"""
category: Optimization
name: Fit time-series data
description: fit model parameters to time-series data
icon: curve.png
menu: yes
tool: no
"""
from tinkercell import *
from tc2py import *
from numpy import *
from CrossEntropy import *

fname = tc_getFilename()
indx = []
headers = []
time = 0
numpts = 0
params = tc_createMatrix(0,0)
trueData = array([])

def leastSquares(x):
    for i in range(0,len(x)):
        tc_setMatrixValue(params, i, 0, x[i])
    tc_setParameters(params)
    sim = tc_simulate(0,time,numpts)
    if len(indx) == 0:
        indx = range(0,sim.cols)
        for i in range(0,sim.cols):
            indx[i] = -1
            for j in range(0,len(headers)):
                if tc_getColumnName(sim, i) == headers[j]:
                    indx[i] = j
    total = 0
    n = 0
    for i in range(0,sim.cols):
        if indx[i] > -1:
            n += 1
            k = indx[i]
            for j in range(0,sim.rows):
                total += (dat[j,k] - tc_getMatrixValue(sim, j, i))**2
    total /= (n*sim.rows)
    return total

def fcallback(t, mu, sigma2):
    tc_showProgress("Fitting data",int(t/2))

if len(fname) > 0:
    tfile = open(fname, "r")
    line  = tfile.readline()
    line = line.replace('\n','')
    line = line.replace('#','')
    tfile.close()
    headers = line.split(',')
    trueData = genfromtxt(fname, type(0.0), comments='#', delimiter=',')
    numpts = len(dat) #rows
    time = dat[ numpts-1, 0 ]  #last time point
    allparams  = tc_getParameters(tc_allItems())
    n = 0
    for i in range(0,allparams.rows):
        if tc_getMatrixValue(allparams, i, 1) != tc_getMatrixValue(allparams, i, 2):
            n += 1
    params = tc_createMatrix(n,1)
    mu = range(0,n)
    sigma2 = range(0,n)
    j = 0
    for i in range(0,n):
        if tc_getMatrixValue(allparams, i, 1) != tc_getMatrixValue(allparams, i, 2):
            d = tc_getMatrixValue(allparams, i, 0)
            tc_setMatrixValue(params, j, 0, d)
            mu[j] = d
            sigma2[j] = ((tc_getMatrixValue(allparams, i, 2) - tc_getMatrixValue(allparams, i, 1))/3.0)**2
            j += 1
    res = CrossEntropy(leastSquares, mu, diag(sigma2), 200, 100, 0.5, fcallback)
    mu = res[0]
    sigma2 = res[1]
    X = random.multivariate_normal(mu,sigma2,100)
    n = len(mu)
    m = tc_createMatrix(100, n)
    for i in range(0,100):
        for j in range(0,n):
            tc_setMatrixValue(i,j, X[i][j])
    for i in range(0,n):
        tc_setMatrixValue(params, i, 0, mu[i])
        tc_setColumnName(m, i, tc_getRowName(params,i))
    tc_setParameters(params)
    tc_scatterplot(m, "Optimized parameters")

