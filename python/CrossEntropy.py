from numpy import *
from tinkercell import *
from tc2py import *
import numpy.random
import numpy.linalg

#Takes an objective function along with an intial guess of the distribution of parameters and returns the final
#best fit distribution of parameters. Assumes that the distributions are Gaussian.
def OptimizeParameters(objective, title="optimizing", maxits=200, N=100, Ne=0.5, logscale=False,epsilon = 1e-5):
    t = 0
    if Ne >= 1 or Ne <= 0:
        Ne = 0.5
    Ne = Ne * N
    S = range(0,N)
    
    oldmax = 0
    curmax = 0
    allparams = tc_getParameters( tc_allItems() )
    n = 0;
    for i in range(0,allparams.rows):
        if tc_getMatrixValue(allparams, i, 2) != tc_getMatrixValue(allparams, i, 1): 
            n += 1
    if n < 1:
        return tc_createMatrix(0,0)

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
    while t < maxits and (t<2 or (oldmax - curmax) > epsilon):     #While not converged and maxits not exceeded
        tc_showProgress(title, int( 100 * t/maxits ))
        X = numpy.random.multivariate_random(mu,sigma2,N)         #Obtain N samples from current sampling distribution
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
        oldmax = curmax
        curmax = max(S)
        indx.sort(lambda x,y: int(S[x] - S[y]))
        X = X[indx]                     #Sort X by objective function values
        X1 = matrix(X[0:Ne])      #select top
        X2 = X1.transpose()
        for i in range(0,n):
            mu[i] = mean(X2[i])      #Update mean of sampling distribution
        sigma2 = cov(X2)               #Update variance of sampling distribution
        t = t+1;                              #Increment iteration counter
    tc_setParameters(params,1)
    tc_showProgress(title, 100)
    return (mu, sigma2,paramnames)               #Return mean and covariance
