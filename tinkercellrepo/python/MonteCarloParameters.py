"""
category: Model Variations
name: Randomize Parameters
descr: Run repeated simulations using random parameters and cluster the results
menu: yes
icon: module.png
tool: no
"""
from numpy import *
from tinkercell import *
from tc2py import *
import numpy.random
import numpy.linalg

def __setparams(x, m):
    for i in range(0,len(x)):
        tc_setMatrixValue(m, i, 0, x[i]);
    tc_updateParameters(m);

def __dopca(mu, sigma2, paramnames):
    s = ""
    print sigma2
    e,v = numpy.linalg.eig(sigma2)
    for i in range(0,len(mu)):
        s += "    " + paramnames[i]
    s += "\nmean:  "
    for i in range(0,len(mu)):
        s += "    " + str(float(mu[i]))
    s += "\nst.dev:"
    for i in range(0,len(mu)):
        s += "    " + str(sqrt(sigma2[i,i]))
    s += "\n"    
    s += "\n"
    if (numpy.sum(e) == 0):
        s += "Principal components analysis failed\n\n"
    else:
        props = 100.0 * e/numpy.sum(e)
        for i in range(0,len(e)):
            if i < 1 or props[i] > 0.5:
                s += str(int(props[i])) + "% of the variability can be attributed to the following linear combination:\n"
                for j in range(0,len(paramnames)):
                    if j > 0:
                        s += "\n" + str( round(v[i,j],3) ) + " * " + paramnames[j]
                    else:
                        s += str( round(v[i,j],3) ) + " * " + paramnames[j]
                s += "\n\n"
    return s

def __montecarlo(indep, start, end, numpoints, ncluster):
    if numpoints < 5:
        numpoints = 5
    paramnames = []
    n = 0;
    
    allparams = tc_getParameters( tc_allItems() )
    for i in range(0,allparams.rows):
        if tc_getMatrixValue(allparams, i, 2) != tc_getMatrixValue(allparams, i, 1): 
            n += 1
    if n < 1:
        n = allparams.rows
        for i in range(0,allparams.rows):
            x = tc_getMatrixValue(allparams, i, 0)
            if x > 0:
                tc_setMatrixValue(allparams, i, 1, x/10.0)
                tc_setMatrixValue(allparams, i, 2, x*10.0)
            elif x < 0:
                tc_setMatrixValue(allparams, i, 2, x/10.0)
                tc_setMatrixValue(allparams, i, 1, x*10.0)
            else:
                tc_setMatrixValue(allparams, i, 2, 0.0)
                tc_setMatrixValue(allparams, i, 2, 1.0)

    params = tc_createMatrix(n,3)
    mins = range(0,n)
    maxs = range(0,n)
    mu = range(0,n)
    j = 0
    for i in range(0,allparams.rows):
        if tc_getMatrixValue(allparams, i, 2) != tc_getMatrixValue(allparams, i, 1):
            tc_setMatrixValue(params, j, 0, tc_getMatrixValue(allparams, i, 0))
            tc_setMatrixValue(params, j, 1, tc_getMatrixValue(allparams, i, 1))
            tc_setMatrixValue(params, j, 2, tc_getMatrixValue(allparams, i, 2))
            tc_setRowName(params,j, tc_getRowName(allparams, i))
            mins[j] = tc_getMatrixValue(allparams, i, 1)
            maxs[j] = tc_getMatrixValue(allparams, i, 2)
            j += 1

    numvars = params.rows;
    paramnames = fromTC(params.rownames)
    X = numpy.random.uniform( mins,maxs, [numpoints, numvars]);
    tc_closePlots()
    tc_holdPlot(1)
    for i in range(0,numpoints):
        tc_showProgress("Monte Carlo", int(100 * i/numpoints))

        __setparams(X[i,:], params)
        if indep.lower() == "time":
            m = tc_simulateDeterministic(start, end, 100)
            tc_plot(m, "")
        else:
            m = tc_steadyStateScan(indep, start, end, 100)
            tc_plot(m, "")

    tc_showProgress("Monte Carlo", int(100))
    m = tc_clusterPlots(ncluster)
    fout = open("pca.txt","w")
    s = ""
    for i in range(0,ncluster):
        s += "\n===========\n Cluster " + str(i+1) + "\n===========\n"
        X2 = []
        for j in range(0,m.rows):
            if (tc_getMatrixValue(m, j, 0) == i+1):                
                X2.append(X[j,:])
        if len(X2) > 1:
            X2 = numpy.matrix(X2)
            mu = numpy.arange(0,numvars)
            for j in range(0,numvars):
                mu[j] = numpy.average(X2[:,j])
            sigma2 = numpy.cov(X2.transpose())
            print sigma2.shape
            s += __dopca(mu, sigma2, paramnames)
    fout.write(s)
    fout.close()
    tc_openUrl("pca.txt")


inputWindow = tc_createMatrix( 5, 1 )
tc_setMatrixValue(inputWindow, 0, 0, 0)
tc_setMatrixValue(inputWindow, 1, 0, 0.0)
tc_setMatrixValue(inputWindow, 2, 0, 200.0)
tc_setMatrixValue(inputWindow, 3, 0, 100)
tc_setMatrixValue(inputWindow, 4, 0, 5)
tc_setRowName(inputWindow, 0, "Independent variable")
tc_setRowName(inputWindow, 1, "Start")
tc_setRowName(inputWindow, 2, "Stop")
tc_setRowName(inputWindow, 3, "Sample size")
tc_setRowName(inputWindow, 4, "Clusters")

params = tc_getParameters(tc_allItems())
list = fromTC(params.rownames)
list.insert(0,"Time")

tc_createInputWindowForScript(inputWindow, "Monte Carlo Parameter Effect", "__montecarlo")
tc_addInputWindowOptions("Monte Carlo Parameter Effect", 0, 0, toTC(list))

