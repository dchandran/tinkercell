%category: Simulation
%name: Z-score Analysis
%descr: Using statistical z-score measure, identify possible areas where the deterministic simulation and stochastic simulation may vary
%menu: yes
%icon: stochastic.png
%tool: no

%tinkercell  %load tinkercell package
list_c = tc_allItems();  %list of all model items

%load parameters into octave
params_c = tc_getParameters(list_c);
for i=1:params_c.rows
	eval([tc_getRowName(params_c,i-1), "=", num2str(tc_getMatrixValue(params_c,i,0)),";"]);
end

%get inputs from user
time = tc_getNumber("Simulation time");
options_c = tc_createStringsArray(2);
tc_setString(options_c, 0, "Using all noise");
tc_setString(options_c, 1, "Using only opposing noise");
analysisType = tc_getStringFromList("How to calculate noise influence", options_c, "Using only opposing noise");

if (time > 0)
	tc_showProgress("Z-score test", int32(0))  %show progress meter
	
	%ODE simulation
	y = tc_simulateDeterministic(0, time, 100);
	dt = time/100;

	%plot
	tc_multiplot(2,2);
	tc_plot(y, "Simulation");

	%create matrices for recording z-scores, curvature, and variance
	tc_showProgress("Z-score test", 10);
	N_c = tc_getStoichiometry(list_c);

	z = tc_createMatrix(y.rows,N_c.rows + 1);   %z-scores
	sigma = tc_createMatrix(y.rows,N_c.rows + 1); %variance
	curvature = tc_createMatrix(y.rows,N_c.rows + 1);  %curvature

	%set row and column names
	tc_setColumnName(z, 0, "Time");
	tc_setColumnName(sigma, 0, "Time");
	tc_setColumnName(curvature, 0, "Time");
	for i=1:N_c.rows
		tc_setColumnName(z,i, tc_getRowName(N_c, i-1));
		tc_setColumnName(sigma,i, tc_getRowName(N_c, i-1));
		tc_setColumnName(curvature, i, tc_getRowName(N_c, i-1));
	end

	rates_c = tc_getRates(list_c);  %get reaction rate equations for all reactions
	N = fromTC(N_c);   %convert TinkerCell matrix to octave matrix
	rates = zeros( size(N,2), 1 ); %will use later

	for i=0:(y.rows-1)   %for each row in simulated result

		tc_showProgress("Z-score test", int32(10 + 90*(i/y.rows))) %show progress bar in TinkerCell

		%create variable names inside octave from the simulation data
		for j=1:(y.cols-1)
		    name = tc_getColumnName(y, j);   %get variable name
		    value = tc_getMatrixValue(y, i, j);  %get variable value at row i
		    eval([name, "=", num2str(value),";"]);  %set name = value
		end

		for j=1:N_c.cols
		    rates(j) = eval(tc_getString(rates_c, j-1));  %evalute each rate equation
		end

		n = size(N,1);
		m = size(N,2);
		du = N*rates;   % diff.eqs = [N] x [Rates]
		for j=1:n    %for each row in N
			v = zeros(1,m);
			if (du(j) > 0)  %if diff.eq is positive
		        q = find(N(j,:) < 0);
		    else           %if negative
		        q = find(N(j,:) > 0);
			end
			if (analysisType == 1)
				v(q) = abs(N(j,q));   %get reactions in the opposite direction
			else
				v = N(j,:);
			end
			sd = abs(v)*sqrt(abs(rates));   %get st.dev of opposing reactions
			if (sd == 0)
				tc_setMatrixValue(z, i, j, -1.0);   %no variance in opposing direction
			else
				tc_setMatrixValue(z, i, j, abs(du(j))/sd);   %Z SCORE using only st.dev in the opposite direction
			end
		    tc_setMatrixValue(sigma, i, j, v * sqrt(abs(rates)) );  %St.dev of the diff.eqns. in opposing direction
		    
		    if (i > 0 && i < (y.rows-1)) % this might be a critical value
		        x1 = tc_getMatrixValue(sigma, i, j);
		        x0 = tc_getMatrixValue(sigma, i-1, j);
		        x2 = tc_getMatrixValue(sigma, i+1, j);
		        second_deriv = (x0 - 2*x1 + x2)/(dt*dt);
				tc_setMatrixValue(curvature, i, j, second_deriv);  %store 2nd derivative in table
			else
				tc_setMatrixValue(curvature, i, j, 0);  % below z*, so use 0
			end
		end
		tc_setMatrixValue(z, i, 0, tc_getMatrixValue(y,i,0) );
		tc_setMatrixValue(sigma, i, 0, tc_getMatrixValue(y,i,0) );
		tc_setMatrixValue(curvature, i, 0, tc_getMatrixValue(y,i,0) );
	end

	tc_showProgress("Z-score test", int32(100))
	tc_plot(z, "Z score");
	tc_plot(sigma, "St.Dev.");
	tc_plot(curvature, "Curvature");
end

