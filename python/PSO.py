from numpy import *
from tinkercell import *
from tc2py import *
import numpy.random
import numpy.linalg

class ParticleSwarm:
    '''Particle Swarm Optimization for TinkerCell model parameters'''
    a = 0.9
    b = 0.1
    c = 0.1
    minimize = False
    maxiter = 30
    threshold = 0.1
    logscale = False
    numpoints = 100
    title = "Particle Swarm Optimation" 

    def __setparams(self, x, m, perm=False):
        for i in range(0,len(x)):
            tc_setMatrixValue(m, i, 0, x[i]);
        if perm:
            tc_setParameters(m,1)
        else:
            tc_updateParameters(m);

    def __dopca(self, mu, sigma2, paramnames):
        allparams = tc_getParameters( tc_allItems() )
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
        if (numpy.sum(e) == 0):
            tc_print("All values are the same, so cannot analyze distribution")
        else:
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

    def run(self, f, multirun=1):
        if multirun < 1:
            multirun = 1
        glist = []
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

        numpoints = self.numpoints;
        numvars = params.rows;
        paramnames = fromTC(params.rownames)

        maxiter = self.maxiter;
        if maxiter < 1: maxiter = 1
        nmax = multirun * maxiter
        for it in range(0,multirun):
            tc_showProgress(self.title + " round " + str(1+it) + "/" + str(multirun), 0)

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
                tc_showProgress(self.title + " round " + str(1+it) + "/" + str(multirun), int((100 * n)/maxiter))
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
            tc_showProgress(self.title + " round " + str(1+it) + "/" + str(multirun), 100)
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

