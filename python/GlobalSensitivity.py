import numpy
from tinkercell import *
from tc2py import *

def MonteCarloPCA(f, rate = 0.1, maxiter = 10, threshold = 0.1):
    allparams = tc_getParameters( tc_allItems() )
    x0 = numpy.arange(0,allparams.rows)
    y0 = f()
    x = 0.0 * numpy.arange(0,allparams.rows)
    sigma2 = numpy.zeros([allparams.rows, allparams.rows])
    for i in range(0,allparams.rows):
        sigma2[i,i] = rate
        x0[i] = tc_getMatrixValue(allparams, i, 0)

    X1 = numpy.ones([100,1]) * x0;
    X1 = X1 + numpy.random.multivariate_normal(x, sigma2, 100)
    X2 = [];
    for n in range(0,maxiter):
        X1 = X1 + numpy.random.multivariate_normal(x, sigma2, 100)
        for i in range(0,100):
            for j in range(0,allparams.rows):
                tc_setMatrixValue(allparams, j, 0, X1[i,j])
            tc_updateParameters(allparams)
            y1 = f()
            if ( abs((y0-y1)/y0) < threshold ):
                X2.append(X1[i,:])
    mu = x0
    sigma2 = numpy.cov(X2)
    paramnames = fromTC(allparams.rownames)

    X = numpy.random.multivariate_normal(mu,sigma2,100)
    m = toTC( X.transpose().tolist() )
    for i in range(0,len(mu)):
        tc_setColumnName(m, i, paramnames[i])
        k = i #tc_getRowIndex(allparams, paramnames[i])
        if k > -1:
            for j in range(0, 100):
                if tc_getMatrixValue(m, j, i) < tc_getMatrixValue(allparams, k, 1):
                    tc_setMatrixValue(m, j, i, tc_getMatrixValue(allparams, k, 1))
                if tc_getMatrixValue(m, j, i) > tc_getMatrixValue(allparams, k, 2):
                    tc_setMatrixValue(m, j, i, tc_getMatrixValue(allparams, k, 2))
    tc_scatterplot(m, "Parameter distribution")
    e,v = numpy.linalg.eig(sigma2)
    props = 100.0 * e/numpy.sum(e)
    fout = open("pca.txt","w")
    s = "===============================================\n"
    s += "Optimized parameters (mean and st.dev)\n"
    s += "==============================================\n\nnames: "
    for i in range(0,len(mu)):
        s += "    " + paramnames[i]
    s += "\nmean:  "
    for i in range(0,len(mu)):
        s += "    " + str(mu[i])
    s += "\nst.dev:"
    for i in range(0,len(mu)):
        s += "    " + str(sqrt(sigma2[i,i]))
    s += "\n"
    for i in range(0,len(mu)):
        s += "    " + str(mu[i])
    s += "\n============================================\n"
    s += "Global sensitivity (assuming normality)\nOrdered from least to most sensitive\n"
    s += "==============================================\n\n"
    for i in range(0,len(e)):
        s += str(int(props[i])) + "% of the variability can be attributed to the following linear combination:\n"
        for j in range(0,len(paramnames)):
            if j > 0:
                s += "\n" + str( round(v[i,j],3) ) + " * " + paramnames[j]
            else:
                s += str( round(v[i,j],3) ) + " * " + paramnames[j]
        s += "\n\n"
    fout.write(s)
    fout.close()
    tc_openUrl("pca.txt")

