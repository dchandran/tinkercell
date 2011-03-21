from numpy import *

def cross_entropy(objective, mu, sigma2, maxits=100, N=100, Ne=10)
    t:=0
    while t < maxits and sum(diag(sigma2)) > epsilon:     #While not converged and maxits not exceeded
        X = random.multivariate_normal(mu,sigma2,N);         #Obtain N samples from current sampling distribution
        S = 
        S = exp(-(X-2)^2) + 0.8 exp(-(X+2)^2);   #Evaluate objective function at sampled points
        X = sort(X,S);                           #Sort X by objective function values (in descending order)
        mu = mean(X(1:Ne)); sigma2=var(X(1:Ne));  #Update parameters of sampling distribution
        t = t+1;                                 #Increment iteration counter
    return (mu, sigma2) #Return mean and covariance

