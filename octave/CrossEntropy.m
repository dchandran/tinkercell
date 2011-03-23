%Takes an objective function along with an intial guess of the distribution of parameters and returns the final
%best fit distribution of parameters. Assumes that the distributions are Gaussian.
function [mu sigma2] = CrossEntropy(objective, mu, sigma2, maxits=200, N=100, Ne=0.5)
    t = 0;
    if (Ne >= 1 || Ne <= 0)
        Ne = 0.5;
    end
    Ne = Ne * N;
    S = zeros(N,1);
    n = length(mu);
    epsilon = 1e-5;
    oldmax = 0;
    curmax = 0;
    while (t < maxits) && (t<2 || ((oldmax - curmax) > epsilon))    %While not converged and maxits not exceeded
        X = mvnrnd(mu,sigma2,N);                      %Obtain N samples from current sampling distribution
        for i = 1:N
            S(i) = objective( X(i,:) );
        end
        oldmax = curmax;
        curmax = max(S);
        [S2, indx] = sort(S);
        X1 = X(indx,:);     %Sort X by objective function values
        X2 = X1(1:Ne,:);   %select top
        for i = 1:n
            mu(i) = mean(X2(:,i));      %Update mean of sampling distribution
         end
        sigma2 = cov(X2);             %Update variance of sampling distribution
        t = t+1;                                 %Increment iteration counter
    end
