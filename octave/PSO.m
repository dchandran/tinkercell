global a b c minimize maxiter threshold logscale numpoints title
a = 0.9;
b = 0.1;
c = 0.1;
minimize = 0;
maxiter = 30;
threshold = 0.1;
logscale = 0;
numpoints = 100;
title = "Particle Swarm Optimation" ;

function g = ParticleSwarm(f,multirun)
    global a b c minimize maxiter threshold logscale numpoints title
    global tinkercell
    t = 0;
    if (multirun < 1)
        multirun = 1;
    endif
    glist = [];
    n = 0;
    
    allparams = tinkercell.tc_getParameters( tinkercell.tc_allItems() );
    for i = 0:allparams.rows
        if (tinkercell.tc_getMatrixValue(allparams, i, 2) ~= tinkercell.tc_getMatrixValue(allparams, i, 1))
            n += 1;
        endif
    endfor
    if (n < 1)
        n = allparams.rows;
        for i = 0:allparams.rows
            x = tinkercell.tc_getMatrixValue(allparams, i, 0);
            if (x > 0)
                tinkercell.tc_setMatrixValue(allparams, i, 1, x/10.0);
                tinkercell.tc_setMatrixValue(allparams, i, 2, x*10.0);
            else
                if (x < 0)
                    tinkercell.tc_setMatrixValue(allparams, i, 2, x/10.0);
                    tinkercell.tc_setMatrixValue(allparams, i, 1, x*10.0);
                else
                    tinkercell.tc_setMatrixValue(allparams, i, 2, 0.0);
                    tinkercell.tc_setMatrixValue(allparams, i, 2, 1.0);
                endif
            endif
        endfor
    endif
    params = tinkercell.tc_createMatrix(n,3);
    mins = zeros(1,n);
    maxs = zeros(1,n);
    mu = zeros(1,n);
    j = 0;
    for i = 0:allparams.rows
        if (tinkercell.tc_getMatrixValue(allparams, i, 2) ~= tinkercell.tc_getMatrixValue(allparams, i, 1))
            tinkercell.tc_setMatrixValue(params, j, 0, tinkercell.tc_getMatrixValue(allparams, i, 0));
            tinkercell.tc_setMatrixValue(params, j, 1, tinkercell.tc_getMatrixValue(allparams, i, 1));
            tinkercell.tc_setMatrixValue(params, j, 2, tinkercell.tc_getMatrixValue(allparams, i, 2));
            tinkercell.tc_setRowName(params,j, tinkercell.tc_getRowName(allparams, i));
            j += 1;
            mins(j) = tinkercell.tc_getMatrixValue(allparams, i, 1);
            maxs(j) = tinkercell.tc_getMatrixValue(allparams, i, 2);
        endif
    endfor

    numvars = params.rows;
    g = zeros(1,numvars);
    if (maxiter < 1)
        maxiter = 1;
    endif
    nmax = multirun * maxiter;
    for it = 1:multirun
        tinkercell.tc_showProgress(title, 0);
        x = (ones(numpoints,1) * mins) + ( (ones(numpoints,1) * (maxs-mins)) .* rand(numpoints,numvars) );
        p = x;
        v = rand(numpoints,numvars);
        y = zeros(numpoints,1);
        for i = 1:numpoints
            q = x(i,:);
            for j = 1:length(q)
                tinkercell.tc_setMatrixValue(params, j-1, 0, q(j));
            endfor
            tinkercell.tc_updateParameters(params);
            y(i) = f();
        endfor

        if (minimize > 0)
            [values,index] = sort(y, "ascend");
            g = x( index(1) , : );
        else
            [values,index] = sort(y,"descend");
            g = x( index(1) , : );
        endif
        n = 0;
        while (n <= maxiter)
            r1 = rand(numpoints,numvars);
            r2 = rand(numpoints,numvars);
            G = ones(numpoints,1)*g;

            v = a.*v + b.*r1.*(p - x) + c.*r2.*(G - x);
            p = x;

            x = x + v;
            tinkercell.tc_showProgress(title, int32((100 * n)/maxiter));
            for i = 1:numpoints
                q = x(i,:);
                for j = 1:length(q)
                    tinkercell.tc_setMatrixValue(params, j-1, 0, q(j));
                endfor
                tinkercell.tc_updateParameters(params);
                y(i) = f();
            endfor
            n += 1;
            if (minimize > 0)
                [values,index] = sort(y, "ascend");
                g = x( index(1) , : );
            else
                [values,index] = sort(y,"descend");
                g = x( index(1) , : );
            endif
            q = g < mins;
            g(q) = mins(q);
            q = g > maxs;
            q(q) = maxs(q);
        endwhile
        glist(it,:) = g;
        tinkercell.tc_showProgress(title, 100);
    endfor

    if (multirun > 1)
        m = tinkercell.tc_createMatrix(multirun, numvars);
        for j=1:numvars
            tinkercell.tc_setColumnName(m, j-1, tinkercell.tc_getRowName(params, j-1));
        endfor
        for i=1:multirun
            for j=1:numvars
                tinkercell.tc_setMatrixValue(m, i-1, j-1, glist(i,j));
            endfor
        endfor
        tinkercell.tc_scatterplot(m, "parameters from each run");
    endif
    q = g;
    for j = 1:length(q)
        tinkercell.tc_setMatrixValue(params, j-1, 0, q(j));
    endfor
    tinkercell.tc_setParameters(params,1);
endfunction

