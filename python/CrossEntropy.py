from numpy import *
from tinkercell import *
from tc2py import *
import numpy.random
import numpy.linalg

#Takes an objective function along with an intial guess of the distribution of parameters and returns the final
#best fit distribution of parameters. Assumes that the distributions are Gaussian.
def OptimizeParameters(objective, title="optimizing", maxits=200, N=100, minimize=True, Ne=0.5, logscale=False,epsilon = 1e-5):
    t = 0
    if Ne >= 1 or Ne <= 0:
        Ne = 0.5
    Ne = Ne * N
    S = range(0,N)
    lasterr = 0
    oldmax = 0
    curmax = 0
    allparams = tc_getParameters( tc_allItems() )
    n = 0;
    for i in range(0,allparams.rows):
        if tc_getMatrixValue(allparams, i, 2) != tc_getMatrixValue(allparams, i, 1): 
            n += 1
    if n < 1:
        n = allparams.rows
        for i in range(0,allparams.rows):
            x = tc_getMatrixValue(allparams, i, 0)
            tc_setMatrixValue(allparams, i, 1, x/10.0)
            if x > 0:
                tc_setMatrixValue(allparams, i, 2, x*10.0)
            else:
                tc_setMatrixValue(allparams, i, 2, 1.0)

    params = tc_createMatrix(n,3)
    minmax = range(0,n)
    mu = range(0,n)
    j = 0
    for i in range(0,allparams.rows):
        if tc_getMatrixValue(allparams, i, 2) != tc_getMatrixValue(allparams, i, 1):
            tc_setMatrixValue(params, j, 0, tc_getMatrixValue(allparams, i, 0))
            tc_setMatrixValue(params, j, 1, tc_getMatrixValue(allparams, i, 1))
            tc_setMatrixValue(params, j, 2, tc_getMatrixValue(allparams, i, 2))
            tc_setRowName(params,j, tc_getRowName(allparams, i))
            mu[j] = tc_getMatrixValue(params, i, 0)
            minmax[j] = ((tc_getMatrixValue(allparams, i, 2) - tc_getMatrixValue(allparams, i, 1))/3.0)**2
            j += 1

    paramnames = fromTC(params.rownames)
    sigma2 = diag(minmax)
    tc_showProgress(title, int(0))
    while t < maxits and (t<2 or (oldmax - curmax) > epsilon):     #While not converged and maxits not exceeded
        X = numpy.random.multivariate_normal(mu,sigma2,N)         #Obtain N samples from current sampling distribution
        indx = range(0,N)
        for i in indx:
            for j in range(0,params.rows):
                d0 = X[i,j]
                if logscale:
                    d0 = exp(X[i,j])
                d1 = tc_getMatrixValue(params, j, 1)
                d2 = tc_getMatrixValue(params, j, 2)
                if d0 < d1:
                    d0 = d1
                if d0 > d2:
                    d0 = d2
                if logscale:
                    X[i,j] = log(d0)
                else:
                    X[i,j] = d0
                tc_setMatrixValue(params, j, 0, d0)                
            tc_updateParameters(params)
            S[i] = objective()
        t = t+1                              #Increment iteration counter
        oldmax = curmax
        curmax = max(S)
        t1 = 100 * t / maxits
        t2 = 100 * epsilon/(oldmax - curmax)
        if t2 > lasterr:
            lasterr = t2
        if t < 2 or t1 > lasterr: 
            tc_showProgress(title, int(t1))
        else:
            tc_showProgress(title, int(lasterr))
        
        if minimize:
            indx.sort(lambda x,y: int(S[x] - S[y]))
        else:
            indx.sort(lambda x,y: int(S[y] - S[x]))
        X = X[indx]                     #Sort X by objective function values
        X1 = matrix(X[0:Ne])      #select top
        X2 = X1.transpose()
        for i in range(0,n):
            mu[i] = mean(X2[i])      #Update mean of sampling distribution
        sigma2 = cov(X2)               #Update variance of sampling distribution
    tc_showProgress(title, 100)
    return (mu, sigma2,paramnames)               #Return mean and covariance

def DoPCA(mu, sigma2, paramnames):
    e,v = numpy.linalg.eig(sigma2)
    props = 100.0 * e/numpy.sum(e)
    fout = open("crossentropy.out","w")
    s = "=====================================\n"
    s += "Optimized parameters (mean)\n"
    s += "=====================================\n\n"
    for i in range(0,len(mu)):
        s += "    " + paramnames[i]
    s += "\n"
    for i in range(0,len(mu)):
        s += "    " + str(mu[i])
    s += "\n\n=====================================\n"
    s += "Global sensitivity (assuming normality)\nOrdered from least to most sensitive\n"
    s += "=====================================\n\n"
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
    tc_openUrl("crossentropy.out")

