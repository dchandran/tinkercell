global a b c minimize maxiter threshold logscale numpoints title
a = 0.9
b = 0.1
c = 0.1
minimize = False
maxiter = 30
threshold = 0.1
logscale = False
numpoints = 100
title = "Particle Swarm Optimation" 

#Takes an objective function along with an intial guess of the distribution of parameters and returns the final
#best fit distribution of parameters. Assumes that the distributions are Gaussian.
function g = ParticleSwarm(objective)
    global tinkercell
    t = 0;

function [] = __setparams(x, m, perm=0)
    for i = 0:length(x)
        tinkercell.tc_setMatrixValue(m, i, 0, x[i]);
    endfor
    if (perm)
        tinkercell.tc_setParameters(m,1);
    else
        tinkercell.tc_updateParameters(m);
    endif
endfunction

function [] = __dopca(mu, sigma2, paramnames)
    [e,v] = eig(sigma2);
    props = 100.0 * e./sum(e);
    fout = fopen ("pca.txt","w");
    fprintf(fout, "===============================================\n");
    fprintf(fout,  "Optimized parameters (mean and st.dev)\n");
    fprintf(fout, "==============================================\n\nnames: ");
	for i = 1:len(mu)
        fprintf(fout, "    %s", tinkercell.tc_getString(paramnames,i));
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
    for i = 1:size(e,1)
	    fprintf(fout, "%lf percent of the variability can be attributed to the following linear combination:\n", props(i));
	    for j = 1:paramnames.length
	        if (j > 1)
	            fprintf(fout, "\n %lf * %s", v(i,j), tinkercell.tc_getString(paramnames,j));
	        else
	            fprintf(fout, "%lf * %s", v(i,j), tinkercell.tc_getString(paramnames,j));
            end
	    fprintf(fout, "\n\n");
    end
    fclose(fout);
    tinkercell.tc_openUrl("pca.txt");
end

function ParticleSwarm(f, multirun=1)
    if multirun < 1:
        multirun = 1
    glist = []
    paramnames = []
    n = 0;
    
    allparams = tinkercell.tc_getParameters( tinkercell.tc_allItems() )
    for i in range(0,allparams.rows):
        if tinkercell.tc_getMatrixValue(allparams, i, 2) != tinkercell.tc_getMatrixValue(allparams, i, 1): 
            n += 1
    if n < 1:
        n = allparams.rows
        for i in range(0,allparams.rows):
            x = tinkercell.tc_getMatrixValue(allparams, i, 0)
            if x > 0:
                tinkercell.tc_setMatrixValue(allparams, i, 1, x/10.0)
                tinkercell.tc_setMatrixValue(allparams, i, 2, x*10.0)
            elif x < 0:
                tinkercell.tc_setMatrixValue(allparams, i, 2, x/10.0)
                tinkercell.tc_setMatrixValue(allparams, i, 1, x*10.0)
            else:
                tinkercell.tc_setMatrixValue(allparams, i, 2, 0.0)
                tinkercell.tc_setMatrixValue(allparams, i, 2, 1.0)

    params = tinkercell.tc_createMatrix(n,3)
    mins = range(0,n)
    maxs = range(0,n)
    mu = range(0,n)
    j = 0
    for i in range(0,allparams.rows):
        if tinkercell.tc_getMatrixValue(allparams, i, 2) != tinkercell.tc_getMatrixValue(allparams, i, 1):
            tinkercell.tc_setMatrixValue(params, j, 0, tinkercell.tc_getMatrixValue(allparams, i, 0))
            tinkercell.tc_setMatrixValue(params, j, 1, tinkercell.tc_getMatrixValue(allparams, i, 1))
            tinkercell.tc_setMatrixValue(params, j, 2, tinkercell.tc_getMatrixValue(allparams, i, 2))
            tinkercell.tc_setRowName(params,j, tinkercell.tc_getRowName(allparams, i))
            mins[j] = tinkercell.tc_getMatrixValue(allparams, i, 1)
            maxs[j] = tinkercell.tc_getMatrixValue(allparams, i, 2)
            j += 1

    numpoints = self.numpoints;
    numvars = params.rows;
    paramnames = fromTC(params.rownames)

    maxiter = self.maxiter;
    if maxiter < 1: maxiter = 1
    nmax = multirun * maxiter
    for it in range(0,multirun):
        tinkercell.tc_showProgress(self.title + " round " + str(1+it) + "/" + str(multirun), 0)

        x = numpy.random.uniform(mins,maxs,[numpoints,numvars]);
        p = x;
        v = numpy.random.uniform(0,1,[numpoints,numvars]);
        y = numpy.zeros([numpoints,1]).tolist();
        for i in range(0,numpoints):
            self.__setparams(x[i,:], params)
            y[i] = f();

        g = x[y.index(min(y)),:]
        if not self.minimize:
            g = x[y.index(max(y)),:]

        n = 0
        while n < 1 or ((max(y) - min(y)) > self.threshold and n <= maxiter):
            r1 = numpy.random.uniform(0,1,[numpoints,numvars]);
            r2 = numpy.random.uniform(0,1,[numpoints,numvars]);
            G = numpy.matrix(numpy.ones([numpoints,1]))*numpy.matrix(g);
            G = numpy.array(G)

            v = self.a*v + self.b*r1*(p - x) + self.c*r2*(G - x)
            p = x

            x = x + v;
            tinkercell.tc_showProgress(self.title + " round " + str(1+it) + "/" + str(multirun), int((100 * n)/maxiter))
            for i in range(0,numpoints):
                self.__setparams(x[i,:], params)
                y[i] = f()

            n += 1
            if self.minimize:
                g = x[ y.index(min(y)),: ]
            else:
                g = x[ y.index(max(y)),: ]
            for i in range(0,numvars):
                if g[i] < mins[i]: g[i] = mins[i]
                if g[i] > maxs[i]: g[i] = maxs[i]
        glist.append(g)
        tinkercell.tc_showProgress(self.title + " round " + str(1+it) + "/" + str(multirun), 100)
    if multirun > 1:
        mu = g
        m = numpy.matrix(glist)
        for i in range(0,len(g)):
            mu[i] = numpy.mean(m[:,i])
        sigma2 = numpy.cov(m)
        self.__dopca(mu, sigma2, paramnames)
    g = glist[ len(glist) - 1 ]
    self.__setparams(g, params, True)
    return g

