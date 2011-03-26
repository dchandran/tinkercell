from numpy import *
from tinkercell import *

#Takes an objective function along with an intial guess of the distribution of parameters and returns the final
#best fit distribution of parameters. Assumes that the distributions are Gaussian.
def CrossEntropy(objective, title="optimizing", maxits=200, N=100, Ne=0.5, logscale=False):
    t = 0
    if Ne >= 1 or Ne <= 0:
        Ne = 0.5
    Ne = Ne * N
    S = range(0,N)
    epsilon = 1e-5
    oldmax = 0
    curmax = 0
    params = tc_getParameters( tc_allItems() )
    n = params.rows
    minmax = range(0,n)
    for i in range(0,n)
        mu[i] = tc_getMatrixValue(params, i, 0)
        minmax[i] = ((tc_getMatrixValue(params, i, 2) - tc_getMatrixValue(params, i, 1))/3.0)**2
    sigma2 = diag(minmax)

    while t < maxits and (t<2 or (oldmax - curmax) > epsilon):     #While not converged and maxits not exceeded
        X = random.multivariate_normal(mu,sigma2,N)         #Obtain N samples from current sampling distribution
        indx = range(0,N)
        for i in indx:
            for j in range(0,params.rows):
                d = 0.0
                if logscale:
                    d = X[i,j]
                    if d < tc_getMatrixValue(params,j,1):
                        d = tc_getMatrixValue(params,j,1)
                        X[i,j] = d
                    if d > tc_getMatrixValue(params,j,2):
                        d = tc_getMatrixValue(params,j,2)
                        X[i,j] = d
                else:
                    d = exp(X[i,j])
                    if d < tc_getMatrixValue(params,j,1):
                        d = tc_getMatrixValue(params,j,1)
                        X[i,j] = log(d)
                    if d > tc_getMatrixValue(params,j,2):
                        d = tc_getMatrixValue(params,j,2)
                        X[i,j] = log(d)
                tc_setMatrixValue(params, j, 0, d)
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
        tc_showProgress(title, int( 100 * t/maxits ))
        t = t+1;                              #Increment iteration counter
    return (mu, sigma2)               #Return mean and covariance
