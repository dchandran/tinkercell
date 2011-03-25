from numpy import *
#Takes an objective function along with an intial guess of the distribution of parameters and returns the final
#best fit distribution of parameters. Assumes that the distributions are Gaussian.
def CrossEntropy(objective, mu, sigma2, maxits=200, N=100, Ne=0.5, callback = None):
    t = 0
    if Ne >= 1 or Ne <= 0:
        Ne = 0.5
    Ne = Ne * N
    S = range(0,N)
    n = len(mu)
    epsilon = 1e-5
    oldmax = 0
    curmax = 0
    while t < maxits and (t<2 or (oldmax - curmax) > epsilon):     #While not converged and maxits not exceeded
        X = random.multivariate_normal(mu,sigma2,N)         #Obtain N samples from current sampling distribution
        indx = range(0,N)
        for i in indx:
            S[i] = objective(X[i])
        oldmax = curmax
        curmax = max(S)
        indx.sort(lambda x,y: int(S[x] - S[y]))
        X = X[indx]                     #Sort X by objective function values
        X1 = matrix(X[0:Ne])   #select top
        X2 = X1.transpose()
        for i in range(0,n):
            mu[i] = mean(X2[i])      #Update mean of sampling distribution
        sigma2 = cov(X2)             #Update variance of sampling distribution
        if not (callback is None):
            callback(t, mu, sigma2)
        t = t+1;                                 #Increment iteration counter
    return (mu, sigma2) #Return mean and covariance

