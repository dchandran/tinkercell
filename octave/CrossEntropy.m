#Takes an objective function along with an intial guess of the distribution of parameters and returns the final
#best fit distribution of parameters. Assumes that the distributions are Gaussian.
function [mu, sigma2,paramnames] = OptimizeParameters(objective, title="optimizing", maxits=200, N=100, minimize=1, Ne=0.5, logscale=0,epsilon = 1e-5):
    t = 0;
    if (Ne >= 1 || Ne <= 0)
        Ne = 0.5;
    end
    Ne = Ne * N;
    S = zeros(N);
    lasterr = 0;
    oldmax = 0;
    curmax = 0;
    allparams = tc_getParameters( tc_allItems() );
    n = 0;
    for i = 0:(allparams.rows-1)
        if (tc_getMatrixValue(allparams, i, 2) != tc_getMatrixValue(allparams, i, 1)) 
            n += 1
        end
    end
    if (n < 1)
        n = allparams.rows;
        for i = 0:(allparams.rows-1):
            x = tc_getMatrixValue(allparams, i, 0);
            tc_setMatrixValue(allparams, i, 1, x/10.0);
            if (x > 0)
                tc_setMatrixValue(allparams, i, 2, x*10.0);
            else
                tc_setMatrixValue(allparams, i, 2, 1.0);
            end
        end
   end
    params = tc_createMatrix(n,3);
    minmax = zeros(n);
    mu = zeros(n);
    j = 0;
    for i = 0:(allparams.rows-1)
        if (tc_getMatrixValue(allparams, i, 2) != tc_getMatrixValue(allparams, i, 1))
            tc_setMatrixValue(params, j, 0, tc_getMatrixValue(allparams, i, 0));
            tc_setMatrixValue(params, j, 1, tc_getMatrixValue(allparams, i, 1));
            tc_setMatrixValue(params, j, 2, tc_getMatrixValue(allparams, i, 2));
            tc_setRowName(params,j, tc_getRowName(allparams, i));
            mu(j+1) = tc_getMatrixValue(params, i, 0);
            minmax(j+1) = ((tc_getMatrixValue(allparams, i, 2) - tc_getMatrixValue(allparams, i, 1))/3.0).^2;
            j += 1;
        end
    end
    paramnames = fromTC(params.rownames);
    sigma2 = diag(minmax);
    tc_showProgress(title, int(0));
    while (t < maxits && (t<2 || (oldmax - curmax) > epsilon))     #While not converged and maxits not exceeded
        X = mvnrnd(mu,sigma2,N);         #Obtain N samples from current sampling distribution
        indx = 1:N;
        for i = indx
            for j = 1:params.rows
                d0 = X(i,j)
                if (logscale)
                    d0 = exp(X(i,j));
                end
                d1 = tc_getMatrixValue(params, j-1, 1);
                d2 = tc_getMatrixValue(params, j-1, 2);
                if (d0 < d1)
                    d0 = d1;
                end
                if (d0 > d2)
                    d0 = d2;
                end
                if (logscale)
                    X(i,j) = log(d0);
                else
                    X(i,j) = d0;
                end
                tc_setMatrixValue(params, j-1, 0, d0);
            end
            tc_updateParameters(params);
            S(i) = objective();
        end
        t = t+1;                              #Increment iteration counter
        oldmax = curmax;
        curmax = max(S);
        t1 = 100 * t / maxits;
        t2 = 100 * epsilon/(oldmax - curmax);
        if (t2 > lasterr)
            lasterr = t2;
        end
        if (t < 2 || t1 > lasterr)
            tc_showProgress(title, int(t1));
        else
            tc_showProgress(title, int(lasterr));
        end
        if (minimize)
            [S2, indx] = sort(S);
        else
            [S2, indx] = sort(-S);
        end
        X = X(indx);                     #Sort X by objective function values
        X1 = matrix(X(1:Ne));      #select top
        X2 = X1';
        for i = 1:n:
            mu(i) = mean(X2(i))      #Update mean of sampling distribution
        end
        sigma2 = cov(X2);               #Update variance of sampling distribution
    end
    tc_showProgress(title, 100);
end

function [e,v] = DoPCA(mu, sigma2, paramnames):
    [e,v] = eig(sigma2);
    props = 100.0 * e./sum(e);
    fout = fopen ("crossentropy.out","w");
    fprintf(fout, "===============================================\n");
    fprintf(fout,  "Optimized parameters (mean and st.dev)\n");
    fprintf(fout, "==============================================\n\nnames: ");
	for i = 1:len(mu)
        fprintf(fout, "    %s", paramnames(i));
    end
    fprintf(fout, "\nmean:  ");
    for i = 1:len(mu)
        fprintf(fout, "    %s", str(mu(i)));
    end
    fprintf(fout, "\nst.dev:");
    for i = 1:len(mu)
        fprintf(fout, "    %s", sqrt(sigma2(i,i)));
    end
    fprintf(fout, "\n");
    for i = 1:len(mu)
        fprintf(fout, "    %s", str(mu(i)));
    end
    fprintf(fout,  "\n============================================\n");
    fprintf(fout, "Global sensitivity (assuming normality)\nOrdered from least to most sensitive\n");
    fprintf(fout, "==============================================\n\n");
    for i = 1:size(e,1):
	    fprintf(fout, "%lf percent of the variability can be attributed to the following linear combination:\n", props(i));
	    for j = 1:size(paramnames,1)
	        if (j > 1)
	            fprintf(fout, "\n %lf * %s", v(i,j), paramnames(j));
	        else
	            fprintf(fout, "%lf * %s", v(i,j), paramnames(j));
            end
	    fprintf(fout, "\n\n");
    end
    fclose(fout);
    tc_openUrl("crossentropy.out");
end